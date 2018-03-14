//  Created by Chris Morgan on 22/12/17.
//  Copyright © 2017 Chris Morgan. All rights reserved.

#ifndef Detector_h
#define Detector_h

#import <opencv2/opencv.hpp>
#import <opencv2/ximgproc.hpp>
#import <opencv2/highgui.hpp>

#import "config.h"
#import "line.h"
#import "document.h"

enum EDGE_DETECTORS {
    MT_EDGE_DETECT_CANNY,
    MT_EDGE_DETECT_TREES
};

class Detector {
public:
    Detector();

    bool initialize(cv::String pathToModel);
    void processImage(cv::Mat& imgOriginal, EDGE_DETECTORS edgeMethod);

    bool getDocumentImage(cv::Mat& image);
    bool getDocumentImage(cv::Mat& srcImage, cv::Mat& destImage);
    
    void getDocumentPoints(std::vector<cv::Point>& points) const;
    void reset();
    
private:
    void calculate_rescale_factor(cv::Mat& imgOriginal);
    void scale_image(cv::Mat& imgOriginal);
    void rescale_image(cv::Mat& imgOriginal);
    void update_detection_state(const Document& rectangle);

    cv::Ptr<cv::ximgproc::StructuredEdgeDetection> m_dollar;
    int m_frames;
    cv::Mat m_imgSaved;
    Document m_lastRect;
    bool m_documentDetected;
    int m_framesSinceDocDetected;
    int rescale_factor;
};

#endif /* Detector_h */
