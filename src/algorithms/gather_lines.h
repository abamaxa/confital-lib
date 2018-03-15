//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#ifndef gather_lines_hpp
#define gather_lines_hpp

#include "common.h"
#include "line.h"
#include "abstract_algorithm.h"

class RectangleDetector : public AbstractAlgorithm {
public:
    virtual void apply(PipelineJob& job);
    
private :
    void group_parallel_and_normal_lines(PipelineJob& job);
    void generate_rectangles(PipelineJob& job);
    bool intersection_in_or_near_image(const PipelineJob& job, const cv::Point& intersection) const;
    
    LineVector selectedCandidates;
};



#endif /* gather_lines_hpp */
