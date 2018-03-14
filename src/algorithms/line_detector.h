//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.

#ifndef line_detector_hpp
#define line_detector_hpp

#include "common.h"
#include "line.h"
#include "abstract_algorithm.h"

class LineDetector : public AbstractAlgorithm {
public:
    virtual void apply(PipelineJob& job);
    
private :
    void find_hough_lines(PipelineJob& job);
    void find_hough_lines_p(PipelineJob& job);
};


#endif /* line_detector_hpp */
