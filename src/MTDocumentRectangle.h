#ifndef MTDocumentRectangle_h_included
#define MTDocumentRectangle_h_included

#import "MTConfig.h"
#import "MTLineRecord.h"

class DocumentRectangle {
public:
    DocumentRectangle();
    DocumentRectangle(const DocumentRectangle& toCopy);
    DocumentRectangle(const cv::Mat& image);

    bool assessRectangle(
        const LineRecord& principleSide,
        const LineRecord& normalSide1,
        const LineRecord& oppositeSide,
        const LineRecord& normalSide2,
        const cv::Mat& image
    );

    void reset();

    void rescale(const cv::Mat& image);

    void draw(cv::Mat& image, const cv::Scalar& color) const;
    void getPoints(std::vector<cv::Point>& points) const;

    void copyDeskewedDocument(const cv::Mat& image, cv::Mat& output) const;

    float getScore() const;

    cv::Point m_bottom_left;
    cv::Point m_bottom_right;
    cv::Point m_top_right;
    cv::Point m_top_left;

    int m_image_width;
    int m_image_height;
    float m_minArea;

    float m_dimensions_ratio;
    float m_image_full_gray_ratio;
    float m_image_edge_gray_ratio;
    float m_area_ratio;
    bool m_vertical_doc;
    bool m_horizontal_doc;
    bool m_camera_orientated;

private:
    bool calculateScores(const LineRecord** sides);

    bool orderSides(const LineRecord** sides) const;

    bool checkDimensionRatio(float bottom_len, float top_len, float left_len,
         float right_len, bool adjustForPerspective, float& result) const;

    void calculateHistogramRatios(const cv::Mat& image);

    float calculate_ratio(cv::Mat& sat, int num_pixels) const;
    float calculate_ratio2(cv::Mat& sat, int num_pixels) const;

    float distancePoints(const cv::Point& pt1, const cv::Point& pt2) const;
    float calcAngle(float len_a, float len_b, float len_c) const;
};


#endif
