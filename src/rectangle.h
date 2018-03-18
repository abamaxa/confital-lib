//  Created by Chris Morgan on 15/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#ifndef rectangle_hpp
#define rectangle_hpp

#include "common.h"
#include "config.h"
#include "line.h"
#include "line_group.h"

const int TOP_SIDE    = 0;
const int RIGHT_SIDE  = 1;
const int BOTTOM_SIDE = 2;
const int LEFT_SIDE   = 3;

const int NUM_SIDES   = 4;

class Rectangle {
public:
    Rectangle();
    bool set(LineVector& lines,const cv::Mat& image);
    
    void reset();
    void rescale(const cv::Mat& image);
    void draw(cv::Mat& image, const cv::Scalar& color) const;
    const Points& get_points() const;
    
    float get_top_length() const;
    float get_bottom_length() const;
    float get_left_length() const;
    float get_right_length() const;
    float get_top_right_to_bottom_left_length() const;
    float get_top_left_to_bottom_right_length() const;
    
    float get_angle_between_top_bottom_sides() const;
    float get_angle_between_left_right_sides() const;
    
private:
    bool calculate_intersections(LineVector& lines);
    bool intersections_are_close() const ;
    bool intersection_in_or_near_image(const cv::Point& intersection) const;
    bool are_lines_within_corners(const LineVector& lines) const;
    void calculate_side_lengths();
    void set_angles(const LineVector& lines);
    void set_corners(cv::Point* points);
    bool order_lines(LineVector& lines, cv::Point* points);
    float distance_points(const cv::Point& pt1, const cv::Point& pt2) const;
    
    Points corners;
    std::vector<float> angles;
    int image_width;
    int image_height;
};

#endif /* rectangle_hpp */
