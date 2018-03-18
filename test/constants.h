//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.

#ifndef constants_h
#define constants_h

const int MAX_DIFFERENCE_BETWEEN_POINTS = 20;
const char* const RESULTS_FILE_NAME = "../../../../../../data/test-results-canny.csv";

const char* const IMAGE_PATHS[] = {
    "messed.png",
    "skewed-nolines.png",
    "skewed-nolines2.png",
    "skewed-failed-to-detect-edge.png",
    "skewed1.png",
    "table-detect-half-page.png",
    "table-failed-to-detect.png",
    "table-failed-to-detect2.png",
    "table-no-detect3.png"
};

const size_t NUM_IMAGES = sizeof(IMAGE_PATHS) / sizeof(const char*);
const char* const IMAGE_DIR = "../../../../../../data/images";
const char* const SAVED_DIR = "../../../../../../data/images_saved";
const char* const MODEL_FILE = "../../../../../../src/model.yml.gz";

#endif /* constants_h */
