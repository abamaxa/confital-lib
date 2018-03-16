//  Created by Chris Morgan on 15/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#include "rectangle.h"

const int TOP_LEFT = 0;
const int TOP_RIGHT = 1;
const int BOTTOM_RIGHT = 2;
const int BOTTOM_LEFT = 3;
const int UNMATCHED = -1;

int find_side(const Line** lines, cv::Point* points, cv::Point corner1, cv::Point corner2);

Rectangle::Rectangle() :
    image_width(0),
    image_height(0)
{
    reset();
}

void Rectangle::reset() {
    corners.clear();
    corners.resize(NUM_SIDES);
    angles.clear();
    angles.resize(NUM_SIDES);
}

bool Rectangle::set
(
    const Line& principle,
    const Line& normal1,
    const Line& opposite,
    const Line& normal2,
    const cv::Mat& image
)
{
    image_width = image.cols;
    image_height = image.rows;
    
    const Line* lines[] = {&principle, &normal1, &opposite, &normal2};
    if (!calculate_intersections(lines))
        return false;
    
    if (!are_lines_within_corners(lines))
        return false;

    if (!intersections_are_close())
        return false;
    
    set_angles(lines);
    
    return true;
}

bool Rectangle::calculate_intersections(const Line** lines) {
    cv::Point points[4];
    
    lines[0]->find_intersection(*lines[1], points[0]);
    lines[0]->find_intersection(*lines[3], points[1]);
    lines[2]->find_intersection(*lines[1], points[2]);
    lines[2]->find_intersection(*lines[3], points[3]);
    
    set_corners(points);
    return order_lines(lines, points);
}

bool compare_y(cv::Point& a, cv::Point& b) {
    return a.y < b.y;
}

bool compare_x_top(cv::Point& a, cv::Point& b) {
    return a.x < b.x;
}

bool compare_x_bottom(cv::Point& a, cv::Point& b) {
    return a.x > b.x;
}

void Rectangle::set_corners(cv::Point* original_points) {
    // Sort in order of top-left, top-right, bottom-right, bottom-left
    assert(corners.size() == NUM_SIDES);
    std::copy(original_points, original_points + NUM_SIDES, corners.begin());
    
    std::sort(corners.begin(), corners.end(), compare_y);
    
    // top most
    std::sort(corners.begin(), corners.begin() + 2, compare_x_top);
    
    // bottom most
    std::sort(corners.begin() + 2, corners.end(), compare_x_bottom);
}

bool Rectangle::order_lines(const Line** lines, cv::Point* points) {
    const Line* ordered[NUM_SIDES];

    for (size_t side = 0;side < NUM_SIDES;side++) {
        const cv::Point& corner1 = corners[side];
        const cv::Point& corner2 = corners[(side + 1) % NUM_SIDES];

        int position = find_side(lines, points, corner1, corner2);
        if (position == UNMATCHED)
            return false;
        
        ordered[side] = lines[position];
    }
    
    memcpy(lines, ordered, sizeof(ordered));
    return true;
}

int find_side(const Line** lines, cv::Point* points, cv::Point corner1, cv::Point corner2)
{
    
    int side_matching_corner1_x = UNMATCHED;
    int side_matching_corner2_x = UNMATCHED;
    int side_matching_corner1_y = UNMATCHED;
    int side_matching_corner2_y = UNMATCHED;
    
    for (int x = 0;x < 3;x += 2) {
        for (int y = 1;y < 4;y += 2) {
            if (*points == corner1) {
                side_matching_corner1_x = x;
                side_matching_corner1_y = y;
            }
            if (*points == corner2) {
                side_matching_corner2_x = x;
                side_matching_corner2_y = y;
            }
            points++;
        }
    }
    
    if (side_matching_corner1_x != UNMATCHED && side_matching_corner2_x != UNMATCHED) {
        if (side_matching_corner1_x == side_matching_corner2_x) {
            return side_matching_corner1_x;
        }
        if (side_matching_corner1_y == side_matching_corner2_y) {
            return side_matching_corner1_y;
        }
    }
    
    return UNMATCHED;
}

bool Rectangle::intersections_are_close() const {
    for (size_t i = 0;i < NUM_SIDES;i++) {
        if (!intersection_in_or_near_image(corners.at(i)))
            return false;
    }

    return true;
}

bool Rectangle::intersection_in_or_near_image(const cv::Point& intersection) const
{
    int x1 = -MAX_INTERSECTION_DISTANCE_OUTSIDE_IMAGE;
    int y1 = -MAX_INTERSECTION_DISTANCE_OUTSIDE_IMAGE;
    int x2 = image_width + MAX_INTERSECTION_DISTANCE_OUTSIDE_IMAGE;
    int y2 = image_height + MAX_INTERSECTION_DISTANCE_OUTSIDE_IMAGE;
    
    return ((intersection.x >= x1 && intersection.x <= x2) &&
            (intersection.y >= y1 && intersection.y <= y2));
}

bool Rectangle::are_lines_within_corners(const Line** lines) const {
    int min_x = std::min(corners[TOP_LEFT].x, corners[BOTTOM_LEFT].x);
    int max_x = std::max(corners[TOP_RIGHT].x, corners[BOTTOM_RIGHT].x);
    int min_y = std::min(corners[TOP_LEFT].y, corners[TOP_RIGHT].y);
    int max_y = std::max(corners[BOTTOM_LEFT].y, corners[BOTTOM_RIGHT].y);
    
    for (int i = 0;i < NUM_SIDES;i++) {
        if (std::max(lines[i]->m_pt1.x, lines[i]->m_pt2.x) < min_x)
            return false;
        
        if (std::min(lines[i]->m_pt1.x, lines[i]->m_pt2.x) > max_x)
            return false;
        
        if (std::max(lines[i]->m_pt1.y, lines[i]->m_pt2.y) < min_y)
            return false;
        
        if (std::min(lines[i]->m_pt1.y, lines[i]->m_pt2.y) > max_y)
            return false;
    }
    
    return true;
}

void Rectangle::set_angles(const Line** lines) {
    angles.clear();
    for (size_t side = 0;side < NUM_SIDES;side++) {
        angles.push_back(lines[side]->m_angle);
    }
}

float Rectangle::distance_points(const cv::Point& pt1, const cv::Point& pt2) const {
    return sqrt(std::pow(pt1.x - pt2.x, 2) + std::pow(pt1.y - pt2.y, 2) );
}

void Rectangle::rescale(const cv::Mat& image) {
    float x_scale = float(image.cols) / float(image_width);
    float y_scale = float(image.rows) / float(image_height);
    
    for (size_t i = 0;i < NUM_SIDES;i++) {
        corners[i].x *= x_scale;
        corners[i].y *= y_scale;
    }
    
    image_width = image.cols;
    image_height = image.rows;
}

const std::vector<cv::Point>& Rectangle::get_points() const {
    return corners;
}

void Rectangle::draw(cv::Mat& image, const cv::Scalar& color) const {
    for (size_t side = 0;side < NUM_SIDES;side++) {
        int next_corner = (side + 1) % NUM_SIDES;
        cv::line(image, corners[side], corners[next_corner], color, 3);
    }
}

float Rectangle::get_top_length() const {
    return distance_points(corners[TOP_RIGHT], corners[TOP_LEFT]);
}

float Rectangle::get_bottom_length() const  {
    return distance_points(corners[BOTTOM_LEFT], corners[BOTTOM_RIGHT]);
}

float Rectangle::get_left_length() const  {
    return distance_points(corners[BOTTOM_LEFT], corners[TOP_LEFT]);
}

float Rectangle::get_right_length() const  {
    return distance_points(corners[BOTTOM_RIGHT], corners[TOP_RIGHT]);
}

float Rectangle::get_top_right_to_bottom_left_length() const {
    return distance_points(corners[TOP_RIGHT], corners[BOTTOM_LEFT]);
}

float Rectangle::get_top_left_to_bottom_right_length() const {
    return distance_points(corners[TOP_LEFT], corners[BOTTOM_RIGHT]);
}

float Rectangle::get_angle_between_top_bottom_sides() const {
    return angles[BOTTOM_SIDE] - angles[TOP_SIDE];
}

float Rectangle::get_angle_between_left_right_sides() const {
    return angles[LEFT_SIDE] - angles[RIGHT_SIDE];
}
