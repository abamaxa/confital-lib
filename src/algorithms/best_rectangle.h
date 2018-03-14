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

class BestRectangleSelector : public AbstractAlgorithm {
public:
    virtual void apply(PipelineJob& job);
};

#endif /* best_rectangle_hpp */
