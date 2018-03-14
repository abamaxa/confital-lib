//  Copyright © 2018 Chris Morgan. All rights reserved.

#ifndef pipeline_job_hpp
#define pipeline_job_hpp

#include "common.h"
#include "line.h"
#include "document.h"

class PipelineJob {
public:
    PipelineJob(cv::Mat& image);
    
    int image_width() const;
    int image_height() const;
    cv::Mat& get_image();
    void add_line(const Line& line);
    
    LineVector line_candidates;
private:
    cv::Mat& image;
    
    LineVector selected_candidates;
    std::vector<Document> rectangles;
};

#endif /* pipeline_job_hpp */
