//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#include "line_detector.h"

/*
 Mat face = Mat(200, 200, CV_8UC1);
 vector<Mat> regions;
 Mat region_frame;
 int width = face.cols * 0.05;
 int heigth = face.rows * 0.05;
 for(int y=0; y<=(face.rows - heigth); y+=heigth)
 {
 for(int x=0; x<=(face.cols - width); x+=width)
 {
 Rect region = Rect(x, y, width, heigth);
 region_frame = face(region);
 regions.push_back(region_frame);
 }
 }
 */

LineDetector::LineDetector(const PipelineJob& _job, size_t _max_lines)
:  job(_job), max_lines(_max_lines)
{
    
}

void LineDetector::apply(cv::Mat& image)
{
    find_hough_lines(image);
}

void LineDetector::find_hough_lines(cv::Mat& image)
{
    std::vector<cv::Vec2f> lines;
    
    cv::HoughLines(image, lines, 1, M_PI/180, 10);
    size_t num_to_check = std::min(max_lines, lines.size());
    
    for (size_t i = 0;i < num_to_check;++i)  {
        float rho = lines[i][0];
        float theta = lines[i][1];
        
        /*Line line_record = Line(job, rho, theta);
        
        if (!line_record.is_similar_line(line_candidates)) {
            line_candidates.push_back(line_record);
        }*/
    }
}

void LineDetector::find_hough_lines_p(cv::Mat& image)
{
    std::vector<cv::Vec4i> lines;
    int width = image.cols;
    int height = image.rows;
    int min_dim = std::min(height, width);
    
    cv::HoughLinesP(image, lines, 1, M_PI/180., 10, min_dim / 10, min_dim / 5);
    
    size_t num_to_check = std::min(max_lines, lines.size());
    
    for (size_t i = 0;i < num_to_check;++i)  {
        /*Line line_record = Line(job, lines[i]);
        
        if (!line_record.is_similar_line(line_candidates)) {
            line_candidates.push_back(line_record);
        }*/
    }
}

