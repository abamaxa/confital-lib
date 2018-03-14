//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#include "debug_drawings.h"

void DebugDrawings::apply(PipelineJob& job) {
    const Document& result = job.get_result();
    if (!result.is_valid()) {
        draw_detected_lines(job);
    }
}

void DebugDrawings::draw_detected_lines(PipelineJob& job) const
{
    int counter = 0;
    std::vector<Document> rectangles = job.get_rectangles();
    const LineVector& lines = job.get_lines();
    cv::Mat image = job.get_initial_image();
    
    if (rectangles.size()) {
        cv::Scalar colours[] = {cv::Scalar(0,255,0), cv::Scalar(255,0,0),
        cv::Scalar(0,0,255) , cv::Scalar(255,0,255)};
        for (;counter < rectangles.size();++counter)
        {
            rectangles[counter].draw(image, colours[(counter / 4) % 4]);
        }
    }
    else if (lines.size()) {
        cv::Scalar colours[] = {cv::Scalar(0,64,0),
            cv::Scalar(64,0,0), cv::Scalar(0,0,64) , cv::Scalar(64,0,64)};
        
        for (;counter < lines.size();++counter)
        {
            lines[counter].debugDraw(image);
            //lines[counter].draw(image, colours[(counter / 4) % 4]);
        }
    }
}

