//
//  main.cpp
//  many-tutor-test
//
//  Created by Chris Morgan on 16/1/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

// Example showing how to read and write images
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "detection_results.h"
#include "detector.h"
#include "constants.h"

//#define DETECTION_MODE MT_EDGE_DETECT_TREES
#define DETECTION_MODE MT_EDGE_DETECT_CANNY

class Test {
public :
    Test(bool _save_images) : fails(0), save_images(_save_images) {
        load_images();
        detector.initialize(MODEL_FILE);
    }
    
    void test_all() {
        for (int i = 0;i < images.size();i++) {
            test_one(i);
        }
    }
    
    void test_one(int ordinal) {
        srand(42);
        
        detector.reset();
        
        cv::Mat test_image;
        images[ordinal].copyTo(test_image);
        
        detector.processImage(test_image, DETECTION_MODE);
        
        check_detection(ordinal);
        
        get_document_image(ordinal);
        
        if (save_images) {
            save_detected_image(test_image, ordinal);
        }
    }
    
    cv::Mat get_document_image(int ordinal) {
        cv::Mat result;
        detector.getDocumentImage(result);
        return result;
    }
    
    int get_number_fails() const {
        return fails;
    }
    
    void report_fails() const {
        std::cout << "Completed with " << get_number_fails() << " fails.\n";
    }
    
private :
    void load_images() {
        for (int i = 0;i < NUM_IMAGES;i++) {
            char path[0x100];
            cv::Mat image;
            sprintf(path, "%s/%s", IMAGE_DIR, IMAGE_PATHS[i]);
            image = cv::imread(path);
            images.push_back(image);
        }
    }
    
    void check_detection(int ordinal) {
        Points points;
        detector.getDocumentPoints(points);
        
        if (saved_results.has_result(ordinal)) {
            DetectionResult detection_result = saved_results.get(ordinal);
            if (!detection_result.compare(points)) {
                std::cout << "Test failed for image " \
                << IMAGE_PATHS[ordinal] << "\n";
                fails++;
            }
            
        } else {
            DetectionResult detection_result = DetectionResult(
                                            IMAGE_PATHS[ordinal], points);
            detection_result.write(std::cout);
        }
    }
    
    void save_detected_image(const cv::Mat& image, int ordinal) const {
        char path[0x100];
        sprintf(path, "%s/%s", SAVED_DIR, IMAGE_PATHS[ordinal]);
        cv::imwrite(path, image);
    }
    
    std::vector<cv::Mat> images;
    DetectionResults saved_results;
    Detector detector;
    int fails;
    bool save_images;
};

void help() {
    std::cout \
    << "Usage:\n" \
    << "    -c <number> : Multiple passes through images, for speed tests\n" \
    << "    -n <number> : Test only image number\n";
}

int main(int argc, char** argv)
{
    if (argc == 1) {
        Test tester = Test(true);
        tester.test_all();
        tester.report_fails();
    }
    else if (argc != 3) {
        help();
    }
    else {
        const char* command = argv[1];
        int number = std::stoi(argv[2]);
        
        if (strcmp(command,"-c")) {
            Test tester = Test(false);
            for (int i = 0; i < number;i++) {
                tester.test_all();
            }
        }
        else if (strcmp(command,"-n")) {
            Test tester = Test(true);
            tester.test_one(number);
            tester.report_fails();
        }
        else {
            help();
        }
    }
    
    return 0;
}




