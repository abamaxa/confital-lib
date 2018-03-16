//  Created by Chris Morgan on 22/12/17.
//  Copyright © 2017 Chris Morgan. All rights reserved.

#ifndef Detector_h
#define Detector_h

#ifdef __cplusplus
#include "common.h"
#include "config.h"
#include "line.h"
#include "document.h"

enum EDGE_DETECTORS {
    MT_EDGE_DETECT_CANNY,
    MT_EDGE_DETECT_TREES
};

class Detector {
public:
    Detector();
    virtual ~Detector();
    
    virtual void initialize(cv::String path_to_model);
    virtual void process_image(cv::Mat& original_image);
    virtual bool found_document() const;
    virtual void copy_deskewed_doc_region(cv::Mat& src_image, cv::Mat& dest_image);
    virtual void get_document_points(std::vector<cv::Point>& points) const;
    virtual void draw_most_recent_detected_document(cv::Mat& original_image) const;
    virtual void reset();
    
private:
    void calculate_rescale_factor(cv::Mat& original_image);
    void scale_image(cv::Mat& original_image);
    void update_detection_state(const Document& document);
    void update_last_detected_document(cv::Mat& original_image);
    
    cv::Ptr<cv::ximgproc::StructuredEdgeDetection> edge_detector;
    int frame_counter;
    cv::Mat scaled_image;
    Document detected_document;
    Document most_recent_detected_document;
    int num_frames_since_doc_detected;
    int rescale_factor;
};

#endif /* __cplusplus */
#endif /* Detector_h */
