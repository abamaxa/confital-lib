//
//  trees_edge_detector.hpp
//  confital-lib-mactest
//
//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#ifndef trees_edge_detector_hpp
#define trees_edge_detector_hpp

#include "abstract_algorithm.h"

class TreeEdgeDetector : public AbstractAlgorithm {
public:
    virtual bool apply(PipelineJob& job);
};

#endif /* trees_edge_detector_hpp */
