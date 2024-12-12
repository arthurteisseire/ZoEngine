#ifndef ZOENGINE_VKRENDERDATA_H
#define ZOENGINE_VKRENDERDATA_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

struct Camera
{
    glm::vec3 position;
    float yawAngle = 0.f;
    float pitchAngle = 0.f;

    [[nodiscard]] glm::quat GetOrientation() const
    {
        glm::vec3 upVector(0.f, 1.f, 0.f);
        glm::quat yawQuat = glm::angleAxis(glm::radians(yawAngle), upVector);

        glm::vec3 rightVector(1.f, 0.f, 0.f);
        glm::quat pitchQuat = glm::angleAxis(glm::radians(pitchAngle), rightVector);

        return yawQuat * pitchQuat;
    }
};

struct MeshVertex
{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 uv;
};

struct BasicMesh
{
    std::vector<MeshVertex> vertices;
};

struct VkUploadMatrices
{
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};

struct VkRenderData
{
    GLFWwindow *rdWindow = nullptr;

    std::size_t mTriangleCount = 0;
    int rdWidth = 0;
    int rdHeight = 0;

    bool mUseChangedShader = false;

    VmaAllocator rdAllocator;

    vkb::Instance rdVkbInstance{};
    vkb::PhysicalDevice rdVkbPhysicalDevice{};
    vkb::Device rdVkbDevice{};
    vkb::Swapchain rdVkbSwapchain{};

    std::vector<VkImage> rdSwapchainImages;
    std::vector<VkImageView> rdSwapchainImageViews;
    std::vector<VkFramebuffer> rdFramebuffers;

    VkQueue rdGraphicsQueue = VK_NULL_HANDLE;
    VkQueue rdPresentQueue = VK_NULL_HANDLE;

    VkImage rdDepthImage = VK_NULL_HANDLE;
    VkImageView rdDepthImageView = VK_NULL_HANDLE;
    VkFormat rdDepthFormat;
    VmaAllocation rdDepthImageAlloc = VK_NULL_HANDLE;

    VkRenderPass rdRenderpass = VK_NULL_HANDLE;
    VkPipelineLayout rdPipelineLayout = VK_NULL_HANDLE;
    VkPipeline rdPipeline = VK_NULL_HANDLE;
    VkPipelineLayout rdPipelineLayoutChanged = VK_NULL_HANDLE;
    VkPipeline rdPipelineChanged = VK_NULL_HANDLE;

    VkCommandPool rdCommandPool = VK_NULL_HANDLE;
    VkCommandBuffer rdCommandBuffer = VK_NULL_HANDLE;

    VkSemaphore rdPresentSemaphore = VK_NULL_HANDLE;
    VkSemaphore rdRenderSemaphore = VK_NULL_HANDLE;
    VkFence rdRenderFence = VK_NULL_HANDLE;

    VkImage rdTextureImage = VK_NULL_HANDLE;
    VkImageView rdTextureImageView = VK_NULL_HANDLE;
    VkSampler rdTextureSampler = VK_NULL_HANDLE;
    VmaAllocation rdTextureImageAlloc = VK_NULL_HANDLE;

    VkDescriptorPool rdDescriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout rdTextureDescriptorLayout = VK_NULL_HANDLE;
    VkDescriptorSet rdDescriptorSet = VK_NULL_HANDLE;

    VkBuffer rdUboBuffer = VK_NULL_HANDLE;
    VmaAllocation rdUboBufferAlloc = nullptr;

    VkDescriptorPool rdUBODescriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout rdUBODescriptorLayout = VK_NULL_HANDLE;
    VkDescriptorSet rdUBODescriptorSet = VK_NULL_HANDLE;

    VkDescriptorPool rdImguiDescriptorPool = VK_NULL_HANDLE;

    Camera camera;
};

#endif //ZOENGINE_VKRENDERDATA_H
