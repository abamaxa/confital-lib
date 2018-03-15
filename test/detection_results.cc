//
//  detection_results.cpp
//  confital-lib-mactest
//
//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#include "detection_results.h"


DetectionResult::DetectionResult(const char* _name, const Points& _points) :
    name(_name), points(_points)
{
    
}
    
DetectionResult::DetectionResult(std::ifstream& stream)
{
    std::string x;
    std::string y;
    
    for (int i = 0;i < 4;i++) {
        std::getline(stream, x, ',');
        std::getline(stream, y, ',');
        if (stream.eof())
            return;
        
        add_point(x, y);
    }
    
    std::getline(stream, name);
}
    
bool DetectionResult::compare(const Points& test_points) const {
    //if (test_points.size() != points.size())
    //   return false;
    
    for (int i = 0;i < points.size();i++) {
        if (!points_identical(test_points[i], points[i])) {
            std::cerr << points << "!=" << test_points << "\n";
            return false;
        }
    }
    
    return true;
}

void DetectionResult::overwrite_points(const Points& new_points) {
    points = new_points;
}

void DetectionResult::write(std::ostream& out) {
    for (int i = 0;i < points.size();i++) {
        out << points[i].x << "," << points[i].y << ",";
    }
    
    out << name << "\n";
}

bool DetectionResult::is_valid() const {
    return (points.size() == 4 && name.size() != 0);
}


bool DetectionResult::points_identical(const cv::Point& point1, const cv::Point& point2) const {
    if (abs(point1.x - point2.x) > MAX_DIFFERENCE_BETWEEN_POINTS)
        return false;
    
    if (abs(point1.y - point2.y) > MAX_DIFFERENCE_BETWEEN_POINTS)
        return false;
    
    return true;
}

void DetectionResult::add_point(const std::string& x, const std::string& y) {
    cv::Point point = cv::Point(std::stoi(x), std::stoi(y));
    points.push_back(point);
}
    

DetectionResults::DetectionResults() {
    read_saved_results();
}

bool DetectionResults::has_result(int index) {
    return !(index < 0 || index >= results.size());
}

DetectionResult DetectionResults::get(int index) {
    return results[index];
}

void DetectionResults::read_saved_results() {
    std::ifstream results_file(RESULTS_FILE_NAME);
    if (!results_file) {
        std::cerr << "Could not open results file " << RESULTS_FILE_NAME << "\n";
        return;
    }
    
    while (!results_file.eof()) {
        DetectionResult result = DetectionResult(results_file);
        if (result.is_valid())
            results.push_back(result);
    }
}

