//  Created by Chris Morgan on 16/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#include "object_detector.h"

cv::dnn::Net ObjectDetector::net;
std::string model = "";

cv::dnn::Net& ObjectDetector::load_and_return_net() {
    if (net.empty()) {
        net = cv::dnn::readNetFromTensorflow(model);
    }
    if (net.empty()) {
        std::cerr << "Could not load net " << model << "\n";
    }
    return net;
}

void ObjectDetector::apply(PipelineJob& job) {
    
    cv::Mat image = job.get_image();
    cv::Size size(image.cols, image.rows);
    cv::Scalar normalise(127.5, 127.5, 127.5);
    
    cv::Mat img_copy(image);
    cv::Mat original_img(image);
    image.convertTo(image, CV_32FC3);
    
    cv::Mat input_blob = cv::dnn::blobFromImage(image, 1.0/127.5,
                                        size, normalise, false);
    
    std::vector<cv::Mat> outblobs(3);
    std::vector<cv::String> out_layers;
    
    out_layers.push_back("slice");
    out_layers.push_back("softmax");
    out_layers.push_back("sigmoid");
    
    // Bbox delta blob
    std::vector<cv::Mat> temp_blob;
    net.setInput(input_blob);
    cv::TickMeter t;
    
    t.start();
    net.forward(temp_blob, out_layers[0]);
    t.stop();
    outblobs[0] = temp_blob[2];
    
    // class_scores blob
    net.setInput(input_blob);
    t.start();
    outblobs[1] = net.forward(out_layers[1]);
    t.stop();
    
    // conf_scores blob
    net.setInput(input_blob);
    t.start();
    outblobs[2] = net.forward(out_layers[2]);
    t.stop();
    
    // Check that the blobs are valid
    for (size_t i = 0; i < outblobs.size(); ++i)
    {
        if (outblobs[i].empty())
        {
            std::cerr << "Blob: " << i << " is empty !\n";
        }
    }
    
    int delta_bbox_size[3] = {23, 23, 36};
    cv::Mat delta_bbox(3, delta_bbox_size, CV_32F, outblobs[0].ptr<float>());
    
    int class_scores_size[2] = {4761, 20};
    cv::Mat class_scores(2, class_scores_size, CV_32F, outblobs[1].ptr<float>());
    
    int conf_scores_size[3] = {23, 23, 9};
    cv::Mat conf_scores(3, conf_scores_size, CV_32F, outblobs[2].ptr<float>());
    
    double threshold = 0.7;
    cv::dnn_objdetect::InferBbox inf(delta_bbox, class_scores, conf_scores);
    inf.filter(threshold);
    
    
    double average_time = t.getTimeSec() / t.getCounter();
    std::cout << "\nTotal objects detected: " << inf.detections.size()
    << " in " << average_time << " seconds\n";
    std::cout << "------\n";
    float x_ratio = (float)original_img.cols / img_copy.cols;
    float y_ratio = (float)original_img.rows / img_copy.rows;
    for (size_t i = 0; i < inf.detections.size(); ++i)
    {
        
        int xmin = inf.detections[i].xmin;
        int ymin = inf.detections[i].ymin;
        int xmax = inf.detections[i].xmax;
        int ymax = inf.detections[i].ymax;
        cv::String class_name = inf.detections[i].label_name;
        std::cout << "Class: " << class_name << "\n"
        << "Probability: " << inf.detections[i].class_prob << "\n"
        << "Co-ordinates: " << inf.detections[i].xmin << " "
        << inf.detections[i].ymin << " "
        << inf.detections[i].xmax << " "
        << inf.detections[i].ymax << "\n";
        std::cout << "------\n";
        // Draw the corresponding bounding box(s)
        cv::rectangle(original_img, cv::Point((int)(xmin * x_ratio), (int)(ymin * y_ratio)),
                      cv::Point((int)(xmax * x_ratio), (int)(ymax * y_ratio)), cv::Scalar(255, 0, 0), 2);
        cv::putText(original_img, class_name, cv::Point((int)(xmin * x_ratio), (int)(ymin * y_ratio)),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 0, 0), 1);
    }
}
