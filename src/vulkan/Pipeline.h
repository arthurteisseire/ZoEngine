#ifndef ZOENGINE_PIPELINE_H
#define ZOENGINE_PIPELINE_H

#include "VkRenderData.h"

class Pipeline
{
public:
    // TODO: Create a PipelineLayout class

    static bool init(VkRenderData &renderData,
                     VkPipelineLayout &pipelineLayout,
                     VkPipeline &pipeline,
                     const std::string &vertexShaderFilename,
                     const std::string &fragmentShaderFilename);

    static void cleanup(VkRenderData &renderData,
                        VkPipelineLayout &pipelineLayout,
                        VkPipeline &pipeline);
};


#endif //ZOENGINE_PIPELINE_H
