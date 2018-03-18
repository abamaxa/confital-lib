//
//  rectangle_detector.cpp
//  confital-lib-mactest
//
//  Created by Chris Morgan on 18/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#include "rectangle_detector.h"
#include "rectangle.h"
#include "document.h"

void RectangleDetector::apply(PipelineJob& job) {
    current_job = &job;
    const ApproxParallelLineGroup& groups = job.get_parallel_line_groups();
    for (auto& group : groups) {
        if (group.contains_less_than_2_sides())
            continue;
            
        auto normal_sides = std::find_if(groups.begin(), groups.end(),
            [group](const ApproxParallelLines& test) {
                return group.is_approx_normal(test);
            }
        );
        
        for(;normal_sides != groups.end();++normal_sides) {
            if ((*normal_sides).contains_less_than_2_sides())
                continue;
            
            make_and_add_rectangles(group, *normal_sides);
        }
    }
    current_job = nullptr;
}

void RectangleDetector::make_and_add_rectangles
(
    const ApproxParallelLines& sides,
    const ApproxParallelLines& normals
) const
{
    for(auto itr = sides.begin(); itr != sides.end(); ++itr) {
        auto itr2 = ApproxParallelLines::iterator(itr);
        for(itr2++; itr2 != sides.end(); itr2++) {
            for(auto ntr = normals.begin(); ntr != normals.end(); ++ntr) {
                auto ntr2 = ApproxParallelLines::iterator(ntr);
                for(ntr2++; ntr2 != normals.end(); ntr2++) {
                    LineVector lines{*itr, *ntr, *itr2, *ntr2};
                    add_valid_rectangle(lines);
                }
            }
        }
    }
}

void RectangleDetector::add_valid_rectangle(LineVector& lines) const {
    Rectangle rectangle;
    const cv::Mat& image = current_job->get_initial_image();
    bool possible_document = rectangle.set(lines, image);
    if (!possible_document)
        return;
    
    Document document;
    possible_document = document.assess_document(rectangle, image);
    
    if (possible_document) {
        current_job->add_rectangle(document);
    }
}
