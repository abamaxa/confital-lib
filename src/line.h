#ifndef Line_h_included
#define Line_h_included

#include "common.h"
#include "config.h"

class SideRecord;
typedef std::vector<SideRecord> SideRecordVector;

class Line;
typedef std::vector<Line> LineVector;

class Line {
public:
    Line(float rho, float theta);
    Line(const cv::Vec4i&);
    //Line(int x1, int y1, int x2, int y2, int width, int height);
    Line(const cv::Point& _pt1, const cv::Point& _pt2, double _angle);

    cv::Point m_pt1;
    cv::Point m_pt2;
    double m_angle;
    SideRecordVector m_parallelSides;
    SideRecordVector m_normalSides;

    // Given 2 line segments, find their intersection point
    // rerurns [Px,Py] point in 'res' or FALSE if parallel. Uses vector cross product technique.
    bool find_intersection(const Line& l2, cv::Point& result) const;
    bool is_similar_line(const std::vector<Line>& lineCandidates, int image_width, int image_height) const;

    void debugDraw(cv::Mat& image) const;
    void draw(cv::Mat& image, const cv::Scalar& color) const;
};

class SideRecord {
public:
    SideRecord(const Line& _lineRecord, float _angle);
    SideRecord(const SideRecord& copy);
    SideRecord& operator=(const SideRecord& copy);

    Line lineRecord;

    float angle;
};

#endif
