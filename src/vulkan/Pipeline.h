#ifndef ZOENGINE_PIPELINE_H
#define ZOENGINE_PIPELINE_H

#include "VkRenderData.h"

class Pipeline
{
public:
    static bool init(VkRenderData &renderData, const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename);
    static void cleanup(VkRenderData &renderData);
};


#endif //ZOENGINE_PIPELINE_H
