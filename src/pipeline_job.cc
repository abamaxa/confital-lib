//  Copyright © 2018 Chris Morgan. All rights reserved.


#include "pipeline_job.h"

PipelineJob::PipelineJob(const cv::Mat& _image)
: image(_image), initial_image(_image)
{
    
}

int PipelineJob::image_width() const {
    return image.cols;
}

int PipelineJob::image_height() const {
    return image.rows;
}

cv::Mat& PipelineJob::get_image() {
    return image;
}

void PipelineJob::add_line(const Line& candidate) {
    cv::Rect image_bounds(0, 0, image_width(), image_height());
    
    for (const auto& line : line_candidates) {
        if (line.is_similar_line(candidate, image_bounds))
            return;
    }
    
    line_candidates.push_back(candidate);
}

LineVector& PipelineJob::get_lines() {
    return line_candidates;
}

const cv::Mat& PipelineJob::get_initial_image() const {
    return initial_image;
}

void PipelineJob::add_rectangle(Document rectangle) {
    rectangles.push_back(rectangle);
}

std::vector<Document>& PipelineJob::get_rectangles() {
    return rectangles;
}

void PipelineJob::set_result(const Document& rectangle) {
    result_rectangle = rectangle;
}

const Document& PipelineJob::get_result() const {
    return result_rectangle;
}

void PipelineJob::set_parallel_line_groups(const ApproxParallelLineGroup& _grps)
{
    groups = _grps;
}

const ApproxParallelLineGroup& PipelineJob::get_parallel_line_groups() const {
    return groups;
}



