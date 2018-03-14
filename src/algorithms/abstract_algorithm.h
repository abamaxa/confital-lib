//
//  abstract_algorithm.h
//  confital-lib-mactest
//
//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#ifndef abstract_algorithm_h
#define abstract_algorithm_h

#include "pipeline_job.h"

class AbstractAlgorithm {
public:
    virtual bool apply(PipelineJob& job);
};

#endif /* abstract_algorithm_h */
