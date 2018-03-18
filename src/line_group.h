//
//  line_group.hpp
//  confital-lib-mactest
//
//  Created by Chris Morgan on 18/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#ifndef line_group_hpp
#define line_group_hpp

#include <unordered_set>
#include "line.h"

class ApproxParallelLines : public std::unordered_set<Line> {
public:
    ApproxParallelLines(const Line& line);
    
    void add_all_approx_parallel_lines(const LineVector& lines);
    float get_angle() const;
    bool line_in_group(const Line& line) const;
    bool is_approx_normal(const ApproxParallelLines& group) const;
   
    static bool is_approximately_parallel(float angle1, float angle2);
    bool contains_less_than_2_sides() const;
    
private:
    bool is_approximately_parallel(const Line& line);

    float angle = 0.0f;
};

typedef std::vector<ApproxParallelLines> ApproxParallelLineGroup;

#endif /* line_group_hpp */
