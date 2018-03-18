//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.

#ifndef line_detector_hpp
#define line_detector_hpp

#include "common.h"
#include "line.h"
#include "abstract_algorithm.h"

const char* const LINE_DETECTOR = "Line Detector";

class LineDetector : public AbstractAlgorithm {
public:
    virtual void apply(PipelineJob& job);
    virtual const char* get_name() { return LINE_DETECTOR; }
private :
    void find_hough_lines(PipelineJob& job);
    void find_hough_lines_p(PipelineJob& job);
};


#endif /* line_detector_hpp */
