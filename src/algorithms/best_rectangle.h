//
//  best_rectangle.hpp
//  confital-lib-mactest
//
//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#ifndef best_rectangle_hpp
#define best_rectangle_hpp

#include "common.h"
#include "abstract_algorithm.h"

const char* const BEST_RECTANGLE = "Best Rectangle";

class BestRectangleSelector : public AbstractAlgorithm {
public:
    virtual void apply(PipelineJob& job);
    virtual const char* get_name() { return BEST_RECTANGLE; }
};

#endif /* best_rectangle_hpp */
