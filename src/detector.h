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
    void edgeDetectCanny(cv::Mat& imgResult);
    void edgeDetectStructuredForest(cv::Mat& imgResult);

    void findHoughLines(cv::Mat& edgesNMS, size_t maxLines);
    void findHoughPLines(cv::Mat& edgesNMS, size_t maxLines);

    void analyseLines();

    int findBestRectangleIndex() const;

    void detectRectangle();

    void debugDrawResults(cv::Mat& image) const;

    cv::Ptr<cv::ximgproc::StructuredEdgeDetection> m_dollar;
    int m_frames;
    int m_width;
    int m_height;
    cv::Mat m_imgSaved;
    Document m_lastRect;
    bool m_documentDetected;
    int m_framesSinceDocDetected;
    
    LineVector lineCandidates;
    LineVector selectedCandidates;
    std::vector<Document> rectangles;
};

#endif /* Detector_h */
