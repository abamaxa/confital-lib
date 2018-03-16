#import "document.h"

Document::Document() {
    reset();
}

void Document::reset() {
    m_dimensions_ratio = 0.0;
    m_image_full_gray_ratio = 0.0;
    m_image_edge_gray_ratio = 0.0;
    m_area_ratio = 0.0;
    rectangle.reset();
    is_probably_doc = false;
    orientation = UNKNOWN;
    parallel_sides_length_ratio = 0.0;
    bottom_len = 0.0f;
    top_len = 0.0f;
    left_len = 0.0f;
    right_len = 0.0f;
    top_left_to_bot_right_len = 0.0f;
    top_right_to_bot_left_len  = 0.0f;
}

bool Document::assess_document(const Rectangle& rectangle, const cv::Mat& image)
{
    m_image_width = image.cols;
    m_image_height = image.rows;

    this->rectangle = rectangle;
    
    set_lengths();
    calculate_area_ratio();
    if (area_is_too_small())
        return false;
    
    determine_orientation();
    
    calculate_length_parallel_sides_ratio();
    if (!length_parallel_sides_ratio_within_bounds())
        return false;
    
    calculate_internal_angles();
    if (!internal_angles_within_bounds())
        return false;
    
    calculate_dimension_ratio();
    if (!dimensions_within_bounds())
        return false;
    
    calculateHistogramRatios(image);
    if (m_image_full_gray_ratio >= MIN_FULL_GRAY_RATIO &&
        m_image_edge_gray_ratio >= MIN_EDGE_GRAY_RATIO)
    {
        is_probably_doc = true;
    }
    
    return is_probably_doc;
}

void Document::set_lengths() {
    bottom_len = rectangle.get_bottom_length();
    top_len = rectangle.get_top_length();
    left_len = rectangle.get_left_length();
    right_len = rectangle.get_right_length();
    top_left_to_bot_right_len = rectangle.get_top_left_to_bottom_right_length();
    top_right_to_bot_left_len = rectangle.get_top_right_to_bottom_left_length();
}

bool Document::is_valid() const {
    return is_probably_doc;
}

void Document::calculate_area_ratio() {
    float area = top_len * left_len;
    m_area_ratio = (area / (m_image_width * m_image_height));
}

bool Document::area_is_too_small() const {
    return (m_area_ratio < MIN_AREA_RATIO);
}

void Document::determine_orientation() {
    /*
     Find two nearly parallel sides. The check the other two sides approximately
     slope towards each other or are parallel too. ie we are lookings at
     an isoceles trapazoid or proper rectangle
     
     Check top and bottom first, the left and right sides
     */
    orientation = UNKNOWN;
    
    float angle = fabs(rectangle.get_angle_between_top_bottom_sides());
    if (angle < M_PI * 0.05) {
        orientation = VERTICAL;
    }
    
    angle = fabs(rectangle.get_angle_between_left_right_sides());
    if (angle < M_PI * 0.05) {
        orientation = HORIZONTAL;
    }
}

void Document::calculate_length_parallel_sides_ratio() {
    if (orientation == HORIZONTAL) {
        if (left_len < right_len)
            parallel_sides_length_ratio = left_len / right_len;
        else
            parallel_sides_length_ratio = right_len / left_len;
    }
    else if (orientation == VERTICAL) {
        parallel_sides_length_ratio = top_len / bottom_len;
    }
    else {
        parallel_sides_length_ratio = std::min(
            fmin(top_len, bottom_len) / fmax(top_len, bottom_len),
            fmin(right_len, left_len) / fmax(right_len, left_len)
        );
    }
}

bool Document::length_parallel_sides_ratio_within_bounds() const {
    if (orientation == UNKNOWN)
        return (parallel_sides_length_ratio >= 0.9);
    else if (parallel_sides_length_ratio < 0.6 || parallel_sides_length_ratio > 1.1)
        return false;
        
    return true;
}

void Document::calculate_internal_angles() {
    internal_angles.clear();
    internal_angles.push_back(find_angle(right_len, top_len, top_left_to_bot_right_len));
    internal_angles.push_back(find_angle(right_len, bottom_len, top_right_to_bot_left_len));
    internal_angles.push_back(find_angle(left_len, top_len, top_right_to_bot_left_len));
    internal_angles.push_back(find_angle(left_len, bottom_len, top_left_to_bot_right_len));
}

bool Document::internal_angles_within_bounds() const {
    // Now check angles, the angles stored are for the perpendicular to the
    // point (0,0), the gradiants are +/- pi / 2
    const float MAX_ANGLE = M_PI * 0.51;
    int count_outside_limit = 0;
    std::vector<float>::const_iterator itr = internal_angles.begin();
    for (;itr != internal_angles.end();++itr) {
        if (*itr > MAX_ANGLE)
            count_outside_limit++;
    }
    
    return (count_outside_limit <= 2);
}

void Document::calculate_dimension_ratio()
{
    bool adjustForPerspective = (orientation != UNKNOWN);
    
    float v =  fmax(left_len, right_len);
    float h =  fmax(bottom_len, top_len);

    m_dimensions_ratio = fmin(h, v) / fmax(h, v);
    m_dimensions_ratio = std::fabs(m_dimensions_ratio - DOCUMENT_ASPECT_RATIO);
    
    if (!dimensions_within_bounds()) {
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
            
            m_dimensions_ratio = fmin(h, v) / fmax(h, v);
            m_dimensions_ratio = std::fabs(m_dimensions_ratio - DOCUMENT_ASPECT_RATIO);
        }
    }
}

bool Document::dimensions_within_bounds() const {
    return (m_dimensions_ratio < DIMENSION_RATIO_MARGIN);
}

void Document::calculateHistogramRatios(const cv::Mat& image) {
    cv::Mat subimg, mask, hsv_img, histogram;
    int channels[] = {1};
    int histSize[] = {NUM_HISTOGRAM_BINS};
    float sranges[] = { 0, 256 };
    const float* ranges[] = { sranges };

    copy_deskewed_document(image, subimg);
    mask = cv::Mat::zeros(subimg.rows, subimg.cols , CV_8UC1);

    cv::Point2f points[NUM_SIDES];
    points[0] = cv::Point2f(0, 0);
    points[1] = cv::Point2f(subimg.cols, 0);
    points[2] = cv::Point2f(subimg.cols, subimg.rows);
    points[3] = cv::Point2f(0, subimg.rows);

    for (int i = 0;i < NUM_SIDES;++i) {
        int n = (i + 1) % NUM_SIDES;
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

float Document::find_angle(float len_a, float len_b, float len_c) const {
    // c2 = a2 + b2 − 2ab cos(C)
    float cosc = (len_a * len_a + len_b * len_b - len_c * len_c) / (2 * len_a * len_c);
    return acos(cosc);
}

void Document::rescale(const cv::Mat& image) {
    rectangle.rescale(image);

    m_image_width = image.cols;
    m_image_height = image.rows;
}

void Document::copy_deskewed_document(const cv::Mat& image, cv::Mat& output) const
{
    // construct destination points to obtain a "birds eye view",
    // (i.e. top-down view) of the image, again specifying points
    // in the top-left, top-right, bottom-right, and bottom-left
    // order
    const std::vector<cv::Point>& corners = rectangle.get_points();
    
    cv::Mat matPoints;
    cv::Point2f points[NUM_SIDES];
    
    assert(corners.size() == NUM_SIDES);
    
    for (size_t i = 0;i < NUM_SIDES;i++) {
        points[i] = corners.at(i);
        matPoints.push_back(corners.at(i));
    }

    // obtain a consistent order of the points and unpack them
    // individually
    cv::RotatedRect box = cv::minAreaRect(matPoints);
    cv::Point2f destination[NUM_SIDES];

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

const std::vector<cv::Point>& Document::get_points() const {
    return rectangle.get_points();
}

void Document::draw(cv::Mat& image, const cv::Scalar& color) const {
    rectangle.draw(image, color);
}

float Document::getScore() const {
    return (m_image_full_gray_ratio * m_image_edge_gray_ratio * m_image_edge_gray_ratio);
    //return (1. - m_dimensions_ratio);
}

