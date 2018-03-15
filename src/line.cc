
#ifdef __cplusplus
#import <opencv2/opencv.hpp>
#import <cmath>
#import "document.h"

#endif

// https://stackoverflow.com/questions/14042397/i-am-struck-at-finding-the-point-of-intersection-of-most-lines-in-an-image
#define PointMinusPoint(P,Q,R)      {(P).x = (Q).x - (R).x; (P).y = (Q).y - (R).y;}
#define PointCross(P,Q)             (((P).x*(Q).y)-((P).y*(Q).x))
#define SIGN(X)             (((X)>=0)? 1:-1 )
#define ABS(a)              ((a) >= 0 ? (a) : (-(a)))
#define ROUND(a)            ((SIGN(a)) * ( ( int )( ABS(a) + 0.5 ) ) )

SideRecord::SideRecord(const Line& _lineRecord, float _angle)
: lineRecord(_lineRecord), angle(_angle)
{}

SideRecord::SideRecord(const SideRecord& copy)
: lineRecord(copy.lineRecord), angle(copy.angle)
{}

SideRecord& SideRecord::operator=(const SideRecord& copy) {
    lineRecord.m_pt1 = copy.lineRecord.m_pt1;
    lineRecord.m_pt2 = copy.lineRecord.m_pt2;
    lineRecord.m_angle = copy.lineRecord.m_angle;
    
    angle = copy.angle;
    return *this;
}

Line::Line(const cv::Point& _pt1, const cv::Point& _pt2, double _angle)
    : m_pt1(_pt1), m_pt2(_pt2), m_angle(_angle)
{

}

bool Line::is_similar_line(const LineVector& lineCandidates, int image_width, int image_height) const
{
    bool similar = false;
    for (LineVector::const_iterator itr = lineCandidates.begin();
         itr != lineCandidates.end(); ++itr)
    {
        const Line& lineRecord = *(itr);
        cv::Point intersection;

        bool did_intersect = find_intersection(lineRecord, intersection);
        if (did_intersect) {
            float angle = fabs(m_angle - lineRecord.m_angle);
            bool isSimilarGrad = (angle < M_PI * 0.1 || angle > M_PI * 0.9);

            bool intersectInImage = (intersection.x >= 0 && intersection.x <= image_width &&
               intersection.y >= 0 && intersection.y <= image_height);

            if (intersectInImage && isSimilarGrad) {
                //NSLog(@"Skipping intersections at", intersection)
                // reject this line because it is too similar in gradiant
                // and location to a stronger line in the image
                similar = true;
            }
        }
    }
    return similar;
}

Line::Line(float rho, float theta) {
    m_angle = theta;

    float a = cos(theta);
    float b = sin(theta);
    float x0 = a*rho;
    float y0 = b*rho;
    int x1 = int(x0 + 1000*(-b));
    int y1 = int(y0 + 1000*(a));
    int x2 = int(x0 - 1000*(-b));
    int y2 = int(y0 - 1000*(a));

    if (y1 < y2) {
        m_pt1 = cv::Point(x1,y1);
        m_pt2 = cv::Point(x2,y2);
    }
    else {
        m_pt2 = cv::Point(x1,y1);
        m_pt1 = cv::Point(x2,y2);
    }
}

/*
Line::Line(const cv::Vec4i& line)
{
    const int LONG_LINE_START = -1000.0;
    const int LONG_LINE_END = 4000.0;
    float x1 = float(line[0]);
    float y1 = float(line[1]);
    float x2 = float(line[2]);
    float y2 = float(line[3]);
    
    // !!! divide by zero
    if (x1 == x2)  {
        m_pt1 = cv::Point(x1, LONG_LINE_START);
        m_pt2 = cv::Point(x2, LONG_LINE_END);
        m_angle = M_PI / 2;
    }
    else if (y1 == y2) {
        m_pt1 = cv::Point(LONG_LINE_START, std::min(y1, y2));
        m_pt2 = cv::Point(LONG_LINE_END, std::max(y1, y2));
        m_angle = 0.0;
    }
    else {
        float m = (y2 - y1) / (x2 - x1);
        // y = mx + c
        float c = y1 - (m * x1);

        float xct = LONG_LINE_END;
        float xcb = LONG_LINE_START;

        float yct = m * xct + c;
        float ycb = m * xcb + c;
        
        if (yct < ycb) {
            m_pt1 = cv::Point(int(xct), int(yct));
            m_pt2 = cv::Point(int(xcb), int(ycb));
        }
        else {
            m_pt2 = cv::Point(int(xct), int(yct));
            m_pt1 = cv::Point(int(xcb), int(ycb));
        }
        
        m_angle = atan(m);
    }
}*/

Line::Line(const cv::Vec4i& line)
{
    // !!! divide by zero
    if (line[0] == line[2])  {
        m_angle = M_PI / 2;
    }
    else if (line[1] == line[3]) {
        m_angle = 0.0;
    }
    else {
        float x1 = float(line[0]);
        float y1 = float(line[1]);
        float x2 = float(line[2]);
        float y2 = float(line[3]);
        
        float m = (y2 - y1) / (x2 - x1);
        m_angle = atan(m);
    }
    
    if (line[1] < line[3]) {
        m_pt1 = cv::Point(line[0], line[1]);
        m_pt2 = cv::Point(line[2], line[3]);
    }
    else {
        m_pt2 = cv::Point(line[0], line[1]);
        m_pt1 = cv::Point(line[2], line[3]);
    }
}

bool Line::find_intersection(const Line& l2, cv::Point& result) const
{
    const cv::Point&  p  = m_pt1;
    cv::Point   dp;
    const cv::Point&  q  = l2.m_pt1;
    cv::Point   dq;
    cv::Point   qmp;            // q-p
    int         dpdq_cross;     // 2 cross products
    int         qpdq_cross;     // dp with dq,  q-p with dq
    float       a;

    PointMinusPoint(dp, m_pt2, m_pt1);
    PointMinusPoint(dq,l2.m_pt2,l2.m_pt1);
    PointMinusPoint(qmp,q,p);

    dpdq_cross = PointCross(dp,dq);
    if (!dpdq_cross){
        // Perpendicular Lines
        return false;
    }

    qpdq_cross = PointCross(qmp,dq);
    a = (qpdq_cross*1.0f/dpdq_cross);

    result.x = ROUND(p.x+a*dp.x);
    result.y = ROUND(p.y+a*dp.y);
    return true;
 }

void Line::draw(cv::Mat& image, const cv::Scalar& color) const {
    cv::line(image, m_pt1, m_pt2, color);
}

void Line::debugDraw(cv::Mat& image) const {
    int intensity = 127;
    if (m_parallelSides.size() && m_normalSides.size() > 1) {
        intensity = 255;
    }

    cv::line(image, m_pt1, m_pt2, cv::Scalar(0,intensity,0));

    for (SideRecordVector::const_iterator itr = m_parallelSides.begin();
         itr != m_parallelSides.end();++itr)
    {
        const SideRecord& side = *(itr);
        cv::line(image, side.lineRecord.m_pt1, side.lineRecord.m_pt2, cv::Scalar(intensity, 0, 0));
    }

    for (SideRecordVector::const_iterator itr = m_normalSides.begin();
         itr != m_normalSides.end();++itr)
    {
        const SideRecord& side = *(itr);
        cv::line(image, side.lineRecord.m_pt1, side.lineRecord.m_pt2, cv::Scalar(0, 0, intensity));
    }
}
