//  Created by Chris Morgan on 16/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#ifndef object_detector_hpp
#define object_detector_hpp

#include "common.h"
#include <opencv2/dnn.hpp>
#include <opencv2/core_detect.hpp>

#include "line.h"
#include "abstract_algorithm.h"

class ObjectDetector : public AbstractAlgorithm {
public:
    virtual void apply(PipelineJob& job);
    
private :
    static cv::dnn::Net& load_and_return_net();
    static cv::dnn::Net net;
};


#endif /* object_detector_hpp */
