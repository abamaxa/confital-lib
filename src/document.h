#ifndef Document_h_included
#define Document_h_included

#include "common.h"
#include "config.h"
#include "line.h"
#include "rectangle.h"

enum ORIENTATION {
    UNKNOWN,
    VERTICAL,
    HORIZONTAL
};

class Document {
public:
    Document();
    bool assess_document(const Rectangle& rectangle, const cv::Mat& image);

    void reset();
    void rescale(const cv::Mat& image);

    void draw(cv::Mat& image, const cv::Scalar& color) const;
    const std::vector<cv::Point>& get_points() const;
    void copy_deskewed_document(const cv::Mat& image, cv::Mat& output) const;

    float getScore() const;
    bool is_valid() const;

private:
    void set_lengths();
    
    void calculate_area_ratio();
    bool area_is_too_small() const;
    void determine_orientation();
    
    void calculate_dimension_ratio() ;
    bool dimensions_within_bounds() const;
    
    void calculate_length_parallel_sides_ratio();
    bool length_parallel_sides_ratio_within_bounds() const;
    
    void calculate_internal_angles();
    bool internal_angles_within_bounds() const;
    
    void calculateHistogramRatios(const cv::Mat& image);

    float calculate_ratio(cv::Mat& sat, int num_pixels) const;
    float find_angle(float len_a, float len_b, float len_c) const;
    
    Rectangle rectangle;
    
    int m_image_width;
    int m_image_height;
    float m_dimensions_ratio;
    float m_image_full_gray_ratio;
    float m_image_edge_gray_ratio;
    float m_area_ratio;
    bool is_probably_doc;
    ORIENTATION orientation;
    float parallel_sides_length_ratio;
    std::vector<float> internal_angles;
    float bottom_len;
    float top_len;
    float left_len;
    float right_len;
    float top_left_to_bot_right_len;
    float top_right_to_bot_left_len;
};


#endif
