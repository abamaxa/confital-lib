//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#ifndef edge_detector_hpp
#define edge_detector_hpp

#include "abstract_algorithm.h"

typedef cv::Ptr<cv::ximgproc::StructuredEdgeDetection> StructuredEdgeDetection;

class TreesEdgeDetector : public AbstractAlgorithm {
public:
    TreesEdgeDetector(StructuredEdgeDetection& edge_detector);
    virtual void apply(PipelineJob& job);
    
private:
    StructuredEdgeDetection edge_detector;
};

class CannyEdgeDetector : public AbstractAlgorithm {
public:
    virtual void apply(PipelineJob& job);
};

#endif /* edge_detector_hpp */
