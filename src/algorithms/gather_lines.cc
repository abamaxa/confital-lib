//  Created by Chris Morgan on 14/3/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.

#include "gather_lines.h"

bool line_in_existing_group(ApproxParallelLineGroup groups, const Line& line);

void LineGatherer::apply(PipelineJob& job) {
    LineVector& lines = job.get_lines();
    ApproxParallelLineGroup groups;
    
    for (auto& line : lines) {
        if (line_in_existing_group(groups, line))
            continue;
        
        ApproxParallelLines group(line);
        
        group.add_all_approx_parallel_lines(lines);
        groups.push_back(group);
    }
    
    job.set_parallel_line_groups(groups);
}

bool line_in_existing_group(ApproxParallelLineGroup groups, const Line& line) {
    return std::any_of(groups.begin(), groups.end(),
        [line](const ApproxParallelLines& group) {
            return group.line_in_group(line);
        }
    );
}
