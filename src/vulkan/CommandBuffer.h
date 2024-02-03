#ifndef ZOENGINE_COMMANDBUFFER_H
#define ZOENGINE_COMMANDBUFFER_H

#include <vulkan/vulkan.h>
#include "VkRenderData.h"

class CommandBuffer
{
public:
    static bool init(VkRenderData &renderData, VkCommandBuffer &commandBuffer);
    static void cleanup(VkRenderData &renderData, VkCommandBuffer &commandBuffer);
};

#endif //ZOENGINE_COMMANDBUFFER_H
