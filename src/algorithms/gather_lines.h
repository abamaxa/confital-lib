//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#ifndef gather_lines_hpp
#define gather_lines_hpp

#include "common.h"
#include "line.h"
#include "line_group.h"
#include "abstract_algorithm.h"

const char* const LINE_GATHER = "Line Gather";

class LineGatherer : public AbstractAlgorithm {
public:
    virtual void apply(PipelineJob& job);
    virtual const char* get_name() { return LINE_GATHER; }
};



#endif /* gather_lines_hpp */
