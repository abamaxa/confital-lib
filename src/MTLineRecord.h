#ifndef MTLineRecord_h_included
#define MTLineRecord_h_included

#import "MTConfig.h"

class SideRecord;
typedef std::vector<SideRecord> SideRecordVector;

class LineRecord;
typedef std::vector<LineRecord> LineRecordVector;

class LineRecord {
public:
    LineRecord(float rho, float theta);
    LineRecord(int x1, int y1, int x2, int y2, int width, int height);
    LineRecord(const cv::Point& _pt1, const cv::Point& _pt2, double _angle);

    cv::Point m_pt1;
    cv::Point m_pt2;
    double m_angle;
    SideRecordVector m_parallelSides;
    SideRecordVector m_normalSides;

    // Given 2 line segments, find their intersection point
    // rerurns [Px,Py] point in 'res' or FALSE if parallel. Uses vector cross product technique.
    bool findLinesIntersectionPoint(const LineRecord& l2, cv::Point& result) const;
    bool isSimilarLine(const std::vector<LineRecord>& lineCandidates, int image_width, int image_height) const;

    void debugDraw(cv::Mat& image) const;
    void draw(cv::Mat& image, const cv::Scalar& color) const;
};

class SideRecord {
public:
    SideRecord(const LineRecord& _lineRecord, float _angle, cv::Point _intersection);
    SideRecord(const SideRecord& copy);
    SideRecord& operator=(const SideRecord& copy);

    LineRecord lineRecord;

    float angle;
    cv::Point intersection;
};

#endif
