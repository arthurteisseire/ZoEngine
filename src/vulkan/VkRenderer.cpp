// Defining VMA_IMPLEMENTATION must be done in cpp file only ONE TIME just above the vk_mem_alloc.h.
#define VMA_IMPLEMENTATION

#include <vk_mem_alloc.h>
#include <glm/gtc/matrix_transform.hpp>

#include "VkRenderer.h"
#include "Logger.h"
#include "Framebuffer.h"
#include "Renderpass.h"
#include "Pipeline.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "SyncObjects.h"
#include "Texture.h"
#include "UniformBuffer.h"
#include "PipelineLayout.h"

VkRenderer::VkRenderer(GLFWwindow *window)
{
    mRenderData.rdWindow = window;
    mRenderData.mCamera.worldPosition = glm::vec3(0.4f, 0.3f, 1.0f);

    // Identity matrices
    mMatrices.viewMatrix = glm::mat4(1.0f);
    mMatrices.projectionMatrix = glm::mat4(1.0f);
}

bool VkRenderer::init(unsigned int width, unsigned int height)
{
    if (!mRenderData.rdWindow)
    {
        Logger::error("%s error: invalid GLFWwindow handle\n", __FUNCTION__);
        return false;
    }

    if (!deviceInit())
    {
        return false;
    }

    if (!initVma())
    {
        return false;
    }

    if (!getQueue())
    {
        return false;
    }

    if (!createSwapchain())
    {
        return false;
    }

    /* must be done AFTER swapchain as we need data from it */
    if (!createDepthBuffer())
    {
        return false;
    }

    if (!createCommandPool())
    {
        return false;
    }

    if (!createCommandBuffer())
    {
        return false;
    }

    /* we need the command pool */
    if (!loadTexture())
    {
        return false;
    }

    if (!createUBO())
    {
        return false;
    }

    if (!createRenderPass())
    {
        return false;
    }

    if (!createPipelineLayout())
    {
        return false;
    }

    /* pipeline needs texture layout */
    if (!createPipelines())
    {
        return false;
    }

    if (!createFramebuffer())
    {
        return false;
    }

    if (!createSyncObjects())
    {
        return false;
    }

    if (!initUserInterface())
    {
        return false;
    }

    setSize(width, height);

    Logger::info("%s: Vulkan renderer initialized to %ix%i\n", __FUNCTION__, width, height);
    return true;
}

bool VkRenderer::deviceInit()
{
    /* instance and window */
    vkb::InstanceBuilder instBuild;
    auto instRet = instBuild.use_default_debug_messenger().request_validation_layers().build();
    // auto instRet = instBuild.build();
    if (!instRet)
    {
        Logger::info("%s error: could not build vkb instance\n", __FUNCTION__);
        return false;
    }
    mRenderData.rdVkbInstance = instRet.value();

    VkResult result = VK_ERROR_UNKNOWN;
    result = glfwCreateWindowSurface(mRenderData.rdVkbInstance, mRenderData.rdWindow, nullptr, &mSurface);
    if (result != VK_SUCCESS)
    {
        Logger::info("%s error: Could not create Vulkan surface\n", __FUNCTION__);
        return false;
    }

    /* just get the first available device */
    vkb::PhysicalDeviceSelector physicalDevSel{mRenderData.rdVkbInstance};
    auto physicalDevSelRet = physicalDevSel.set_surface(mSurface).select();
    if (!physicalDevSelRet)
    {
        Logger::info("%s error: could not get physical devices\n", __FUNCTION__);
        return false;
    }
    mRenderData.rdVkbPhysicalDevice = physicalDevSelRet.value();

    Logger::info("%s: found physical device '%s'\n", __FUNCTION__, mRenderData.rdVkbPhysicalDevice.name.c_str());

    vkb::DeviceBuilder devBuilder{mRenderData.rdVkbPhysicalDevice};
    auto devBuilderRet = devBuilder.build();
    if (!devBuilderRet)
    {
        Logger::info("%s error: could not get devices\n", __FUNCTION__);
        return false;
    }
    mRenderData.rdVkbDevice = devBuilderRet.value();

    return true;
}

bool VkRenderer::getQueue()
{
    auto graphQueueRet = mRenderData.rdVkbDevice.get_queue(vkb::QueueType::graphics);
    if (!graphQueueRet.has_value())
    {
        Logger::info("%s error: could not get graphics queue\n", __FUNCTION__);
        return false;
    }
    mRenderData.rdGraphicsQueue = graphQueueRet.value();

    auto presentQueueRet = mRenderData.rdVkbDevice.get_queue(vkb::QueueType::present);
    if (!presentQueueRet.has_value())
    {
        Logger::info("%s error: could not get present queue\n", __FUNCTION__);
        return false;
    }
    mRenderData.rdPresentQueue = presentQueueRet.value();

    return true;
}

bool VkRenderer::createDepthBuffer()
{
    VkExtent3D depthImageExtent = {
            mRenderData.rdVkbSwapchain.extent.width,
            mRenderData.rdVkbSwapchain.extent.height,
            1
    };

    mRenderData.rdDepthFormat = VK_FORMAT_D32_SFLOAT;

    VkImageCreateInfo depthImageInfo{};
    depthImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    depthImageInfo.imageType = VK_IMAGE_TYPE_2D;
    depthImageInfo.format = mRenderData.rdDepthFormat;
    depthImageInfo.extent = depthImageExtent;
    depthImageInfo.mipLevels = 1;
    depthImageInfo.arrayLayers = 1;
    depthImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    depthImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    VmaAllocationCreateInfo depthAllocInfo{};
    depthAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    depthAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vmaCreateImage(mRenderData.rdAllocator, &depthImageInfo, &depthAllocInfo, &mRenderData.rdDepthImage,
                       &mRenderData.rdDepthImageAlloc, nullptr) != VK_SUCCESS)
    {
        Logger::info("%s error: could not allocate depth buffer memory\n", __FUNCTION__);
        return false;
    }

    VkImageViewCreateInfo depthImageViewinfo{};
    depthImageViewinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    depthImageViewinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depthImageViewinfo.image = mRenderData.rdDepthImage;
    depthImageViewinfo.format = mRenderData.rdDepthFormat;
    depthImageViewinfo.subresourceRange.baseMipLevel = 0;
    depthImageViewinfo.subresourceRange.levelCount = 1;
    depthImageViewinfo.subresourceRange.baseArrayLayer = 0;
    depthImageViewinfo.subresourceRange.layerCount = 1;
    depthImageViewinfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    if (vkCreateImageView(mRenderData.rdVkbDevice.device, &depthImageViewinfo, nullptr,
                          &mRenderData.rdDepthImageView) != VK_SUCCESS)
    {
        Logger::info("%s error: could not create depth buffer image view\n", __FUNCTION__);
        return false;
    }
    return true;
}

bool VkRenderer::createSwapchain()
{
    vkb::SwapchainBuilder swapChainBuild{mRenderData.rdVkbDevice};

    /* VK_PRESENT_MODE_FIFO_KHR enables vsync */
    auto swapChainBuildRet = swapChainBuild.set_old_swapchain(mRenderData.rdVkbSwapchain).set_desired_present_mode(
            VK_PRESENT_MODE_FIFO_KHR).build();
    if (!swapChainBuildRet)
    {
        Logger::info("%s error: could not init swapchain\n", __FUNCTION__);
        return false;
    }

    vkb::destroy_swapchain(mRenderData.rdVkbSwapchain);
    mRenderData.rdVkbSwapchain = swapChainBuildRet.value();

    return true;
}

bool VkRenderer::recreateSwapchain()
{
    /* handle minimize */
    int width = 0, height = 0;
    glfwGetFramebufferSize(mRenderData.rdWindow, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(mRenderData.rdWindow, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(mRenderData.rdVkbDevice.device);

    /* cleanup */
    Framebuffer::cleanup(mRenderData);
    vkDestroyImageView(mRenderData.rdVkbDevice.device, mRenderData.rdDepthImageView, nullptr);
    vmaDestroyImage(mRenderData.rdAllocator, mRenderData.rdDepthImage, mRenderData.rdDepthImageAlloc);

    mRenderData.rdVkbSwapchain.destroy_image_views(mRenderData.rdSwapchainImageViews);

    /* and recreate */
    if (!createSwapchain())
    {
        Logger::info("%s error: could not recreate swapchain\n", __FUNCTION__);
        return false;
    }

    if (!createDepthBuffer())
    {
        Logger::info("%s error: could not recreate depth buffer\n", __FUNCTION__);
        return false;
    }

    if (!createFramebuffer())
    {
        Logger::info("%s error: could not recreate framebuffers\n", __FUNCTION__);
        return false;
    }

    return true;
}

bool VkRenderer::createRenderPass()
{
    if (!Renderpass::init(mRenderData))
    {
        Logger::info("%s error: could not init renderpass\n", __FUNCTION__);
        return false;
    }
    return true;
}

bool VkRenderer::createPipelineLayout()
{
    if (!PipelineLayout::init(mRenderData, mRenderData.rdPipelineLayout))
    {
        Logger::error("%s error: could not init pipeline layout\n", __FUNCTION__);
        return false;
    }
    if (!PipelineLayout::init(mRenderData, mRenderData.rdPipelineLayoutChanged))
    {
        Logger::error("%s error: could not init pipeline layout\n", __FUNCTION__);
        return false;
    }
    return true;
}

bool VkRenderer::createPipelines()
{
    std::string shaderOutputDir = "../cmake-build-debug/compiled_shaders/";
    std::string vertexShaderFile = shaderOutputDir + "basic.vert.spv";
    std::string fragmentShaderFile = shaderOutputDir + "basic.frag.spv";
    if (!Pipeline::init(mRenderData,
                        mRenderData.rdPipelineLayout,
                        mRenderData.rdPipeline,
                        vertexShaderFile,
                        fragmentShaderFile))
    {
        Logger::info("%s error: could not init pipeline\n", __FUNCTION__);
        return false;
    }

    std::string vertexShaderFileChanged = shaderOutputDir + "changed.vert.spv";
    std::string fragmentShaderFileChanged = shaderOutputDir + "changed.frag.spv";
    if (!Pipeline::init(mRenderData,
                        mRenderData.rdPipelineLayoutChanged,
                        mRenderData.rdPipelineChanged,
                        vertexShaderFileChanged,
                        fragmentShaderFileChanged))
    {
        Logger::info("%s error: could not init pipeline changed\n", __FUNCTION__);
        return false;
    }
    return true;
}

bool VkRenderer::createFramebuffer()
{
    if (!Framebuffer::init(mRenderData))
    {
        Logger::info("%s error: could not init framebuffer\n", __FUNCTION__);
        return false;
    }
    return true;
}

bool VkRenderer::createCommandPool()
{
    if (!CommandPool::init(mRenderData))
    {
        Logger::info("%s error: could not create command pool\n", __FUNCTION__);
        return false;
    }
    return true;
}

bool VkRenderer::createCommandBuffer()
{
    if (!CommandBuffer::init(mRenderData, mRenderData.rdCommandBuffer))
    {
        Logger::info("%s error: could not create command buffers\n", __FUNCTION__);
        return false;
    }
    return true;
}

bool VkRenderer::createSyncObjects()
{
    if (!SyncObjects::init(mRenderData))
    {
        Logger::info("%s error: could not create sync objects\n", __FUNCTION__);
        return false;
    }

    return true;
}

bool VkRenderer::createUBO()
{
    if (!UniformBuffer::init(mRenderData))
    {
        Logger::error("%s error: could not create uniform buffers\n", __FUNCTION__);
        return false;
    }
    return true;
}

bool VkRenderer::loadTexture()
{
    std::string textureFileName = "../resource/texture/crate.png";
    if (!Texture::loadTexture(mRenderData, textureFileName))
    {
        Logger::info("%s error: could not load texture\n", __FUNCTION__);
        return false;
    }

    return true;
}

bool VkRenderer::initVma()
{
    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.physicalDevice = mRenderData.rdVkbPhysicalDevice.physical_device;
    allocatorInfo.device = mRenderData.rdVkbDevice.device;
    allocatorInfo.instance = mRenderData.rdVkbInstance.instance;
    if (vmaCreateAllocator(&allocatorInfo, &mRenderData.rdAllocator) != VK_SUCCESS)
    {
        Logger::info("%s error: could not init VMA\n", __FUNCTION__);
        return false;
    }

    return true;
}

bool VkRenderer::initUserInterface()
{
    if (!mUserInterface.init(mRenderData))
    {
        Logger::error("%s error: Can't init user inteface (imgui)", __FUNCTION__);
        return false;
    }
    return true;
}

void VkRenderer::cleanup()
{
    vkDeviceWaitIdle(mRenderData.rdVkbDevice.device);

    mUserInterface.cleanup(mRenderData);

    Texture::cleanup(mRenderData);
    vmaDestroyBuffer(mRenderData.rdAllocator, mVertexBuffer, mVertexBufferAlloc);
    vmaDestroyBuffer(mRenderData.rdAllocator, mIndexBuffer, mIndexBufferAlloc);

    SyncObjects::cleanup(mRenderData);
    CommandBuffer::cleanup(mRenderData, mRenderData.rdCommandBuffer);
    CommandPool::cleanup(mRenderData);
    Framebuffer::cleanup(mRenderData);
    Pipeline::cleanup(mRenderData, mRenderData.rdPipeline);
    PipelineLayout::cleanup(mRenderData, mRenderData.rdPipelineLayout);
    Pipeline::cleanup(mRenderData, mRenderData.rdPipelineChanged);
    PipelineLayout::cleanup(mRenderData, mRenderData.rdPipelineLayoutChanged);
    Renderpass::cleanup(mRenderData);
    UniformBuffer::cleanup(mRenderData);

    vkDestroyImageView(mRenderData.rdVkbDevice.device, mRenderData.rdDepthImageView, nullptr);
    vmaDestroyImage(mRenderData.rdAllocator, mRenderData.rdDepthImage, mRenderData.rdDepthImageAlloc);
    vmaDestroyAllocator(mRenderData.rdAllocator);

    mRenderData.rdVkbSwapchain.destroy_image_views(mRenderData.rdSwapchainImageViews);
    vkb::destroy_swapchain(mRenderData.rdVkbSwapchain);

    vkb::destroy_device(mRenderData.rdVkbDevice);
    vkb::destroy_surface(mRenderData.rdVkbInstance.instance, mSurface);
    vkb::destroy_instance(mRenderData.rdVkbInstance);


    Logger::info("%s: Vulkan renderer destroyed\n", __FUNCTION__);
}

void VkRenderer::setSize(unsigned int width, unsigned int height)
{
    mRenderData.rdWidth = static_cast<int>(width);
    mRenderData.rdHeight = static_cast<int>(height);
    /* Vulkan detects changes and recreates swapchain */
    Logger::info("%s: resized window to %ix%i\n", __FUNCTION__, width, height);
}

bool VkRenderer::uploadBasicMesh(const BasicMesh &vertexData)
{
    /* needs to be split in buffer alloc and upload */

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = vertexData.vertices.size() * sizeof(MeshVertex);
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    VmaAllocationCreateInfo vmaAllocInfo{};
    vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    if (vmaCreateBuffer(mRenderData.rdAllocator, &bufferInfo, &vmaAllocInfo, &mVertexBuffer, &mVertexBufferAlloc,
                        nullptr) != VK_SUCCESS)
    {
        Logger::info("%s error: could not allocate vertex buffer via VMA\n", __FUNCTION__);
        return false;
    }

    void *data;
    vmaMapMemory(mRenderData.rdAllocator, mVertexBufferAlloc, &data);
    std::memcpy(data, vertexData.vertices.data(), vertexData.vertices.size() * sizeof(MeshVertex));
    vmaUnmapMemory(mRenderData.rdAllocator, mVertexBufferAlloc);

    mRenderData.mTriangleCount = vertexData.vertices.size() / 3;

    return true;
}

bool VkRenderer::uploadVMesh(const VMesh &vMesh)
{
    /* needs to be split in buffer alloc and upload */

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = vMesh.vertices.size() * sizeof(MeshVertex);
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    VmaAllocationCreateInfo vmaAllocInfo{};
    vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    if (vmaCreateBuffer(mRenderData.rdAllocator, &bufferInfo, &vmaAllocInfo, &mVertexBuffer, &mVertexBufferAlloc,
                        nullptr) != VK_SUCCESS)
    {
        Logger::info("%s error: could not allocate vertex buffer via VMA\n", __FUNCTION__);
        return false;
    }

    void *data;
    vmaMapMemory(mRenderData.rdAllocator, mVertexBufferAlloc, &data);
    std::memcpy(data, vMesh.vertices.data(), vMesh.vertices.size() * sizeof(MeshVertex));
    vmaUnmapMemory(mRenderData.rdAllocator, mVertexBufferAlloc);

    mRenderData.mTriangleCount = vMesh.vertices.size() / 3;

    return true;
}

static void copyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue transferQueue,
                       VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    // Allocate a one-time command buffer.
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate command buffer for copy operation!");
    }

    // Begin recording the command buffer.
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    // Specify the region to copy.
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    // End recording.
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to record command buffer!");
    }

    // Submit the command buffer and wait for it to finish.
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    if (vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit copy command buffer!");
    }
    vkQueueWaitIdle(transferQueue);

    // Clean up the command buffer.
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

bool VkRenderer::uploadIndexBuffer(const VMesh &vMesh)
{
    // --- Create a staging buffer using VMA ---
    VkBuffer stagingBuffer;
    VmaAllocation stagingAllocation;

    VkBufferCreateInfo stagingBufferInfo = {};
    stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferInfo.size = vMesh.indices.size();
    stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VmaAllocationCreateInfo stagingAllocInfo = {};
    stagingAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

    if (vmaCreateBuffer(mRenderData.rdAllocator, &stagingBufferInfo, &stagingAllocInfo,
                        &stagingBuffer, &stagingAllocation, nullptr) != VK_SUCCESS)
    {
        Logger::error("Failed to create staging buffer!\n", __FUNCTION__);
        return false;
    }

// Map and copy index data to the staging buffer
    void *stagingData = nullptr;
    vmaMapMemory(mRenderData.rdAllocator, stagingAllocation, &stagingData);
    memcpy(stagingData, vMesh.indices.data(), vMesh.indices.size());
    vmaUnmapMemory(mRenderData.rdAllocator, stagingAllocation);

// --- Create the GPU-local index buffer using VMA ---
    VkBufferCreateInfo indexBufferInfo = {};
    indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    indexBufferInfo.size = vMesh.indices.size();
    indexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

    VmaAllocationCreateInfo indexAllocInfo = {};
    indexAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    if (vmaCreateBuffer(mRenderData.rdAllocator, &indexBufferInfo, &indexAllocInfo,
                        &mIndexBuffer, &mIndexBufferAlloc, nullptr) != VK_SUCCESS)
    {
        Logger::error("Failed to create GPU index buffer!\n", __FUNCTION__);
        return false;
    }

// --- Copy data from staging buffer to GPU index buffer ---
// This function should record and submit a command buffer that copies from one buffer to the other.
//    copyBuffer(stagingBuffer, vkIndexBuffer, vMesh.indices.size());
    copyBuffer(mRenderData.rdVkbDevice, mRenderData.rdCommandPool, mRenderData.rdGraphicsQueue, stagingBuffer,
               mIndexBuffer, vMesh.indices.size());

// Clean up the staging buffer
    vmaDestroyBuffer(mRenderData.rdAllocator, stagingBuffer, stagingAllocation);
    mRenderData.mIndexType = vMesh.indexType;
    mRenderData.mIndexCount = vMesh.indices.size() /
                              (mRenderData.mIndexType == VK_INDEX_TYPE_UINT16 ? sizeof(uint16_t)
                                                                              : sizeof(uint32_t));
    return true;
}

bool VkRenderer::draw()
{
    if (vkWaitForFences(mRenderData.rdVkbDevice.device, 1, &mRenderData.rdRenderFence, VK_TRUE, UINT64_MAX)
        !=
        VK_SUCCESS)
    {
        Logger::error("%s: error: waiting for fences failed\n", __FUNCTION__);
        return false;
    }

    if (vkResetFences(mRenderData.rdVkbDevice.device, 1, &mRenderData.rdRenderFence)
        !=
        VK_SUCCESS)
    {
        Logger::error("%s: error: fence reset failed\n", __FUNCTION__);
        return false;
    }

    uint32_t imageIndex = 0;
    VkResult result = vkAcquireNextImageKHR(mRenderData.rdVkbDevice.device,
                                            mRenderData.rdVkbSwapchain.swapchain,
                                            UINT64_MAX,
                                            mRenderData.rdPresentSemaphore,
                                            VK_NULL_HANDLE,
                                            &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return recreateSwapchain();
    } else
    {
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            Logger::error("%s: error: failed to acquire swapchain image. Error is '%i'\n", __FUNCTION__, result);
            return false;
        }
    }

    if (vkResetCommandBuffer(mRenderData.rdCommandBuffer, 0) != VK_SUCCESS)
    {
        Logger::error("%s: error: failed to reset command buffer\n", __FUNCTION__);
        return false;
    }

    VkCommandBufferBeginInfo cmdBeginInfo{};
    cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(mRenderData.rdCommandBuffer, &cmdBeginInfo) != VK_SUCCESS)
    {
        Logger::error("%s: error: failed to begin command buffer\n", __FUNCTION__);
        return false;
    }

    VkClearValue colorClearValue;
    colorClearValue.color = {{0.1f, 0.1f, 0.1f, 0.1f}};

    VkClearValue depthValue;
    depthValue.depthStencil.depth = 1.0f;

    VkClearValue clearValues[] = {colorClearValue, depthValue};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = mRenderData.rdRenderpass;

    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent = mRenderData.rdVkbSwapchain.extent;
    renderPassBeginInfo.framebuffer = mRenderData.rdFramebuffers[imageIndex];

    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;

    // Set projection matrix
    const auto currentTime = static_cast<float>(glfwGetTime());

    mMatrices.projectionMatrix = glm::perspective(
            glm::radians(90.0f),
            static_cast<float>(mRenderData.rdVkbSwapchain.extent.width) /
            static_cast<float>(mRenderData.rdVkbSwapchain.extent.height),
            0.1f,
            10.0f
    );

    glm::mat4 model = glm::mat4(1.0f);

    if (mRenderData.mUseChangedShader)
    {
        model = glm::rotate(glm::mat4(1.0f), currentTime, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    // Set view matrix
    Camera &camera = mRenderData.mCamera;
    camera.ApplyDesiredTransform(mRenderData.deltaTime);
    glm::mat4 viewMatrix = glm::mat4_cast(glm::conjugate(camera.GetOrientation())) * model;
    mMatrices.viewMatrix = glm::translate(viewMatrix, -camera.worldPosition);

    // Begin render pass
    vkCmdBeginRenderPass(mRenderData.rdCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // The rendering pipeline happens here
    if (mRenderData.mUseChangedShader)
    {
        vkCmdBindPipeline(mRenderData.rdCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mRenderData.rdPipelineChanged);
    } else
    {
        vkCmdBindPipeline(mRenderData.rdCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mRenderData.rdPipeline);
    }

    // Required for dynamic viewport
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(mRenderData.rdVkbSwapchain.extent.width);
    viewport.height = static_cast<float>(mRenderData.rdVkbSwapchain.extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(mRenderData.rdCommandBuffer, 0, 1, &viewport);

    VkRect2D scissors{};
    scissors.offset = {0, 0};
    scissors.extent = mRenderData.rdVkbSwapchain.extent;
    vkCmdSetScissor(mRenderData.rdCommandBuffer, 0, 1, &scissors);

    // The triangle drawing itself
    vkCmdBindDescriptorSets(
            mRenderData.rdCommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            mRenderData.rdPipelineLayout,
            0,
            1,
            &mRenderData.rdDescriptorSet,
            0,
            nullptr
    );
    vkCmdBindDescriptorSets(
            mRenderData.rdCommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            mRenderData.rdPipelineLayout,
            1,
            1,
            &mRenderData.rdUBODescriptorSet,
            0,
            nullptr
    );

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(mRenderData.rdCommandBuffer, 0, 1, &mVertexBuffer, &offset);
    // Draw using indices now
    // vkCmdDraw(mRenderData.rdCommandBuffer, mRenderData.mTriangleCount * 3, 1, 0, 0);
    vkCmdBindIndexBuffer(mRenderData.rdCommandBuffer, mIndexBuffer, 0, mRenderData.mIndexType);
    vkCmdDrawIndexed(mRenderData.rdCommandBuffer, mRenderData.mIndexCount,
                     1, 0, 0, 0);

    // imgui overlay
    mUserInterface.createFrame(mRenderData);
    mUserInterface.render(mRenderData);

    vkCmdEndRenderPass(mRenderData.rdCommandBuffer);

    if (vkEndCommandBuffer(mRenderData.rdCommandBuffer) != VK_SUCCESS)
    {
        Logger::error("$s: error: failed to end command buffer\n", __FUNCTION__);
        return false;
    }

    UniformBuffer::uploadData(mRenderData, mMatrices);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submitInfo.pWaitDstStageMask = &waitStage;

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &mRenderData.rdPresentSemaphore;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &mRenderData.rdRenderSemaphore;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mRenderData.rdCommandBuffer;

    if (vkQueueSubmit(mRenderData.rdGraphicsQueue, 1, &submitInfo, mRenderData.rdRenderFence) != VK_SUCCESS)
    {
        Logger::error("%s: error: failed to submit draw command buffer\n", __FUNCTION__);
        return false;
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &mRenderData.rdRenderSemaphore;

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &mRenderData.rdVkbSwapchain.swapchain;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(mRenderData.rdPresentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        return recreateSwapchain();
    } else
    {
        if (result != VK_SUCCESS)
        {
            Logger::error("%s: error: failed to present swapchain image\n", __FUNCTION__);
            return false;
        }
    }

    mRenderData.deltaTime = currentTime - mRenderData.lastTickTime;
    mRenderData.lastTickTime = currentTime;

    return true;
}

void VkRenderer::handleKeyEvents(int key, int scancode, int action, int mods)
{
    // Change shaders

    if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        mRenderData.mUseChangedShader = !mRenderData.mUseChangedShader;
    }


    // Camera position

    mRenderData.mCamera.desiredMoveDirection = glm::vec3();
    if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
        glm::vec3 worldRightVector(1.f, 0.f, 0.f);
        glm::vec3 cameraRightVector = mRenderData.mCamera.GetOrientation() * worldRightVector;
        mRenderData.mCamera.desiredMoveDirection = cameraRightVector;
    }
    if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
        glm::vec3 worldRightVector(1.f, 0.f, 0.f);
        glm::vec3 cameraRightVector = mRenderData.mCamera.GetOrientation() * worldRightVector;
        mRenderData.mCamera.desiredMoveDirection = -cameraRightVector;
    }
    if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_W) == GLFW_PRESS)
    {
        glm::vec3 worldForwardVector(0.f, 0.f, -1.f);
        glm::vec3 cameraForwardVector = mRenderData.mCamera.GetOrientation() * worldForwardVector;
        mRenderData.mCamera.desiredMoveDirection = cameraForwardVector;
    }
    if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
        glm::vec3 worldForwardVector(0.f, 0.f, -1.f);
        glm::vec3 cameraForwardVector = mRenderData.mCamera.GetOrientation() * worldForwardVector;
        mRenderData.mCamera.desiredMoveDirection = -cameraForwardVector;
    }
    if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
    {
        mRenderData.mCamera.desiredMoveDirection.y += 1.f;
    }
    if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
    {
        mRenderData.mCamera.desiredMoveDirection.y -= 1.f;
    }


    // Camera Orientation

    mRenderData.mCamera.desiredRotationAngle = glm::vec2();
    if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        mRenderData.mCamera.desiredRotationAngle[0] = -1.f;
    }
    if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        mRenderData.mCamera.desiredRotationAngle[0] = 1.f;
    }
    if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mRenderData.mCamera.desiredRotationAngle[1] = -1.f;
    }
    if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mRenderData.mCamera.desiredRotationAngle[1] = 1.f;
    }
}

void VkRenderer::handleMouseButtonEvents(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        mRenderData.mCamera.mouseLock = true;
        glfwSetInputMode(mRenderData.rdWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetInputMode(mRenderData.rdWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        mRenderData.mCamera.mouseLock = false;
        glfwSetInputMode(mRenderData.rdWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        mRenderData.mMouse.lastPos.reset();
    }
}

void VkRenderer::handleMousePositionEvents(double x, double y)
{
    if (mRenderData.mCamera.mouseLock)
    {
        if (mRenderData.mMouse.lastPos.has_value())
        {
            glm::vec2 currentMousePos(x, y);
            glm::vec2 deltaPos = currentMousePos - mRenderData.mMouse.lastPos.value();
            mRenderData.mMouse.lastPos = currentMousePos;

            Camera &camera = mRenderData.mCamera;
            camera.yawPitchAngles[0] -= deltaPos[0] / 10.f;
            camera.yawPitchAngles[1] += deltaPos[1] / 10.f;
        } else
        {
            mRenderData.mMouse.lastPos = glm::vec2(x, y);
        }
    }
}
