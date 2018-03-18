//
//  detection_results.hpp
//  confital-lib-mactest
//
//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#ifndef detection_results_hpp
#define detection_results_hpp

#include "common.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdlib>

#include "constants.h"

class DetectionResult {
public:
    DetectionResult(const char* _name, const Points& _points);
    DetectionResult(std::ifstream& stream);
    bool compare(const Points& test_points) const;
    void overwrite_points(const Points& new_points);
    void write(std::ostream& out);
    bool is_valid() const;
    
private :
    bool points_identical(const cv::Point& point1, const cv::Point& point2) const;
    void add_point(const std::string& x, const std::string& y);
    
    std::string name;
    Points points;
};

class DetectionResults {
public :
    DetectionResults();
    bool has_result(int index);
    DetectionResult get(int index);
    
private :
    void read_saved_results();
    
    std::vector<DetectionResult> results;
};


#endif /* detection_results_hpp */
