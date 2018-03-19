//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#ifndef edge_detector_hpp
#define edge_detector_hpp

#include "abstract_algorithm.h"

typedef cv::Ptr<cv::ximgproc::StructuredEdgeDetection> StructuredEdgeDetection;

const char* const TREE_DETECTOR = "Tree Edge Detector";
const char* const CANNY_DETECTOR = "Canny Edge Detector";

class CannyEdgeDetector : public AbstractAlgorithm {
public:
    virtual void apply(PipelineJob& job);
    virtual const char* get_name() { return CANNY_DETECTOR; }
};

#if COMPILE_TREE_DETECTION
class TreesEdgeDetector : public AbstractAlgorithm {
public:
    TreesEdgeDetector();
    virtual void apply(PipelineJob& job);
    virtual const char* get_name() { return TREE_DETECTOR; }
    
    virtual bool load(std::string path_to_model);
    
private:
    StructuredEdgeDetection edge_detector;
};
#endif

#endif /* edge_detector_hpp */
