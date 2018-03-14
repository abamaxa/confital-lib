//  Copyright © 2018 Chris Morgan. All rights reserved.

#ifndef pipeline_job_hpp
#define pipeline_job_hpp

#include "common.h"

class PipelineJob {
public:
    cv::Mat& image;
    const cv::Point& image_dimensions;
    
    int image_width() const;
    int image_height() const;
};

#endif /* pipeline_job_hpp */
