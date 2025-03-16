#ifndef ZOENGINE_VKRENDERDATA_H
#define ZOENGINE_VKRENDERDATA_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>
#include <optional>
#include <algorithm>

struct Camera
{
    // Position
    glm::vec3 desiredMoveDirection;
    glm::vec3 worldPosition;
    float moveSpeed = 5.f; // units per second

    // Orientation
    glm::vec2 desiredRotationAngle;
    glm::vec2 yawPitchAngles;
    float rotateSpeed = 90.f; // degrees per seconds

    // Inputs
    bool mouseLock = false;

    void ApplyDesiredTransform(float dt)
    {
        ApplyDesiredMovement(dt);
        ApplyDesiredOrientation(dt);
    }

    [[nodiscard]] glm::quat GetOrientation() const
    {
        const glm::vec3 upVector(0.f, 1.f, 0.f);
        const glm::quat yawQuat = glm::angleAxis(glm::radians(yawPitchAngles[0]), upVector);

        const glm::vec3 rightVector(1.f, 0.f, 0.f);
        const glm::quat pitchQuat = glm::angleAxis(glm::radians(yawPitchAngles[1]), rightVector);

        return yawQuat * pitchQuat;
    }

private:
    void ApplyDesiredMovement(float dt)
    {
        worldPosition += desiredMoveDirection * moveSpeed * dt;
    }

    void ApplyDesiredOrientation(float dt)
    {
        yawPitchAngles += desiredRotationAngle * rotateSpeed * dt;
        yawPitchAngles[0] = NormalizeAngle(yawPitchAngles[0]);
        yawPitchAngles[1] = std::clamp(yawPitchAngles[1], -89.9f, 89.9f);
    }

    static float NormalizeAngle(const float angle)
    {
        float normalizedAngle = std::fmod(angle, 360.f);
        if (angle < 0.f)
        {
            normalizedAngle += 360.f;
        }
        return normalizedAngle;
    }
};

struct Mouse
{
    std::optional<glm::vec2> lastPos;
};

struct VVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

struct VMesh
{
    std::vector<VVertex> vertices;

    VkIndexType indexType;
    std::vector<uint8_t> indices;
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

    std::size_t mIndexCount = 0;
    VkIndexType mIndexType;

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

    Mouse mMouse;
    Camera mCamera;
    float lastTickTime;
    float deltaTime;
};

#endif //ZOENGINE_VKRENDERDATA_H
