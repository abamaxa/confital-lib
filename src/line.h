#ifndef Line_h_included
#define Line_h_included

#include "common.h"
#include "config.h"

class Line;
typedef std::vector<Line> LineVector;

class Line {
public:
    Line(float rho, float theta);
    Line(const cv::Vec4i&);
    Line(const cv::Point& _pt1, const cv::Point& _pt2, double _angle);

    float get_angle() const;
    bool inside_rectangle(const Points& bounds) const;
    
    std::size_t get_hash() const noexcept;
    bool operator==(const Line& t) const;

    bool find_intersection(const Line& l2, cv::Point& result) const;
    bool is_similar_line(const Line& test, const cv::Rect& test_bounds) const;

    void draw(cv::Mat& image, const cv::Scalar& color) const;
    
private:
    cv::Point point1;
    cv::Point point2;
    double angle;
};

namespace std {
    template <> struct hash<Line>    {
        size_t operator()(const Line & line) const {
            return line.get_hash();
        }
    };
}

#endif
