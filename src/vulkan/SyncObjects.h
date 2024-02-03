#ifndef ZOENGINE_SYNCOBJECTS_H
#define ZOENGINE_SYNCOBJECTS_H

#include <vulkan/vulkan.h>
#include "VkRenderData.h"

class SyncObjects
{
public:
    static bool init(VkRenderData &renderData);
    static void cleanup(VkRenderData &renderData);
};


#endif //ZOENGINE_SYNCOBJECTS_H
