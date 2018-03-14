//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#ifndef debug_drawings_hpp
#define debug_drawings_hpp

#include "common.h"
#include "abstract_algorithm.h"

class DebugDrawings : public AbstractAlgorithm {
public:
    virtual void apply(PipelineJob& job);
    
private :
    void draw_detected_lines(PipelineJob& job) const;
};

#endif /* debug_drawings_hpp */
