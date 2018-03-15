//
//  rectangle.hpp
//  confital-lib-mactest
//
//  Created by Chris Morgan on 15/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#ifndef rectangle_hpp
#define rectangle_hpp
/*
#include "config.h"
#include "line.h"
#include "pipeline_job.h"

class Rectangle {
public:
    Document();
    Document(const Document& toCopy);
    Document(const cv::Mat& image);
    
    bool assessRectangle(
                         const Line& principleSide,
                         const Line& normalSide1,
                         const Line& oppositeSide,
                         const Line& normalSide2,
                         const PipelineJob& job
                         );
    
    void reset();
    
    void rescale(const cv::Mat& image);
    
    void draw(cv::Mat& image, const cv::Scalar& color) const;
    void getPoints(std::vector<cv::Point>& points) const;
    bool is_valid() const;
    
private:
    cv::Point m_bottom_left;
    cv::Point m_bottom_right;
    cv::Point m_top_right;
    cv::Point m_top_left;
    
    void calculate_intersections(const Line** lines);
    bool intersections_are_close(const PipelineJob& job) const;
    bool orderSides(const Line** sides) const;

    float distancePoints(const cv::Point& pt1, const cv::Point& pt2) const;
    float calcAngle(float len_a, float len_b, float len_c) const;
};*/

#endif /* rectangle_hpp */
