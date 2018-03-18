//  Created by Chris Morgan on 22/12/17.
//  Copyright © 2017 Chris Morgan. All rights reserved.

#include <cmath>
#include <algorithm>

#include "detector.h"
#include "pipeline_job.h"
#include "algorithms/algorithms.h"

const int INITIAL_FRAMES_SINCE_DETECTED_DOC_BIG_NUM = 10000;

Detector::Detector() :
    frame_counter(0),
    num_frames_since_doc_detected(INITIAL_FRAMES_SINCE_DETECTED_DOC_BIG_NUM),
    rescale_factor(1),
    show_detected_lines(false)
{
}

Detector::~Detector() {
}

void Detector::set_debug_show_detected_lines(bool show) {
    show_detected_lines = show;
}

void Detector::assemble_default_pipeline() {
    pipeline.clear();
    
    pipeline.push_back(std::make_shared<CannyEdgeDetector>());
    //pipeline.push_back(std::make_shared<TreesEdgeDetector>());
    pipeline.push_back(std::make_shared<LineDetector>());
    pipeline.push_back(std::make_shared<LineGatherer>());
    pipeline.push_back(std::make_shared<RectangleDetector>());
    pipeline.push_back(std::make_shared<BestRectangleSelector>());
    
    if (show_detected_lines)
        pipeline.push_back(std::make_shared<DebugDrawings>());
    
    load_algorithm_models();
}

void Detector::load_algorithm_models() {
    for (auto algorithm : pipeline) {
        auto itr = model_paths.find(algorithm->get_name());
        if (itr != model_paths.end())
            algorithm->load((*itr).second);
    }
}

void Detector::add_pipeline_stage(Algorithm algorithm) {
    pipeline.push_back(algorithm);
}

void Detector::set_model_filepath(std::string model_name, std::string path) {
    model_paths.insert({model_name, path});
}

void Detector::detect(cv::Mat& original_image) {
    calculate_rescale_factor(original_image);
    scale_image(original_image);
    
    PipelineJob job(scaled_image);
    
    if (!pipeline.size())
        assemble_default_pipeline();
    
    for (auto algorithm : pipeline) {
        algorithm->apply(job);
    }

    update_detection_state(job.get_result());
    update_last_detected_document(original_image);
}

void Detector::calculate_rescale_factor(cv::Mat& original_image) {
    int minDim = std::min(original_image.cols, original_image.rows);
    int scale = ceil(minDim / 360) + 1;
    rescale_factor = ceil(log(scale)/log(2));
}

void Detector::scale_image(cv::Mat& original_image) {
    for (int i = 0;i < rescale_factor;i++) {
        if (i)
            cv::pyrDown(scaled_image, scaled_image);
        else
            cv::pyrDown(original_image, scaled_image);
    }
}

void Detector::update_detection_state(const Document& document) {
    detected_document = document;
    frame_counter++;
    
    if (document.is_valid())
        num_frames_since_doc_detected = 0;
    else
        num_frames_since_doc_detected++;
}

void Detector::update_last_detected_document(cv::Mat& original_image) {
    if (detected_document.is_valid()) {
        most_recent_detected_document = detected_document;
        most_recent_detected_document.rescale(original_image);
    }
}

void Detector::highlight_most_recent_detected_document(cv::Mat& original_image) const {
    if (found_document())
        most_recent_detected_document.draw(original_image, cv::Scalar(0,255,0,255));
}

bool Detector::found_document() const {
    return (most_recent_detected_document.is_valid() &&
            num_frames_since_doc_detected < MAX_FAILED_DETECTIONS);
}

void Detector::copy_deskewed_doc_region(cv::Mat& src_image, cv::Mat& dest_image)
{
    if (found_document()) {
        cv::Mat resized;

        Document copy_document = detected_document;
        copy_document.rescale(src_image);
        copy_document.copy_deskewed_document(src_image, resized);
        
        cv::Size orig_size(src_image.cols, src_image.rows);
        cv::resize(resized, dest_image, orig_size, 0, 0, cv::INTER_LANCZOS4);
    } 
}

void Detector::get_document_points(Points& points) const {
    const Points& doc_points = most_recent_detected_document.get_points();
    points.clear();
    points.insert(points.begin(), doc_points.begin(), doc_points.end());
}

void Detector::reset() {
    frame_counter = 0;
    num_frames_since_doc_detected = INITIAL_FRAMES_SINCE_DETECTED_DOC_BIG_NUM;
    detected_document.reset();
    most_recent_detected_document.reset();
}
