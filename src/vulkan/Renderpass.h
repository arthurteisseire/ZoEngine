#ifndef ZOENGINE_RENDERPASS_H
#define ZOENGINE_RENDERPASS_H

#include <vulkan/vulkan.h>
#include "VkRenderData.h"

class Renderpass
{
public:
    static bool init(VkRenderData &renderData);
    static void cleanup(VkRenderData &renderData);
};


#endif //ZOENGINE_RENDERPASS_H
