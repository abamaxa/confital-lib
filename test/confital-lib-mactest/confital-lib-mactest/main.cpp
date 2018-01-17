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
#include <vector>
#include "MTDetector.h"


const char* IMAGE_PATHS[] = {
    "messed.png",
    "skewed-nolines.png",
    "skewed-nolines2.png",
    "skewed-failed-to-detect-edge.png",
    "skewed1.png",
    "table-detect-half-page.png",
    "table-failed-to-detect.png",
    "table-failed-to-detect2.png",
    "table-no-detect3.png"
};

const size_t NUM_IMAGES = sizeof(IMAGE_PATHS) / sizeof(const char*);
const char* IMAGE_DIR = "../../../../../../images";
const char* SAVED_DIR = "../../../../../../images_saved";
const char* MODEL_FILE = "../../../../../../../src/model.yml.gz";

void loadImages(std::vector<cv::Mat>& images) {
    for (int i = 0;i < NUM_IMAGES;i++) {
        char path[0x100];
        cv::Mat image;
        sprintf(path, "%s/%s", IMAGE_DIR, IMAGE_PATHS[i]);
        image = cv::imread(path);
        images.push_back(image);
    }
}

int doDetection(MTDetector& detector, std::vector<cv::Mat>& images, bool save) {
    int rows = 0;
    for (int i = 0;i < images.size();i++) {
        detector.reset();
        detector.processImage(images[i], MT_EDGE_DETECT_TREES);
        
        cv::Mat result;
        detector.getDocumentImage(result);
        
        if (save) {
            char path[0x100];
            sprintf(path, "%s/%s", SAVED_DIR, IMAGE_PATHS[i]);
            cv::imwrite(path, images[i]);
        }
        
        rows += result.rows;
        //break;
    }
    return rows;
}

int main(int argc, char** argv)
{
    int rows = 0;
    std::vector<cv::Mat> images;
    MTDetector detector;
    
    detector.initialize(MODEL_FILE);
    
    loadImages(images);
    
    /*for (int i = 0;i < 100;i++) {
        rows += doDetection(detector, images, false);
    }*/
    
    doDetection(detector, images, true);
    
    return 0;
}


