#include <cmath>
#include "line.h"
#include "gather_lines.h"

Line::Line(const cv::Point& _pt1, const cv::Point& _pt2, double _angle)
    : point1(_pt1), point2(_pt2), angle(_angle)
{
}

bool Line::is_similar_line(const Line& line, const cv::Rect& bounds) const
{
    cv::Point intersection;
    bool did_intersect = find_intersection(line, intersection);
    if (did_intersect) {
        bool is_similar_grad = ApproxParallelLines::is_approximately_parallel(
                get_angle(), line.get_angle());
        
        bool intersect_in_image = bounds.contains(intersection);

        if (intersect_in_image && is_similar_grad) {
            //NSLog(@"Skipping intersections at", intersection)
            // reject this line because it is too similar in gradiant
            // and location to a stronger line in the image
            return true;
        }
    }
    return false;
}

Line::Line(float rho, float theta) {
    angle = theta;

    float a = cos(theta);
    float b = sin(theta);
    float x0 = a*rho;
    float y0 = b*rho;
    int x1 = int(x0 + 1000*(-b));
    int y1 = int(y0 + 1000*(a));
    int x2 = int(x0 - 1000*(-b));
    int y2 = int(y0 - 1000*(a));

    point1 = cv::Point(x1,y1);
    point2 = cv::Point(x2,y2);
}

Line::Line(const cv::Vec4i& line)
{
    // !!! divide by zero
    if (line[0] == line[2])  {
        angle = M_PI / 2;
    }
    else if (line[1] == line[3]) {
        angle = 0.0;
    }
    else {
        float x1 = float(line[0]);
        float y1 = float(line[1]);
        float x2 = float(line[2]);
        float y2 = float(line[3]);
        
        float m = (y2 - y1) / (x2 - x1);
        angle = atan(m);
    }

    point1 = cv::Point(line[0], line[1]);
    point2 = cv::Point(line[2], line[3]);
}

bool Line::find_intersection(const Line& l2, cv::Point& result) const
{
    // https://stackoverflow.com/questions/14042397/i-am-struck-at-finding-the-point-of-intersection-of-most-lines-in-an-image
    #define PointMinusPoint(P,Q,R)      {(P).x = (Q).x - (R).x; (P).y = (Q).y - (R).y;}
    #define PointCross(P,Q)             (((P).x*(Q).y)-((P).y*(Q).x))
    #define SIGN(X)             (((X)>=0)? 1:-1 )
    #define ABS(a)              ((a) >= 0 ? (a) : (-(a)))
    #define ROUND(a)            ((SIGN(a)) * ( ( int )( ABS(a) + 0.5 ) ) )
    
    const cv::Point&  p  = point1;
    cv::Point   dp;
    const cv::Point&  q  = l2.point1;
    cv::Point   dq;
    cv::Point   qmp;            // q-p
    int         dpdq_cross;     // 2 cross products
    int         qpdq_cross;     // dp with dq,  q-p with dq
    float       a;

    PointMinusPoint(dp, point2, point1);
    PointMinusPoint(dq,l2.point2,l2.point1);
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

float Line::get_angle() const {
    return angle;
}

bool Line::inside_rectangle(const Points& corners) const {
    int min_x = std::min(corners[TOP_LEFT].x, corners[BOTTOM_LEFT].x);
    int max_x = std::max(corners[TOP_RIGHT].x, corners[BOTTOM_RIGHT].x);
    int min_y = std::min(corners[TOP_LEFT].y, corners[TOP_RIGHT].y);
    int max_y = std::max(corners[BOTTOM_LEFT].y, corners[BOTTOM_RIGHT].y);
    
    if (std::max(point1.x, point2.x) < min_x)
        return false;
    
    if (std::min(point1.x, point2.x) > max_x)
        return false;
    
    if (std::max(point1.y, point2.y) < min_y)
        return false;
    
    if (std::min(point1.y, point2.y) > max_y)
        return false;

    return true;
}

void Line::draw(cv::Mat& image, const cv::Scalar& color) const {
    cv::line(image, point1, point2, color);
}

std::size_t Line::get_hash() const noexcept {
    std::size_t h1 = std::hash<int>{}(point1.x);
    std::size_t h2 = std::hash<int>{}(point1.y);
    std::size_t h3 = std::hash<int>{}(point2.x);
    std::size_t h4 = std::hash<int>{}(point2.y);
    std::size_t h5 = std::hash<float>{}(angle);
    return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4);
}

bool Line::operator==(const Line& t) const {
    return point1 == t.point1 && point2 == t.point2 && angle == t.angle;
};
