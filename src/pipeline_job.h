//  Copyright © 2018 Chris Morgan. All rights reserved.

#ifndef pipeline_job_hpp
#define pipeline_job_hpp

#include "common.h"
#include "line.h"
#include "line_group.h"
#include "document.h"

typedef std::unordered_map<std::string, float> ScoreMap;

class PipelineJob : public ScoreMap {
public:
    PipelineJob(const cv::Mat& image);
    
    int image_width() const;
    int image_height() const;
    
    cv::Mat& get_image();
    const cv::Mat& get_initial_image() const;
    
    void set_parallel_line_groups(const ApproxParallelLineGroup& groups);
    const ApproxParallelLineGroup& get_parallel_line_groups() const;
    
    LineVector& get_lines();
    void add_line(const Line& line);
    
    std::vector<Document>& get_rectangles();
    void add_rectangle(Document rectangle);
    
    void set_result(const Document& rectangle);
    const Document& get_result() const;
    
private:
    cv::Mat image;
    const cv::Mat& initial_image;

    Document result_rectangle;
    
    LineVector line_candidates;
    ApproxParallelLineGroup groups;
    std::vector<Document> rectangles;
};

#endif /* pipeline_job_hpp */
