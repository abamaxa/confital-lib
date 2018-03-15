//  Created by Chris Morgan on 22/12/17.
//  Copyright © 2017 Chris Morgan. All rights reserved.

#ifndef Detector_h
#define Detector_h

#import <opencv2/opencv.hpp>
#import <opencv2/ximgproc.hpp>
#import <opencv2/highgui.hpp>

#import "config.h"
#import "line.h"
#import "document.h"

enum EDGE_DETECTORS {
    MT_EDGE_DETECT_CANNY,
    MT_EDGE_DETECT_TREES
};

class Detector {
public:
    Detector();

    virtual bool initialize(cv::String path_to_model);
    virtual void process_image(cv::Mat& original_image);
    virtual bool found_document() const;
    virtual bool copy_deskewed_doc_region(cv::Mat& src_image, cv::Mat& dest_image);
    virtual void get_document_points(std::vector<cv::Point>& points) const;
    virtual void reset();
    
private:
    void calculate_rescale_factor(cv::Mat& original_image);
    void scale_image(cv::Mat& original_image);
    void rescale_image(cv::Mat& original_image);
    void update_detection_state(const Document& rectangle);

    cv::Ptr<cv::ximgproc::StructuredEdgeDetection> edge_detector;
    int frame_counter;
    cv::Mat scaled_image;
    Document detected_document;
    Document last_valid_document;
    int num_frames_since_doc_detected;
    int rescale_factor;
};

#endif /* Detector_h */
