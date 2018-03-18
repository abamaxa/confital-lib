//
//  trees_edge_detector.cpp
//  confital-lib-mactest
//
//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#include "edge_detector.h"

void CannyEdgeDetector::apply(PipelineJob& job) {
    cv::Mat& image = job.get_image();
    cv::Mat image_gray;
    
    cv::cvtColor(image, image_gray, CV_BGR2GRAY);
    cv::medianBlur(image_gray, image_gray, 11);
    cv::Canny(image_gray, image, 75, 200);
}

TreesEdgeDetector::TreesEdgeDetector() {
    
}

void TreesEdgeDetector::apply(PipelineJob& job) {
    assert(edge_detector);
    
    cv::Mat imgTemp, image_CV_32FC3;
    cv::Mat& image = job.get_image();
    
    cv::medianBlur(image, imgTemp, 11);
    cv::cvtColor(imgTemp, imgTemp, CV_BGR2RGB);
    //cv::cvtColor(image, imgTemp, CV_BGR2RGB);
    
    imgTemp.convertTo(image_CV_32FC3, CV_32FC3, 1/255.0);
    image = cv::Mat(image_CV_32FC3.size(), image_CV_32FC3.type());
    
    //NSLog(@"Orig type %d, Type %d (%d)", image.type(), image_CV_32FC3.type(), CV_32FC3);
    edge_detector->detectEdges(image_CV_32FC3, image);
    
    // computes orientation from edge map
    cv::Mat orientation_map;
    edge_detector->computeOrientation(image, orientation_map);
    
    // suppress edges
    cv::Mat edge_nms;
    edge_detector->edgesNms(image, orientation_map, edge_nms, 2, 0, 1, true);
    
    edge_nms.convertTo(image, CV_8UC1, 255.);
}

bool TreesEdgeDetector::load(std::string path_to_model) {
    if (path_to_model.size()) {
        cv::String path(path_to_model);
        edge_detector = cv::ximgproc::createStructuredEdgeDetection(path);
    }
    
    return (edge_detector != nullptr);
}

