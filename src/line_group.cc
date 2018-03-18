//
//  line_group.cpp
//  confital-lib-mactest
//
//  Created by Chris Morgan on 18/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#include "line_group.h"

ApproxParallelLines::ApproxParallelLines(const Line& line)
: angle(line.get_angle())
{
    insert(line);
}

void ApproxParallelLines::add_all_approx_parallel_lines(const LineVector& lines_to_add)
{
    for (auto& line : lines_to_add) {
        if (is_approximately_parallel(line))
            insert(line);
    }
}

bool ApproxParallelLines::line_in_group(const Line& line) const {
    return (find(line) != end());
}

float ApproxParallelLines::get_angle() const {
    return angle;
}

bool ApproxParallelLines::is_approx_normal(const ApproxParallelLines& group) const
{
    float difference = fabs(angle - group.get_angle());
    return (difference < M_PI * 0.6 && difference > M_PI * 0.4);
}

bool ApproxParallelLines::is_approximately_parallel(const Line& line) {
    return is_approximately_parallel(angle, line.get_angle());
}

bool ApproxParallelLines::is_approximately_parallel(float angle1, float angle2)
{
    float difference = fabs(angle1 - angle2);
    return (difference < M_PI * 0.1 || difference > M_PI * 0.9);
}

bool ApproxParallelLines::contains_less_than_2_sides() const {
    return (size() < 2);
}
