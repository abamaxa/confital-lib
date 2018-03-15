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


void LineDetector::apply(PipelineJob& job)
{
    find_hough_lines(job);
}

void LineDetector::find_hough_lines(PipelineJob& job)
{
    std::vector<cv::Vec2f> lines;
    
    cv::HoughLines(job.get_image(), lines, 1, M_PI/180, 10);
    size_t num_to_check = std::min(MAX_NUM_HOUGH_LINES_TO_DETECT, lines.size());
    
    for (size_t i = 0;i < num_to_check;++i)  {
        float rho = lines[i][0];
        float theta = lines[i][1];
        
        Line line_record = Line(rho, theta);
        job.add_line(line_record);
    }
}

void LineDetector::find_hough_lines_p(PipelineJob& job)
{
    std::vector<cv::Vec4i> lines;
    int min_dim = std::min(job.image_height(), job.image_width());
    
    cv::HoughLinesP(job.get_image(), lines, 1, M_PI/180., 10,
                    min_dim / 20, min_dim / 20);
    
    size_t num_to_check = std::min(MAX_NUM_HOUGH_P_LINES_TO_DETECT, lines.size());
    
    for (size_t i = 0;i < num_to_check;++i)  {
        Line line_record = Line(lines[i]);
        job.add_line(line_record);
    }
}

