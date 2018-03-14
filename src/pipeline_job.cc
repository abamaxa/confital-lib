//  Copyright © 2018 Chris Morgan. All rights reserved.


#include "pipeline_job.h"

PipelineJob::PipelineJob(cv::Mat& _image)
: image(_image)
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

void PipelineJob::add_line(const Line& line) {
    if (!line.is_similar_line(line_candidates, image_width(), image_height())) {
        line_candidates.push_back(line);
    }
}
