#include "rectangle.h"

int find_side(cv::Point* points, cv::Point corner1, cv::Point corner2);

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

bool Rectangle::set(LineVector& lines, const cv::Mat& image) {
    image_width = image.cols;
    image_height = image.rows;
    
    if (!calculate_intersections(lines))
        return false;
    
    if (!are_lines_within_corners(lines))
        return false;
    
    if (!intersections_are_close())
        return false;
    
    set_angles(lines);
    return true;
}

bool Rectangle::calculate_intersections(LineVector& lines) {
    cv::Point points[4];
    
    lines[0].find_intersection(lines[1], points[0]);
    lines[0].find_intersection(lines[3], points[1]);
    lines[2].find_intersection(lines[1], points[2]);
    lines[2].find_intersection(lines[3], points[3]);
    
    set_corners(points);
    return order_lines(lines, points);
}

void Rectangle::set_corners(cv::Point* original_points) {
    // Sort in order of top-left, top-right, bottom-right, bottom-left
    assert(corners.size() == NUM_SIDES);
    std::copy(original_points, original_points + NUM_SIDES, corners.begin());
    
    std::sort(corners.begin(),corners.end(),[](cv::Point& a,cv::Point& b) {
        return a.y < b.y;
    });
    
    // top most
    std::sort(corners.begin(),corners.begin()+2,[](cv::Point& a,cv::Point& b) {
        return a.x < b.x;
    });
    
    // bottom most
    std::sort(corners.begin() + 2, corners.end(),[](cv::Point& a,cv::Point& b) {
        return a.x > b.x;
    });
}

bool Rectangle::order_lines(LineVector& lines, cv::Point* points) {
    LineVector ordered;

    for (size_t side = 0;side < NUM_SIDES;side++) {
        const cv::Point& corner1 = corners[side];
        const cv::Point& corner2 = corners[(side + 1) % NUM_SIDES];

        int position = find_side(points, corner1, corner2);
        if (position == UNMATCHED)
            return false;
        
        ordered.push_back(lines[position]);
    }
    
    std::copy(ordered.begin(), ordered.end(), lines.begin());
    return true;
}

int find_side(cv::Point* points, cv::Point corner1, cv::Point corner2) {
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
    for (const auto& corner : corners) {
        if (!intersection_in_or_near_image(corner))
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

bool Rectangle::are_lines_within_corners(const LineVector& lines) const {
    for (const auto& line : lines) {
        if (!line.inside_rectangle(corners))
            return false;
    }
    return true;
}

void Rectangle::set_angles(const LineVector& lines) {
    angles.clear();
    for (const auto& line : lines) {
        angles.push_back(line.get_angle());
    }
}

float Rectangle::distance_points(const cv::Point& pt1, const cv::Point& pt2) const {
    return sqrt(std::pow(pt1.x - pt2.x, 2) + std::pow(pt1.y - pt2.y, 2) );
}

void Rectangle::rescale(const cv::Mat& image) {
    float x_scale = float(image.cols) / float(image_width);
    float y_scale = float(image.rows) / float(image_height);
    
    for (auto& corner : corners) {
        corner.x *= x_scale;
        corner.y *= y_scale;
    }
    
    image_width = image.cols;
    image_height = image.rows;
}

const Points& Rectangle::get_points() const {
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
