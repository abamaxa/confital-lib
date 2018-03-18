//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

#include "best_rectangle.h"

void BestRectangleSelector::apply(PipelineJob& job) {
    float bestScore = 0.0;
    int bestOrdinal = -1;
    
    Document best_rectangle;
    std::vector<Document> rectangles = job.get_rectangles();
    
    for(size_t index = 0;index < rectangles.size();++index) {
        float thisScore = rectangles[index].get_score();
        if (thisScore > bestScore || index == 0) {
            bestScore = thisScore;
            bestOrdinal = static_cast<int>(index);
        }
    }
    
    if (bestOrdinal != -1) {
        best_rectangle = rectangles.at(bestOrdinal);
        job.set_result(best_rectangle);
    }  
}

