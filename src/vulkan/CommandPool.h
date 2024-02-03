#ifndef ZOENGINE_COMMANDPOOL_H
#define ZOENGINE_COMMANDPOOL_H

#include <vulkan/vulkan.h>
#include "VkRenderData.h"

class CommandPool
{
public:
    static bool init(VkRenderData &renderData);
    static void cleanup(VkRenderData &renderData);
};

#endif //ZOENGINE_COMMANDPOOL_H
