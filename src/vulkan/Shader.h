#ifndef ZOENGINE_SHADER_H
#define ZOENGINE_SHADER_H

#include <string>
#include <vulkan/vulkan.h>

class Shader
{
public:
    static VkShaderModule loadShader(VkDevice device, std::string shaderFileName);

private:
    static std::string loadFileToString(std::string fileName);
};

#endif //ZOENGINE_SHADER_H
