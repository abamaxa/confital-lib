//
//  MTDetector.m
//
//  Created by Chris Morgan on 22/12/17.
//  Copyright © 2017 Chris Morgan. All rights reserved.
//

#ifdef __cplusplus
#import <cmath>
#import <algorithm>

#import "MTDetector.h"

//typedef std::vector<cv::Point> PointVector;

MTDetector::MTDetector() :
    m_frames(0),
    m_width(0),
    m_height(0),
    m_documentDetected(false),
    m_framesSinceDocDetected(1000)
{

}

void MTDetector::reset() {
    m_frames = 0;
    m_width = 0;
    m_height = 0;
    m_documentDetected = false;
    m_framesSinceDocDetected = 1000;

    m_lastRect.reset();
}

bool MTDetector::initialize(cv::String pathToModel)
{
    //NSString* modelFilename = [[NSBundle mainBundle] pathForResource:@"model.yml" ofType:@"gz"];
    //cv::String modelFilenameStr = std::string([modelFilename UTF8String]);
    m_dollar = cv::ximgproc::createStructuredEdgeDetection(pathToModel);
    return true;
}

bool MTDetector::getDocumentImage(cv::Mat& image) {
    if (m_documentDetected || m_framesSinceDocDetected < MAX_FAILED_DETECTIONS) {
        cv::Mat resized;

        m_lastRect.copyDeskewedDocument(m_imgSaved, resized);
        cv::Size orig_size(m_imgSaved.cols, m_imgSaved.rows);
        cv::resize(resized, image, orig_size, 0, 0, cv::INTER_LANCZOS4);
        return true;
    } else {
        m_imgSaved.copyTo(image);
        return false;
    }
}

bool MTDetector::getDocumentImage(cv::Mat& srcImage, cv::Mat& destImage) {
    if (m_documentDetected || m_framesSinceDocDetected < MAX_FAILED_DETECTIONS) {
        cv::Mat resized;
        
        // Scale
        m_lastRect.rescale(srcImage);
        m_lastRect.copyDeskewedDocument(srcImage, resized);
        
        cv::Size orig_size(srcImage.cols, srcImage.rows);
        
        cv::resize(resized, destImage, orig_size, 0, 0, cv::INTER_LANCZOS4);
        return true;
    } else {
        destImage = srcImage;
        return false;
    }
}

void MTDetector::getDocumentPoints(std::vector<cv::Point>& points) const {
    m_lastRect.getPoints(points);
}

void MTDetector::processImage(cv::Mat& imgOriginal, EDGE_DETECTORS edgeMethod)
{
    std::vector<DocumentRectangle> rectangles;
    cv::Mat imgResult;

    m_frames++;

    int minDim = std::min(imgOriginal.cols, imgOriginal.rows);
    int scale = ceil(minDim / 360) + 1;
    int rescales = ceil(log(scale)/log(2));
    
    for (int i = 0;i < rescales;i++) {
        if (i) {
            cv::pyrDown(m_imgSaved, m_imgSaved);
        }
        else {
            cv::pyrDown(imgOriginal, m_imgSaved);
        }
    }

    /*if (scale == 2)
        cv::pyrDown(imgOriginal, m_imgSaved);
    else */
    //cv::resize(imgOriginal, m_imgSaved, cv::Size(), 1. / scale, 1. /scale);
    
    m_width = m_imgSaved.cols;
    m_height = m_imgSaved.rows;

    switch (edgeMethod) {
        case MT_EDGE_DETECT_TREES:
            // Fall through to Canny if the detector is not initialised
            if (m_dollar) {
                edgeDetectStructuredForest(imgResult);
                break;
            }

        default:
            edgeDetectCanny(imgResult);
            break;
    }

    findHoughLines(imgResult, 16, rectangles);

    int bestRectangleIndex = findBestRectangleIndex(rectangles);
    if (bestRectangleIndex != -1 || m_framesSinceDocDetected < MAX_FAILED_DETECTIONS) {
        imgOriginal.copyTo(m_imgSaved);
        if (bestRectangleIndex != -1) {
            m_lastRect = rectangles.at(bestRectangleIndex);
            m_lastRect.rescale(imgOriginal);
        }

        m_lastRect.draw(imgOriginal, cv::Scalar(0,255,0,255));

    }
    else {
#ifdef DEBUG_DRAWING
        //cv::resize(m_imgSaved, imgOriginal, cv::Size(), scale,scale);
        for (int i = 0;i < rescales;i++) {
            if (i) {
                cv::pyrUp(imgOriginal, imgOriginal);
            }
            else {
                cv::pyrUp(m_imgSaved, imgOriginal);
            }
        }
        cv::cvtColor(imgOriginal, imgOriginal, CV_RGB2BGR);
#else
        imgOriginal.copyTo(m_imgSaved);
#endif
    }

    if (bestRectangleIndex != -1) {
        m_documentDetected = true;
        m_framesSinceDocDetected = 0;
    }
    else {
        m_documentDetected = false;
        m_framesSinceDocDetected++;
    }
}

void MTDetector::edgeDetectCanny(cv::Mat& imgResult) {
    cv::Mat image_gray;

    cv::cvtColor(m_imgSaved, image_gray, CV_BGR2GRAY);
    cv::medianBlur(image_gray, image_gray, 11);
    cv::Canny(image_gray, imgResult, 75, 200);
}

void MTDetector::edgeDetectStructuredForest(cv::Mat& imgResult) {
    cv::Mat imgTemp, image_CV_32FC3;

    //cv::medianBlur(m_imgSaved, imgTemp, 11);
    //cv::cvtColor(imgTemp, imgTemp, CV_BGR2RGB);
    cv::cvtColor(m_imgSaved, imgTemp, CV_BGR2RGB);
    
    imgTemp.convertTo(image_CV_32FC3, CV_32FC3, 1/255.0);
    imgResult = cv::Mat(image_CV_32FC3.size(), image_CV_32FC3.type());

    //NSLog(@"Orig type %d, Type %d (%d)", image.type(), image_CV_32FC3.type(), CV_32FC3);
    m_dollar->detectEdges(image_CV_32FC3, imgResult);

    // computes orientation from edge map
    cv::Mat orientation_map;
    m_dollar->computeOrientation(imgResult, orientation_map);

    // suppress edges
    cv::Mat edge_nms;
    m_dollar->edgesNms(imgResult, orientation_map, edge_nms, 2, 0, 1, true);

    edge_nms.convertTo(imgResult, CV_8UC1, 255.);
}

void MTDetector::findHoughLines(cv::Mat& edgesNMS, size_t maxLines, std::vector<DocumentRectangle>& rectangles)
{
    std::vector<cv::Vec2f> lines;
    LineRecordVector lineCandidates;

    cv::HoughLines(edgesNMS, lines, 1, M_PI/180, 10);
    size_t num_to_check = std::min(maxLines, lines.size());

    for (size_t i = 0;i < num_to_check;++i)  {
        float rho = lines[i][0];
        float theta = lines[i][1];

        LineRecord lineRecord = LineRecord(rho, theta);

        if (!lineRecord.isSimilarLine(lineCandidates, m_width, m_height)) {
            lineCandidates.push_back(lineRecord);
        }
    }

    analyseLines(lineCandidates, rectangles);
}

void MTDetector::findHoughPLines(cv::Mat& edgesNMS, size_t maxLines, std::vector<DocumentRectangle>& rectangles)
{
    std::vector<cv::Vec4i> lines;
    LineRecordVector lineCandidates;
    int minDim = std::min(m_height, m_width);

    cv::HoughLinesP(edgesNMS, lines, 1, M_PI/180., 10, minDim / 10, minDim / 5);

    size_t num_to_check = std::min(maxLines, lines.size());

    for (size_t i = 0;i < num_to_check;++i)  {
        LineRecord lineRecord = LineRecord(lines[i][0], lines[i][1], lines[i][2], lines[i][3], m_width, m_height);

        if (!lineRecord.isSimilarLine(lineCandidates, m_width, m_height)) {
            lineCandidates.push_back(lineRecord);
        }
    }

    analyseLines(lineCandidates, rectangles);
}


void MTDetector::analyseLines(LineRecordVector& lineCandidates, std::vector<DocumentRectangle>& rectangles)
{
    LineRecordVector selectedCandidates;
    int counter = 0;
    for (LineRecordVector::iterator itr = lineCandidates.begin();
        itr != lineCandidates.end();++itr)
    {
        LineRecord& lineRecord = *(itr);

        for (int i = counter;i < lineCandidates.size();++i)
        {
            LineRecord& lineRecord2 = lineCandidates[i];
            // Default values match lines that do not intersect.
            if (&lineRecord2 == &lineRecord) {
                continue;
            }

            float angle = fabs(lineRecord.m_angle - lineRecord2.m_angle);
            bool isSimilarGrad = (angle < M_PI * 0.1 || angle > M_PI * 0.9);
            bool isApproxNormal = (angle < M_PI * 0.6 && angle > M_PI * 0.4);
            cv::Point intersection;
            /*bool intersectInImage = false;

            if (lineRecord.findLinesIntersectionPoint(lineRecord2, intersection)) {
                intersectInImage = ((intersection.x >= 0 || intersection.x <= m_width) &&
                   (intersection.y >= 0 || intersection.y <= m_height));
            }*/

            if (isSimilarGrad) {
                lineRecord.m_parallelSides.push_back(SideRecord(lineRecord2, angle, intersection));
            }
            else if (isApproxNormal) {
                lineRecord.m_normalSides.push_back(SideRecord(lineRecord2, angle, intersection));
            }
        }

        if (lineRecord.m_parallelSides.size() && lineRecord.m_normalSides.size() >= 2) {
            selectedCandidates.push_back(lineRecord);
        }
    }

    detectRectangle(selectedCandidates, rectangles);

#ifdef DEBUG_DRAWING
    if (findBestRectangleIndex(rectangles) == -1) {
        debugDrawResults(m_imgSaved, rectangles, lineCandidates, selectedCandidates);
    }
#endif
}

int MTDetector::findBestRectangleIndex(std::vector<DocumentRectangle>& rectangles) const {
    float bestScore = 0.0;
    int bestOrdinal = -1;

    for(int index = 0;index < rectangles.size();++index) {
        float thisScore = rectangles[index].getScore();
        if (thisScore > bestScore || index == 0) {
            bestScore = thisScore;
            bestOrdinal = index;
        }
    }

    return bestOrdinal;
}

void MTDetector::detectRectangle(const LineRecordVector& selectedCandidates, std::vector<DocumentRectangle>& rectangles) const
{
    for (LineRecordVector::const_iterator itr = selectedCandidates.begin();
          itr != selectedCandidates.end();++itr)
    {
        const LineRecord& lineRecord = *(itr);
        /* Iterate through possible sides and make combinations of
        one side with appprox same gradiant and two normals.
        Select using approx equal sides.
        Then do histrogram of enclosed rect to check it looks like paper.

        Decide final selection criteria - largest, strongest lines? Favor first
        found greater than approx 1/3 image size, otherwise largest found?
        */
        for (SideRecordVector::const_iterator pitr = lineRecord.m_parallelSides.begin();
              pitr != lineRecord.m_parallelSides.end();++pitr)
        {
            const SideRecord& oppositeSide = *(pitr);
            const LineRecord& pRecord = oppositeSide.lineRecord;
            for (size_t i = 0;i < lineRecord.m_normalSides.size() - 1;++i)
            {
                const SideRecord& adjSide1 = lineRecord.m_normalSides[i];
                const LineRecord& nRecord = adjSide1.lineRecord;

                for (size_t n = 1;n < lineRecord.m_normalSides.size();++n)
                {
                    const SideRecord& adjSide2 = lineRecord.m_normalSides[n];
                    const LineRecord& nRecord2 = adjSide2.lineRecord;

                    if (&nRecord == &nRecord2) {
                        continue;
                    }

                    DocumentRectangle rectangle(m_imgSaved);
                    bool possible_document = rectangle.assessRectangle(
                                        lineRecord, nRecord, pRecord, nRecord2, m_imgSaved);

                    if (!possible_document) {
                        continue;
                    }

                    rectangles.push_back(rectangle);
                }
            }
        }
    }
}

void MTDetector::debugDrawResults
(
    cv::Mat& image,
    const std::vector<DocumentRectangle>& rectangles,
    const LineRecordVector& lineCandidates,
    const LineRecordVector& selectedCandidates
) const
{
    int counter = 0;
    if (rectangles.size()) {
        cv::Scalar colours[] = {cv::Scalar(0,255,0), cv::Scalar(255,0,0),
            cv::Scalar(0,0,255) , cv::Scalar(255,0,255)};
        for (;counter < selectedCandidates.size();++counter)
        {
            selectedCandidates[counter].draw(image, colours[(counter / 4) % 4]);
        }
    }
    else if (selectedCandidates.size()) {
        for (;counter < lineCandidates.size();++counter)
        {
            lineCandidates[counter].debugDraw(image);
        }
    }
    else if (lineCandidates.size()) {
        cv::Scalar colours[] = {cv::Scalar(0,64,0), cv::Scalar(64,0,0), cv::Scalar(0,0,64) , cv::Scalar(64,0,64)};
        for (;counter < lineCandidates.size();++counter)
        {
            lineCandidates[counter].draw(image, colours[(counter / 4) % 4]);
        }
    }
}

#endif
