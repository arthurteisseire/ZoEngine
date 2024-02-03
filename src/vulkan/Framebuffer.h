#ifndef ZOENGINE_FRAMEBUFFER_H
#define ZOENGINE_FRAMEBUFFER_H

#include "VkRenderData.h"

class Framebuffer
{
public:
    static bool init(VkRenderData &renderData);
    static void cleanup(VkRenderData &renderData);
};


#endif //ZOENGINE_FRAMEBUFFER_H
