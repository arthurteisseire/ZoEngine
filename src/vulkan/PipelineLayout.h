#ifndef ZOENGINE_PIPELINELAYOUT_H
#define ZOENGINE_PIPELINELAYOUT_H

#include "VkRenderData.h"

class PipelineLayout
{
public:
    static bool init(VkRenderData& renderData, VkPipelineLayout& pipelineLayout);
    static void cleanup(VkRenderData &renderData, VkPipelineLayout &pipelineLayout);
};


#endif //ZOENGINE_PIPELINELAYOUT_H
