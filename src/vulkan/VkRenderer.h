#ifndef ZOENGINE_VKRENDERER_H
#define ZOENGINE_VKRENDERER_H


#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>
#include <VkBootstrap.h>
#include <GLFW/glfw3.h>
#include "VkRenderData.h"
#include "UserInterface.h"

class VkRenderer
{
public:
    explicit VkRenderer(GLFWwindow *window);
    bool init(unsigned int width, unsigned int height);
    void setSize(unsigned int width, unsigned int height);
    bool uploadData(BasicMesh vertexData);
    bool draw();
    void cleanup();
    void handleKeyEvents(int key, int scancode, int action, int mods);

private:
    VkRenderData mRenderData{};

    UserInterface mUserInterface;

    VkSurfaceKHR mSurface = VK_NULL_HANDLE;

    VkBuffer mVertexBuffer;
    VmaAllocation mVertexBufferAlloc;

    VkUploadMatrices mMatrices{};

    bool mUseChangedShader = false;

    bool deviceInit();
    bool getQueue();
    bool createDepthBuffer();
    bool createSwapchain();
    bool createRenderPass();
    bool createPipelineLayout();
    bool createPipelines();
    bool createFramebuffer();
    bool createCommandPool();
    bool createCommandBuffer();
    bool createSyncObjects();
    bool loadTexture();

    bool initVma();
    bool initUserInterface();

    bool recreateSwapchain();
    bool createUBO();
};


#endif //ZOENGINE_VKRENDERER_H
