//
//  MTDetector.h
//
//  Created by Chris Morgan on 22/12/17.
//  Copyright © 2017 Chris Morgan. All rights reserved.
//

#ifndef MTDetector_h
#define MTDetector_h

#import <opencv2/opencv.hpp>
#import <opencv2/ximgproc.hpp>
#import <opencv2/highgui.hpp>

#import "MTConfig.h"
#import "MTLineRecord.h"
#import "MTDocumentRectangle.h"

enum EDGE_DETECTORS {
    MT_EDGE_DETECT_CANNY,
    MT_EDGE_DETECT_TREES
};

class MTDetector {
public:
    MTDetector();

    bool initialize(cv::String pathToModel);
    void processImage(cv::Mat& imgOriginal, EDGE_DETECTORS edgeMethod);

    bool getDocumentImage(cv::Mat& image);
    bool getDocumentImage(cv::Mat& srcImage, cv::Mat& destImage);
    
    void getDocumentPoints(std::vector<cv::Point>& points) const;
    void reset();
    
private:
    void edgeDetectCanny(cv::Mat& imgResult);
    void edgeDetectStructuredForest(cv::Mat& imgResult);

    void findHoughLines(cv::Mat& edgesNMS, size_t maxLines, std::vector<DocumentRectangle>& rectangles);
    void findHoughPLines(cv::Mat& edgesNMS, size_t maxLines, std::vector<DocumentRectangle>& rectangles);

    void analyseLines(LineRecordVector& lineCandidates, std::vector<DocumentRectangle>& rectangles);

    int findBestRectangleIndex(std::vector<DocumentRectangle>& rectangles) const;

    void detectRectangle
    (
        const LineRecordVector& selectedCandidates,
        std::vector<DocumentRectangle>& rectangles
    ) const;

    void debugDrawResults
    (
     cv::Mat& image,
     const std::vector<DocumentRectangle>& rectangles,
     const LineRecordVector& lineCandidates,
     const LineRecordVector& selectedCandidates
    ) const;

    cv::Ptr<cv::ximgproc::StructuredEdgeDetection> m_dollar;
    int m_frames;
    int m_width;
    int m_height;
    cv::Mat m_imgSaved;
    DocumentRectangle m_lastRect;
    bool m_documentDetected;
    int m_framesSinceDocDetected;
};

#endif /* MTDetector_h */
