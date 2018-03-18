//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#ifndef Common_h
#define Common_h

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#import <opencv2/opencv.hpp>
#import <opencv2/ximgproc.hpp>
#import <opencv2/highgui.hpp>
#pragma clang diagnostic pop

#import <cmath>
#import <algorithm>

const int TOP_LEFT = 0;
const int TOP_RIGHT = 1;
const int BOTTOM_RIGHT = 2;
const int BOTTOM_LEFT = 3;
const int UNMATCHED = -1;

typedef std::vector<cv::Point> Points;

#endif
