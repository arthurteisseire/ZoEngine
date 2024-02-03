#ifndef ZOENGINE_TEXTURE_H
#define ZOENGINE_TEXTURE_H

#include <string>
#include <vulkan/vulkan.h>
#include "VkRenderData.h"

class Texture
{
public:
    static bool loadTexture(VkRenderData &renderData, const std::string& textureFilename);
    static void cleanup(VkRenderData &renderData);
};

#endif //ZOENGINE_TEXTURE_H
