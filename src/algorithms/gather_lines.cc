//
//  gather_lines.cpp
//  confital-lib-mactest
//
//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#include "gather_lines.h"

void RectangleDetector::apply(PipelineJob& job) {
    group_parallel_and_normal_lines(job);
    generate_rectangles(job);
}

void RectangleDetector::group_parallel_and_normal_lines(PipelineJob& job)
{
    int counter = 0;
    LineVector& line_candidates = job.get_lines();
    for (LineVector::iterator itr = line_candidates.begin();
         itr != line_candidates.end();++itr)
    {
        Line& lineRecord = *(itr);
        
        for (int i = counter;i < line_candidates.size();++i)
        {
            Line& lineRecord2 = line_candidates[i];
            // Default values match lines that do not intersect.
            if (&lineRecord2 == &lineRecord) {
                continue;
            }
            
            float angle = fabs(lineRecord.m_angle - lineRecord2.m_angle);
            bool isSimilarGrad = (angle < M_PI * 0.1 || angle > M_PI * 0.9);
            bool isApproxNormal = (angle < M_PI * 0.6 && angle > M_PI * 0.4);
            
            if (isSimilarGrad) {
                lineRecord.m_parallelSides.push_back(SideRecord(lineRecord2, angle));
            }
            else if (isApproxNormal) {
                cv::Point intersection;
                lineRecord.find_intersection(lineRecord2, intersection);
                if (intersection_in_or_near_image(job, intersection))
                    lineRecord.m_normalSides.push_back(SideRecord(lineRecord2, angle));
            }
        }
        
        if (lineRecord.m_parallelSides.size() && lineRecord.m_normalSides.size() >= 2) {
            selectedCandidates.push_back(lineRecord);
        }
    }
}

void RectangleDetector::generate_rectangles(PipelineJob& job)
{
    for (LineVector::const_iterator itr = selectedCandidates.begin();
         itr != selectedCandidates.end();++itr)
    {
        const Line& lineRecord = *(itr);
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
            const Line& pRecord = oppositeSide.lineRecord;
            for (size_t i = 0;i < lineRecord.m_normalSides.size() - 1;++i)
            {
                const SideRecord& adjSide1 = lineRecord.m_normalSides[i];
                const Line& nRecord = adjSide1.lineRecord;
                
                for (size_t n = 1;n < lineRecord.m_normalSides.size();++n)
                {
                    const SideRecord& adjSide2 = lineRecord.m_normalSides[n];
                    const Line& nRecord2 = adjSide2.lineRecord;
                    
                    if (&nRecord == &nRecord2) {
                        continue;
                    }
                    
                    Document rectangle(job.get_initial_image());
                    bool possible_document = rectangle.assessRectangle(
                        lineRecord, nRecord, pRecord, nRecord2, job.get_initial_image());
                    
                    if (!possible_document) {
                        continue;
                    }
                    
                    job.add_rectangle(rectangle);
                }
            }
        }
    }
}

bool RectangleDetector::intersection_in_or_near_image
(
    const PipelineJob& job,
    const cv::Point& intersection
) const
{
    int x1 = -MAX_INTERSECTION_DISTANCE_OUTSIDE_IMAGE;
    int y1 = -MAX_INTERSECTION_DISTANCE_OUTSIDE_IMAGE;
    int x2 = job.image_width() + MAX_INTERSECTION_DISTANCE_OUTSIDE_IMAGE;
    int y2 = job.image_height() + MAX_INTERSECTION_DISTANCE_OUTSIDE_IMAGE;
    
    return ((intersection.x >= x1 && intersection.x <= x2) &&
            (intersection.y >= y1 && intersection.y <= y2));
}


