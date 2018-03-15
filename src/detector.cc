//  Created by Chris Morgan on 22/12/17.
//  Copyright © 2017 Chris Morgan. All rights reserved.

#ifdef __cplusplus
#import <cmath>
#import <algorithm>

#import "detector.h"
#include "pipeline_job.h"
#include "algorithms/algorithms.h"

Detector::Detector() :
    frame_counter(0),
    num_frames_since_doc_detected(1000),
    rescale_factor(1)
{
}

void Detector::reset() {
    frame_counter = 0;
    num_frames_since_doc_detected = 1000;

    detected_document.reset();
    last_valid_document.reset();
}

bool Detector::initialize(cv::String path_to_model)
{
    edge_detector = cv::ximgproc::createStructuredEdgeDetection(path_to_model);
    return true;
}

void Detector::process_image(cv::Mat& original_image, EDGE_DETECTORS edge_method)
{
    cv::Mat imgResult;

    frame_counter++;

    calculate_rescale_factor(original_image);
    scale_image(original_image);

    PipelineJob job(scaled_image);
    CannyEdgeDetector canny_detector;
    TreesEdgeDetector trees_detector(edge_detector);
    LineDetector line_detector;
    RectangleDetector rectangle_detector;
    BestRectangleSelector rectangle_selector;
    
    canny_detector.apply(job);
    line_detector.apply(job);
    rectangle_detector.apply(job);
    rectangle_selector.apply(job);
    
#ifdef DEBUG_DRAWING
    DebugDrawings draw_detected_objects;
    draw_detected_objects.apply(job);
#endif
    
    detected_document = job.get_result();
    
    if (detected_document.is_valid() || found_document()) {
        original_image.copyTo(scaled_image);
        if (detected_document.is_valid()) {
            last_valid_document = detected_document;
            last_valid_document.rescale(original_image);
        }

        last_valid_document.draw(original_image, cv::Scalar(0,255,0,255));
    }
    else {
#ifdef DEBUG_DRAWING
        rescale_image(original_image);
#else
        original_image.copyTo(scaled_image);
#endif
    }

    update_detection_state(detected_document);
}

void Detector::calculate_rescale_factor(cv::Mat& original_image) {
    int minDim = std::min(original_image.cols, original_image.rows);
    int scale = ceil(minDim / 360) + 1;
    rescale_factor = ceil(log(scale)/log(2));
}

void Detector::scale_image(cv::Mat& original_image) {
    for (int i = 0;i < rescale_factor;i++) {
        if (i) {
            cv::pyrDown(scaled_image, scaled_image);
        }
        else {
            cv::pyrDown(original_image, scaled_image);
        }
    }
}

void Detector::rescale_image(cv::Mat& original_image) {
    //cv::resize(m_imgSaved, original_image, cv::Size(), scale,scale);
    for (int i = 0;i < rescale_factor;i++) {
        if (i) {
            cv::pyrUp(original_image, original_image);
        }
        else {
            cv::pyrUp(scaled_image, original_image);
        }
    }
}

void Detector::update_detection_state(const Document& rectangle) {
    if (rectangle.is_valid()) {
        num_frames_since_doc_detected = 0;
    }
    else {
        num_frames_since_doc_detected++;
    }
}

bool Detector::copy_deskewed_doc_region(cv::Mat& src_image, cv::Mat& dest_image)
{
    if (found_document()) {
        cv::Mat resized;
        
        // Scale
        Document copy_document = detected_document;
        copy_document.rescale(src_image);
        copy_document.copy_deskewed_document(src_image, resized);
        
        cv::Size orig_size(src_image.cols, src_image.rows);
        
        cv::resize(resized, dest_image, orig_size, 0, 0, cv::INTER_LANCZOS4);
        return true;
    } else {
        dest_image = src_image;
        return false;
    }
}

void Detector::get_document_points(std::vector<cv::Point>& points) const {
    last_valid_document.getPoints(points);
}

bool Detector::found_document() const {
    return (last_valid_document.is_valid() || \
            num_frames_since_doc_detected < MAX_FAILED_DETECTIONS);
}

#endif
