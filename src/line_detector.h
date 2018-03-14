//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.

#ifndef line_detector_hpp
#define line_detector_hpp

#include "common.h"
#include "pipeline_job.h"
#include "line.h"

class LineDetector {
public:
    LineDetector(const PipelineJob& job, size_t max_lines);
    void apply(cv::Mat& image);
    
    private :
    void find_hough_lines(cv::Mat& image);
    void find_hough_lines_p(cv::Mat& image);
    
    size_t max_lines;
    const PipelineJob& job;
    LineVector line_candidates;
};


#endif /* line_detector_hpp */
