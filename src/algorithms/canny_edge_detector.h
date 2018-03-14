//
//  canny_edge_detector.hpp
//  confital-lib-mactest
//
//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#ifndef canny_edge_detector_hpp
#define canny_edge_detector_hpp

#include "abstract_algorithm.h"

class CannyEdgeDetector : public AbstractAlgorithm {
public:
    virtual bool apply(PipelineJob& job);
};

#endif /* canny_edge_detector_hpp */
