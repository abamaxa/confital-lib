//
//  MTConfig.h
//
//  Created by Chris Morgan on 31/12/17.
//  Copyright © 2017 Chris Morgan. All rights reserved.
//

#ifndef MTConfig_h
#define MTConfig_h

const float MIN_FULL_GRAY_RATIO = 0.80;
const float MIN_EDGE_GRAY_RATIO = 0.85;
const float MIN_AREA_RATIO = 0.15;
const float DIMENSION_RATIO_MARGIN = 0.05;
const float DOCUMENT_ASPECT_RATIO = 0.707070;

const int MAX_FAILED_DETECTIONS = 2;

// The number of lines returned by the line detection algorithm, these
// are the strongest lines found in the image
const size_t MAX_NUM_HOUGH_LINES_TO_DETECT = 16;
const size_t MAX_NUM_HOUGH_P_LINES_TO_DETECT = 32;
const int MAX_INTERSECTION_DISTANCE_OUTSIDE_IMAGE = 16;

const int NUM_HISTOGRAM_BINS = 4;


#endif /* MTConfig_h */
