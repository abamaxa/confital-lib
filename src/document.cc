#ifdef __cplusplus
#import <opencv2/opencv.hpp>
#import <opencv2/ximgproc.hpp>
#import <opencv2/highgui.hpp>
#import <cmath>

#import "document.h"
#import "line.h"

#endif

#define BOTTOM_SIDE 0
#define TOP_SIDE    1
#define LEFT_SIDE   2
#define RIGHT_SIDE  3

Document::Document() :
    m_dimensions_ratio(0.0),
    m_image_full_gray_ratio(0.0),
    m_image_edge_gray_ratio(0.0),
    m_area_ratio(0.0),
    m_vertical_doc(false),
    m_horizontal_doc(false),
    m_camera_orientated(false),
    m_image_width(0),
    m_image_height(0),
    m_minArea(0),
    is_probably_doc(false)
{
}

Document::Document(const cv::Mat& image) :
    m_dimensions_ratio(0.0),
    m_image_full_gray_ratio(0.0),
    m_image_edge_gray_ratio(0.0),
    m_area_ratio(0.0),
    m_vertical_doc(false),
    m_horizontal_doc(false),
    m_camera_orientated(false),
    is_probably_doc(false)
{
    m_image_width = image.cols;
    m_image_height = image.rows;
    m_minArea = float(m_image_width) * float(m_image_height) * MIN_AREA_RATIO;
}

Document::Document(const Document& toCopy) :
    m_dimensions_ratio(toCopy.m_dimensions_ratio),
    m_image_full_gray_ratio(toCopy.m_image_full_gray_ratio),
    m_image_edge_gray_ratio(toCopy.m_image_edge_gray_ratio),
    m_area_ratio(toCopy.m_area_ratio),
    m_vertical_doc(toCopy.m_vertical_doc),
    m_horizontal_doc(toCopy.m_horizontal_doc),
    m_camera_orientated(toCopy.m_camera_orientated),
    m_bottom_left(toCopy.m_bottom_left),
    m_bottom_right(toCopy.m_bottom_right),
    m_top_right(toCopy.m_top_right),
    m_top_left(toCopy.m_top_left),
    m_image_width(toCopy.m_image_width),
    m_image_height(toCopy.m_image_height),
    m_minArea(toCopy.m_minArea),
    is_probably_doc(toCopy.is_probably_doc)
{

}

void Document::reset() {
    m_dimensions_ratio = 0.0;
    m_image_full_gray_ratio = 0.0;
    m_image_edge_gray_ratio = 0.0;
    m_area_ratio = 0.0;
    m_vertical_doc = false;
    m_horizontal_doc = false;
    m_camera_orientated = false;
    
    m_bottom_left = m_bottom_right = m_top_right = m_top_left = cv::Point();
    
    is_probably_doc = false;
}

bool Document::assessRectangle(
    const Line& principleSide,
    const Line& normalSide1,
    const Line& oppositeSide,
    const Line& normalSide2,
    const cv::Mat& image
)
{
    const Line* lines[] = {&principleSide, &normalSide1, &oppositeSide, &normalSide2};
    if (!calculate_intersections(lines))
        return false;

    if (!are_lines_within_corners(lines))
        return false;
    
    m_camera_orientated = true;
    
    if (!intersections_are_close())
        return false;
 
    is_probably_doc = calculateScores(lines);
    
    if (is_probably_doc) {
        calculateHistogramRatios(image);
        if (m_image_full_gray_ratio < MIN_FULL_GRAY_RATIO ||
            m_image_edge_gray_ratio < MIN_EDGE_GRAY_RATIO)
        {
            is_probably_doc = false;
        }
    }

    return is_probably_doc;
}

int match_side(const Line** lines, cv::Point* points, cv::Point match, cv::Point match2) {
    int idx = 0;
    int match_i = -1;
    int match_i2 = -1;
    int match_n = -1;
    int match_n2 = -1;
    
    for (int i = 0;i < 3;i += 2) {
        for (int n = 1;n < 4;n += 2) {
            if (points[idx] == match) {
                match_i = i;
                match_n = n;
            }
            if (points[idx] == match2) {
                match_i2 = i;
                match_n2 = n;
            }
            idx += 1;
        }
    }
    
    if (match_i != -1 && match_i2 != -1) {
        if (match_i == match_i2) {
            return match_i;
        }
        if (match_n == match_n2) {
            return match_n;
        }
    }
    
    return -1;
}

bool Document::calculate_intersections(const Line** lines) {
    cv::Point points[4];

    lines[0]->find_intersection(*lines[1], points[0]);
    lines[0]->find_intersection(*lines[3], points[1]);
    lines[2]->find_intersection(*lines[1], points[2]);
    lines[2]->find_intersection(*lines[3], points[3]);
    
    int min_sum = 100000;
    int max_sum = -min_sum;
    int min_diff = 100000;
    int max_diff = -min_sum;
    
    for (int i = 0;i < 4;i++) {
        int sum = points[i].x + points[i].y;
        int diff = points[i].y - points[i].x;
        
        if (sum > max_sum) {
            m_bottom_right = points[i];
            max_sum = sum;
        }
        if (sum < min_sum) {
            m_top_left = points[i];
            min_sum = sum;
        }
        if (diff > max_diff) {
            m_bottom_left = points[i];
            max_diff = diff;
        }
        if (diff < min_diff) {
            m_top_right = points[i];
            min_diff = diff;
        }
    }
    
    const Line* ordered_lines[4];
    try {
        ordered_lines[BOTTOM_SIDE] = lines[match_side(lines, points, m_bottom_left, m_bottom_right)];
        ordered_lines[TOP_SIDE] = lines[match_side(lines, points, m_top_left, m_top_right)];
        ordered_lines[LEFT_SIDE] = lines[match_side(lines, points, m_top_left, m_bottom_left)];
        ordered_lines[RIGHT_SIDE] = lines[match_side(lines, points, m_top_right, m_bottom_right)];
        memcpy(lines, ordered_lines, sizeof(ordered_lines));
        return true;
    } catch (int error) {
        return false;
    }
}

bool Document::intersections_are_close() const {
    return (
        intersection_in_or_near_image(m_bottom_left) &&
        intersection_in_or_near_image(m_bottom_right) &&
        intersection_in_or_near_image(m_top_right) &&
        intersection_in_or_near_image(m_top_left)
    );
}

bool Document::intersection_in_or_near_image(const cv::Point& intersection) const
{
    int x1 = -MAX_INTERSECTION_DISTANCE_OUTSIDE_IMAGE;
    int y1 = -MAX_INTERSECTION_DISTANCE_OUTSIDE_IMAGE;
    int x2 = m_image_width + MAX_INTERSECTION_DISTANCE_OUTSIDE_IMAGE;
    int y2 = m_image_height + MAX_INTERSECTION_DISTANCE_OUTSIDE_IMAGE;
    
    return ((intersection.x >= x1 && intersection.x <= x2) &&
            (intersection.y >= y1 && intersection.y <= y2));
}

bool Document::are_lines_within_corners(const Line** lines) const {
    int min_x = std::min(m_top_left.x, m_bottom_left.x);
    int max_x = std::max(m_top_right.x, m_bottom_right.x);
    int min_y = std::min(m_top_left.y, m_top_right.y);
    int max_y = std::max(m_bottom_left.y, m_bottom_right.y);
    
    for (int i = 0;i < 4;i++) {
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

bool Document::is_valid() const {
    return is_probably_doc;
}

bool Document::calculateScores(const Line** sides) {
    /* Order of sides is bottom, top, left and right.
     Order of corners is bottom, right, top and left.

    Need to check histogram at this stage and save area, ratio and histogram
    results for use later to select best rectangle

    Normalize values (squared)
    */

    float bottom_len = distancePoints(m_bottom_left, m_bottom_right);
    float top_len = distancePoints(m_top_right, m_top_left);

    float left_len = distancePoints(m_bottom_left, m_top_left);
    float right_len = distancePoints(m_bottom_right, m_top_right);

    float area = top_len * left_len;
    if (area < m_minArea) {
        return false;
    }

    float ratio_margin = 0.07;
    bool vertical_doc = false;
    bool horizontal_doc = false;
    float corner_angle1 = 0.0, corner_angle2 = 0.0;
    bool failed = false;
    float angle = 0.0;
    float para_ratio = 0.0;

    m_area_ratio = (area / (m_image_width * m_image_height));

    if (m_camera_orientated) {
        /*
        Find two nearly parallel sides. The check the other two sides approximately
        slope towards each other or are parallel too. ie we are lookings at
        an isoceles trapazoid or proper rectangle

        Check top and bottom first, the left and right sides
        */
        angle = fabs(sides[BOTTOM_SIDE]->m_angle - sides[TOP_SIDE]->m_angle);
        if (angle < M_PI * 0.05) {
            vertical_doc = true;
        }

        angle = fabs(sides[LEFT_SIDE]->m_angle - sides[RIGHT_SIDE]->m_angle);
        if (angle < M_PI * 0.05) {
            horizontal_doc = true;
        }

        // If this works, probably best to order the sides rather than bother
        // determining orientation in image.
        // In essence, sum of both interior angles should be less that pi radian
        // However, both should also be less than pi / 2 radians
        // As a heuristic, we will exclude shapes where the top is longer than the
        // bottom too.
        
        if (horizontal_doc) {
            if (left_len < right_len) {
                para_ratio = left_len / right_len;
                // bottom_left, bottom_right, top_left, top_right
                corner_angle1 = calcAngle(right_len, top_len,
                        distancePoints(m_bottom_right, m_top_right));
                corner_angle2 = calcAngle(right_len, bottom_len,
                        distancePoints(m_bottom_left, m_top_right));
            }
            else {
                para_ratio = right_len / left_len;
                corner_angle1 = calcAngle(left_len, top_len,
                        distancePoints(m_bottom_left, m_top_right));
                corner_angle2 = calcAngle(left_len, bottom_len,
                        distancePoints(m_bottom_right, m_top_left));
            }
        }
        else if (vertical_doc) {
            para_ratio = top_len / bottom_len;
            corner_angle1 = calcAngle(left_len, bottom_len,
                            distancePoints(m_bottom_right, m_top_left));
            corner_angle2 = calcAngle(right_len, bottom_len,
                            distancePoints(m_bottom_left, m_top_right));
        }
        else {
            para_ratio = fmin(top_len, bottom_len) / fmax(top_len, bottom_len);
            if (para_ratio < 0.9) {
                failed = true;
            }

            para_ratio = fmin(right_len, left_len) / fmax(right_len, left_len);
            if (para_ratio < 0.9) {
                failed = true;
            }
        }
    }

    if (vertical_doc || horizontal_doc) {
        // Why not calculate all internal angles and add them to the array
        if (para_ratio < 0.6 || para_ratio > 1.1) {
            failed = true;
        }

        // Now check angles, the angles stored are for the perpendicular to the
        // point (0,0), the gradiants are +/- pi / 2
        if (corner_angle1 > M_PI * 0.51) {
                //NSLog(angle, (angle - M_PI / 2), angle_from_parallel)
            failed = true;
        }
        else if (corner_angle2 > M_PI * 0.51) {
            //NSLog(angle, (angle - M_PI / 2), angle_from_parallel)
            failed = true;
        }

        ratio_margin = 0.2;
    }

    if (failed) {
        return false;
    }

    bool ratios_ok = checkDimensionRatio(bottom_len, top_len,
                    left_len, right_len, (vertical_doc || horizontal_doc),
                    m_dimensions_ratio);

    if (!ratios_ok) {
        return false;
    }

    return true;
}


bool Document::checkDimensionRatio
(
    float bottom_len,
    float top_len,
    float left_len,
    float right_len,
    bool adjustForPerspective,
    float& result
) const
{
    const float margin = DIMENSION_RATIO_MARGIN;
    const float MAX_RATIO = 0.707070 + margin;
    const float MIN_RATIO = 0.707070 - margin;

    bool shape_ok = true;
    float v =  fmax(left_len, right_len);
    float h =  fmax(bottom_len, top_len);

    result = fmin(h, v) / fmax(h, v);
    
    if (result > MAX_RATIO || result < MIN_RATIO) {
        if (adjustForPerspective) {
            // perspective correction for quadrilateral
            float vratio = (v - fmin(left_len, right_len)) / v;
            float hratio = (h - fmin(bottom_len, top_len)) / h;
            if (vratio < hratio) {
                // Use hratio, boost left/right ie v
                v = v + (v * hratio);
                
            } else {
                h = h + (h * hratio);
            }
            
            result = fmin(h, v) / fmax(h, v);
        }
        
        if (result > MAX_RATIO || result < MIN_RATIO) {
            shape_ok = false;
        }
    }

    result = std::fabs(result - 0.707070);
    //NSLog(ratio)
    return shape_ok;
}

void Document::calculateHistogramRatios(const cv::Mat& image) {
    cv::Mat subimg, mask, hsv_img, histogram;
    int channels[] = {1};
    int histSize[] = {4};
    float sranges[] = { 0, 256 };
    const float* ranges[] = { sranges };

    copy_deskewed_document(image, subimg);
    mask = cv::Mat::zeros(subimg.rows, subimg.cols , CV_8UC1);

    cv::Point2f points[4];
    points[0] = cv::Point2f(0, 0);
    points[1] = cv::Point2f(subimg.cols, 0);
    points[2] = cv::Point2f(subimg.cols, subimg.rows);
    points[3] = cv::Point2f(0, subimg.rows);

    for (int i = 0;i < 4;++i) {
        int n = (i + 1) % 4;
        cv::line(mask, points[i], points[n], cv::Scalar(255,255,255),20);
        cv::line(subimg,points[i], points[n], cv::Scalar(255,255,255),3);
    }

    cv::cvtColor(subimg, hsv_img, cv::COLOR_RGB2HSV);
    cv::calcHist(&hsv_img, 1, channels, mask, histogram, 1, histSize, ranges);

    cv::Scalar totals = cv::sum(histogram);

    m_image_edge_gray_ratio = calculate_ratio(histogram, totals[0]);

    cv::calcHist(&hsv_img, 1, channels, cv::Mat(), histogram, 1, histSize, ranges);
    m_image_full_gray_ratio = calculate_ratio(histogram, hsv_img.cols * hsv_img.rows);
}

float Document::calculate_ratio(cv::Mat& sat, int num_pixels) const {
    return (float(sat.at<float>(0)) / float(num_pixels));
}

float Document::calculate_ratio2(cv::Mat& sat, int num_pixels) const {
    return ((float(sat.at<float>(0)) + float(sat.at<float>(1))) / float(num_pixels));
}

float Document::distancePoints(const cv::Point& pt1, const cv::Point& pt2) const {
    return sqrt(pow(pt1.x - pt2.x, 2) + pow(pt1.y - pt2.y, 2) );
}

float Document::calcAngle(float len_a, float len_b, float len_c) const {
    // c2 = a2 + b2 − 2ab cos(C)
    float cosc = (len_a * len_a + len_b * len_b - len_c * len_c) / (2 * len_a * len_c);
    return acos(cosc);
}

void Document::rescale(const cv::Mat& image) {
    float x_scale = float(image.cols) / float(m_image_width);
    float y_scale = float(image.rows) / float(m_image_height);

    m_bottom_left.x *= x_scale;
    m_bottom_left.y *= y_scale;
    m_bottom_right.x *= x_scale;
    m_bottom_right.y *= y_scale;
    m_top_right.x *= x_scale;
    m_top_right.y *= y_scale;
    m_top_left.x *= x_scale;
    m_top_left.y *= y_scale;

    m_image_width = image.cols;
    m_image_height = image.rows;
    m_minArea = float(m_image_width) * float(m_image_height) * MIN_AREA_RATIO;
}

void Document::copy_deskewed_document(const cv::Mat& image, cv::Mat& output) const
{
    // construct destination points to obtain a "birds eye view",
    // (i.e. top-down view) of the image, again specifying points
    // in the top-left, top-right, bottom-right, and bottom-left
    // order
    cv::Point2f points[4];
    points[0] = m_top_left;
    points[1] = m_top_right;
    points[2] = m_bottom_right;
    points[3] = m_bottom_left;

    cv::Mat matPoints;
    matPoints.push_back(m_top_left);
    matPoints.push_back(m_top_right);
    matPoints.push_back(m_bottom_right);
    matPoints.push_back(m_bottom_left);

    // obtain a consistent order of the points and unpack them
    // individually
    cv::RotatedRect box = cv::minAreaRect(matPoints);

    cv::Point2f destination[4];

    destination[0] = cv::Point2f(0, 0);
    destination[1] = cv::Point2f(box.boundingRect().width-1, 0);
    destination[2] = cv::Point2f(box.boundingRect().width-1, box.boundingRect().height-1);
    destination[3] = cv::Point2f(0, box.boundingRect().height-1);

    // compute the perspective transform matrix and then apply it
    cv::Mat M = cv::getPerspectiveTransform(points, destination);

    cv::Size size(box.boundingRect().width, box.boundingRect().height);
    //cv::Mat warped(size, image.type());

    cv::warpPerspective(image, output, M,
            cv::Size(box.boundingRect().width, box.boundingRect().height));
}

void Document::getPoints(std::vector<cv::Point>& matPoints) const {
    //cv::Mat points;
    matPoints.clear();

    matPoints.push_back(m_top_left);
    matPoints.push_back(m_top_right);
    matPoints.push_back(m_bottom_right);
    matPoints.push_back(m_bottom_left);
}

void Document::draw(cv::Mat& image, const cv::Scalar& color) const {
    cv::line(image, m_top_left, m_top_right, color, 3);
    cv::line(image, m_top_right, m_bottom_right, color, 3);
    cv::line(image, m_bottom_left, m_bottom_right, color, 3);
    cv::line(image, m_bottom_left, m_top_left, color, 3);
}

float Document::getScore() const {
    return (m_image_full_gray_ratio * m_image_edge_gray_ratio * m_image_edge_gray_ratio);
    //return (1. - m_dimensions_ratio);
}

