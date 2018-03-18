//  Created by Chris Morgan on 18/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#ifndef rectangle_detector_hpp
#define rectangle_detector_hpp

#include "common.h"
#include "line.h"
#include "abstract_algorithm.h"

const char* const RECTANGLE_DETECTOR = "Rectangle Detector";

class RectangleDetector : public AbstractAlgorithm {
public:
    virtual void apply(PipelineJob& job);
    virtual const char* get_name() { return RECTANGLE_DETECTOR; }
    
private :
    void make_and_add_rectangles(const ApproxParallelLines& sides,
                         const ApproxParallelLines& normals) const;
    
    void add_valid_rectangle(LineVector& lines) const;
    PipelineJob* current_job;
};

#endif /* rectangle_detector_hpp */
