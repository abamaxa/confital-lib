//  Created by Chris Morgan on 22/12/17.
//  Copyright © 2017 Chris Morgan. All rights reserved.

#ifdef __cplusplus
#import <cmath>
#import <algorithm>

#import "detector.h"

#include "pipeline_job.h"
#include "algorithms/algorithms.h"

//typedef std::vector<cv::Point> PointVector;

Detector::Detector() :
    m_frames(0),
    m_documentDetected(false),
    m_framesSinceDocDetected(1000),
    rescale_factor(1)
{

}

void Detector::reset() {
    m_frames = 0;
    m_documentDetected = false;
    m_framesSinceDocDetected = 1000;

    m_lastRect.reset();
}

bool Detector::initialize(cv::String pathToModel)
{
    //NSString* modelFilename = [[NSBundle mainBundle] pathForResource:@"model.yml" ofType:@"gz"];
    //cv::String modelFilenameStr = std::string([modelFilename UTF8String]);
    m_dollar = cv::ximgproc::createStructuredEdgeDetection(pathToModel);
    return true;
}

void Detector::processImage(cv::Mat& imgOriginal, EDGE_DETECTORS edgeMethod)
{
    cv::Mat imgResult;

    m_frames++;

    calculate_rescale_factor(imgOriginal);
    scale_image(imgOriginal);

    PipelineJob job(m_imgSaved);
    CannyEdgeDetector canny_detector;
    TreesEdgeDetector trees_detector(m_dollar);
    LineDetector line_detector;
    RectangleDetector rectangle_detector;
    BestRectangleSelector rectangle_selector;
    
    canny_detector.apply(job);
    line_detector.apply(job);
    rectangle_detector.apply(job);
    rectangle_selector.apply(job);
    
#ifdef DEBUG_DRAWING
    DebugDrawings draw_detected_objects;
    draw_detected_objects.apply(job);
#endif
    
    const Document& rectangle = job.get_result();
    
    if (rectangle.is_valid() || m_framesSinceDocDetected < MAX_FAILED_DETECTIONS) {
        imgOriginal.copyTo(m_imgSaved);
        if (rectangle.is_valid()) {
            m_lastRect = rectangle;
            m_lastRect.rescale(imgOriginal);
        }

        m_lastRect.draw(imgOriginal, cv::Scalar(0,255,0,255));
    }
    else {
#ifdef DEBUG_DRAWING
        rescale_image(imgOriginal);
#else
        imgOriginal.copyTo(m_imgSaved);
#endif
    }

    update_detection_state(rectangle);
}

void Detector::calculate_rescale_factor(cv::Mat& imgOriginal) {
    int minDim = std::min(imgOriginal.cols, imgOriginal.rows);
    int scale = ceil(minDim / 360) + 1;
    rescale_factor = ceil(log(scale)/log(2));
}

void Detector::scale_image(cv::Mat& imgOriginal) {
    for (int i = 0;i < rescale_factor;i++) {
        if (i) {
            cv::pyrDown(m_imgSaved, m_imgSaved);
        }
        else {
            cv::pyrDown(imgOriginal, m_imgSaved);
        }
    }
}

void Detector::rescale_image(cv::Mat& imgOriginal) {
    //cv::resize(m_imgSaved, imgOriginal, cv::Size(), scale,scale);
    for (int i = 0;i < rescale_factor;i++) {
        if (i) {
            cv::pyrUp(imgOriginal, imgOriginal);
        }
        else {
            cv::pyrUp(m_imgSaved, imgOriginal);
        }
    }
    cv::cvtColor(imgOriginal, imgOriginal, CV_RGB2BGR);
}

void Detector::update_detection_state(const Document& rectangle) {
    if (rectangle.is_valid()) {
        m_documentDetected = true;
        m_framesSinceDocDetected = 0;
    }
    else {
        m_documentDetected = false;
        m_framesSinceDocDetected++;
    }
}

bool Detector::getDocumentImage(cv::Mat& image) {
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

bool Detector::getDocumentImage(cv::Mat& srcImage, cv::Mat& destImage) {
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

void Detector::getDocumentPoints(std::vector<cv::Point>& points) const {
    m_lastRect.getPoints(points);
}

#endif
