#ifndef ZOENGINE_UNIFORMBUFFER_H
#define ZOENGINE_UNIFORMBUFFER_H

#include <vulkan/vulkan.h>
#include "VkRenderData.h"

class UniformBuffer
{
public:
    static bool init(VkRenderData& renderData);
    static void uploadData(VkRenderData& renderData, VkUploadMatrices& matrices);
    static void cleanup(VkRenderData& renderData);
};


#endif //ZOENGINE_UNIFORMBUFFER_H
