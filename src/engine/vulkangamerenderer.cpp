#include "vulkangamerenderer.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>

#include <QFile>
#include <QVulkanFunctions>

#include "gamescene.h"

namespace {

struct Vertex
{
    float x, y, z, u, v;
};

constexpr std::array<Vertex, 4> quad {{
    {0.f, 0.f, 0.f, 0.f, 0.f},
    {0.f, 1.f, 0.f, 0.f, 1.f},
    {1.f, 0.f, 0.f, 1.f, 0.f},
    {1.f, 1.f, 0.f, 1.f, 1.f}
}};

uint32_t memoryIndex(QVulkanWindow *window, uint32_t allowed, VkMemoryPropertyFlags required)
{
    VkPhysicalDeviceMemoryProperties properties;
    window->vulkanInstance()->functions()->vkGetPhysicalDeviceMemoryProperties(
        window->physicalDevice(), &properties);
    for (uint32_t i = 0; i < properties.memoryTypeCount; ++i)
        if ((allowed & (1u << i)) &&
            (properties.memoryTypes[i].propertyFlags & required) == required)
            return i;
    return std::numeric_limits<uint32_t>::max();
}

}

VulkanGameRenderer::VulkanGameRenderer(const GameScene &scene, QVulkanWindow *window) :
    m_scene{scene},
    m_window{window}
{
}

VkShaderModule VulkanGameRenderer::createShader(const QString &name)
{
    QFile file{name};
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning("Failed to read Vulkan shader %s", qPrintable(name));
        return VK_NULL_HANDLE;
    }
    const QByteArray blob = file.readAll();
    VkShaderModuleCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.codeSize = static_cast<std::size_t>(blob.size());
    info.pCode = reinterpret_cast<const uint32_t *>(blob.constData());

    VkShaderModule shader = VK_NULL_HANDLE;
    const VkResult result = m_devFuncs->vkCreateShaderModule(m_window->device(), &info, nullptr, &shader);
    if (result != VK_SUCCESS)
        qWarning("Failed to create Vulkan shader module: %d", result);
    return shader;
}

bool VulkanGameRenderer::createTextureImage(const QSize &size, VkImageTiling tiling,
                                             VkImageUsageFlags usage, VkImage *image, VkDeviceMemory *memory)
{
    const VkDevice device = m_window->device();
    VkImageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = m_texFormat;
    info.extent = {uint32_t(size.width()), uint32_t(size.height()), 1};
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = tiling;
    info.usage = usage;
    info.initialLayout = tiling == VK_IMAGE_TILING_LINEAR ?
                             VK_IMAGE_LAYOUT_PREINITIALIZED : VK_IMAGE_LAYOUT_UNDEFINED;
    if (m_devFuncs->vkCreateImage(device, &info, nullptr, image) != VK_SUCCESS)
        return false;

    VkMemoryRequirements requirements;
    m_devFuncs->vkGetImageMemoryRequirements(device, *image, &requirements);
    const VkMemoryPropertyFlags flags = tiling == VK_IMAGE_TILING_LINEAR ?
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT :
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    const uint32_t index = memoryIndex(m_window, requirements.memoryTypeBits, flags);
    if (index == std::numeric_limits<uint32_t>::max())
        return false;

    VkMemoryAllocateInfo allocation{};
    allocation.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocation.allocationSize = requirements.size;
    allocation.memoryTypeIndex = index;
    if (m_devFuncs->vkAllocateMemory(device, &allocation, nullptr, memory) != VK_SUCCESS)
        return false;
    return m_devFuncs->vkBindImageMemory(device, *image, *memory, 0) == VK_SUCCESS;
}

bool VulkanGameRenderer::writeLinearImage(const QImage &image, VkImage target, VkDeviceMemory memory)
{
    const VkDevice device = m_window->device();
    VkImageSubresource subresource{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0};
    VkSubresourceLayout layout;
    m_devFuncs->vkGetImageSubresourceLayout(device, target, &subresource, &layout);

    uchar *mapped = nullptr;
    if (m_devFuncs->vkMapMemory(device, memory, layout.offset, layout.size, 0,
                                reinterpret_cast<void **>(&mapped)) != VK_SUCCESS)
        return false;
    for (int y = 0; y < image.height(); ++y)
        std::memcpy(mapped + y * layout.rowPitch, image.constScanLine(y),
                    std::size_t(image.width()) * 4);
    m_devFuncs->vkUnmapMemory(device, memory);
    return true;
}

bool VulkanGameRenderer::createTexture(const QImage &source, Texture &texture)
{
    const QImage image = source.convertToFormat(QImage::Format_RGBA8888_Premultiplied);
    texture.size = image.size();

    VkFormatProperties properties;
    m_window->vulkanInstance()->functions()->vkGetPhysicalDeviceFormatProperties(
        m_window->physicalDevice(), m_texFormat, &properties);

    if ((properties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) &&
        !qEnvironmentVariableIntValue("QT_VK_FORCE_STAGE_TEX"))
    {
        if (!createTextureImage(image.size(), VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_SAMPLED_BIT,
                                &texture.image, &texture.memory) ||
            !writeLinearImage(image, texture.image, texture.memory))
            return false;
        texture.layoutPending = true;
    }
    else if (properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)
    {
        if (!createTextureImage(image.size(), VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                &texture.stagingImage, &texture.stagingMemory) ||
            !createTextureImage(image.size(), VK_IMAGE_TILING_OPTIMAL,
                                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                &texture.image, &texture.memory) ||
            !writeLinearImage(image, texture.stagingImage, texture.stagingMemory))
            return false;
        texture.stagingPending = true;
    }
    else
        return false;

    VkImageViewCreateInfo view{};
    view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view.image = texture.image;
    view.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view.format = m_texFormat;
    view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view.subresourceRange.levelCount = 1;
    view.subresourceRange.layerCount = 1;
    return m_devFuncs->vkCreateImageView(m_window->device(), &view, nullptr, &texture.view) == VK_SUCCESS;
}

void VulkanGameRenderer::ensureTextures()
{
    const VkCommandBuffer command = m_window->currentCommandBuffer();
    auto transition = [this, command](VkImage image, VkImageLayout oldLayout,
                                       VkImageLayout newLayout, VkAccessFlags sourceAccess,
                                       VkAccessFlags targetAccess, VkPipelineStageFlags sourceStage,
                                       VkPipelineStageFlags targetStage) {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.srcAccessMask = sourceAccess;
        barrier.dstAccessMask = targetAccess;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.layerCount = 1;
        m_devFuncs->vkCmdPipelineBarrier(command, sourceStage, targetStage, 0,
                                         0, nullptr, 0, nullptr, 1, &barrier);
    };

    for (Texture &texture : m_textures)
    {
        if (texture.layoutPending)
        {
            transition(texture.image, VK_IMAGE_LAYOUT_PREINITIALIZED,
                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                       VK_ACCESS_HOST_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
                       VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            texture.layoutPending = false;
        }
        if (texture.stagingPending)
        {
            transition(texture.stagingImage, VK_IMAGE_LAYOUT_PREINITIALIZED,
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       VK_ACCESS_HOST_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT,
                       VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
            transition(texture.image, VK_IMAGE_LAYOUT_UNDEFINED,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       0, VK_ACCESS_TRANSFER_WRITE_BIT,
                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

            VkImageCopy copy{};
            copy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copy.srcSubresource.layerCount = 1;
            copy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copy.dstSubresource.layerCount = 1;
            copy.extent = {uint32_t(texture.size.width()), uint32_t(texture.size.height()), 1};
            m_devFuncs->vkCmdCopyImage(command, texture.stagingImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                       texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
            transition(texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                       VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
                       VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            texture.stagingPending = false;
        }
    }
}

void VulkanGameRenderer::initResources()
{
    const VkDevice device = m_window->device();
    m_devFuncs = m_window->vulkanInstance()->deviceFunctions(device);

    VkBufferCreateInfo buffer{};
    buffer.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer.size = sizeof(quad);
    buffer.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if (m_devFuncs->vkCreateBuffer(device, &buffer, nullptr, &m_vertexBuffer) != VK_SUCCESS)
        qFatal("Failed to create Vulkan vertex buffer");

    VkMemoryRequirements requirements;
    m_devFuncs->vkGetBufferMemoryRequirements(device, m_vertexBuffer, &requirements);
    const uint32_t index = memoryIndex(m_window, requirements.memoryTypeBits,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (index == std::numeric_limits<uint32_t>::max())
        qFatal("No host-visible Vulkan vertex buffer memory");

    VkMemoryAllocateInfo allocation{};
    allocation.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocation.allocationSize = requirements.size;
    allocation.memoryTypeIndex = index;
    if (m_devFuncs->vkAllocateMemory(device, &allocation, nullptr, &m_vertexMemory) != VK_SUCCESS ||
        m_devFuncs->vkBindBufferMemory(device, m_vertexBuffer, m_vertexMemory, 0) != VK_SUCCESS)
        qFatal("Failed to allocate Vulkan vertex buffer");

    void *mapped = nullptr;
    if (m_devFuncs->vkMapMemory(device, m_vertexMemory, 0, sizeof(quad), 0, &mapped) != VK_SUCCESS)
        qFatal("Failed to map Vulkan vertex buffer");
    std::memcpy(mapped, quad.data(), sizeof(quad));
    m_devFuncs->vkUnmapMemory(device, m_vertexMemory);

    VkSamplerCreateInfo sampler{};
    sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler.magFilter = VK_FILTER_NEAREST;
    sampler.minFilter = VK_FILTER_NEAREST;
    sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler.maxAnisotropy = 1.f;
    if (m_devFuncs->vkCreateSampler(device, &sampler, nullptr, &m_sampler) != VK_SUCCESS)
        qFatal("Failed to create Vulkan sprite sampler");

    m_textures.resize(m_scene.textures.size());
    for (std::size_t i = 0; i < m_textures.size(); ++i)
        if (!createTexture(m_scene.textures[i], m_textures[i]))
            qFatal("Failed to upload Vulkan sprite texture %zu", i);

    const uint32_t textureCount = uint32_t(std::max<std::size_t>(1, m_textures.size()));
    VkDescriptorPoolSize poolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, textureCount};
    VkDescriptorPoolCreateInfo pool{};
    pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool.maxSets = textureCount;
    pool.poolSizeCount = 1;
    pool.pPoolSizes = &poolSize;
    if (m_devFuncs->vkCreateDescriptorPool(device, &pool, nullptr, &m_descPool) != VK_SUCCESS)
        qFatal("Failed to create Vulkan descriptor pool");

    VkDescriptorSetLayoutBinding binding{};
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    VkDescriptorSetLayoutCreateInfo setLayout{};
    setLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    setLayout.bindingCount = 1;
    setLayout.pBindings = &binding;
    if (m_devFuncs->vkCreateDescriptorSetLayout(device, &setLayout, nullptr, &m_descSetLayout) != VK_SUCCESS)
        qFatal("Failed to create Vulkan descriptor set layout");

    for (Texture &texture : m_textures)
    {
        VkDescriptorSetAllocateInfo set{};
        set.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        set.descriptorPool = m_descPool;
        set.descriptorSetCount = 1;
        set.pSetLayouts = &m_descSetLayout;
        if (m_devFuncs->vkAllocateDescriptorSets(device, &set, &texture.descriptorSet) != VK_SUCCESS)
            qFatal("Failed to allocate Vulkan texture descriptor");

        VkDescriptorImageInfo imageInfo{m_sampler, texture.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = texture.descriptorSet;
        write.dstBinding = 0;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.pImageInfo = &imageInfo;
        m_devFuncs->vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
    }

    VkPipelineCacheCreateInfo cache{};
    cache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    if (m_devFuncs->vkCreatePipelineCache(device, &cache, nullptr, &m_pipelineCache) != VK_SUCCESS)
        qFatal("Failed to create Vulkan pipeline cache");

    VkPushConstantRange push{};
    push.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push.offset = 0;
    push.size = 16 * sizeof(float);
    VkPipelineLayoutCreateInfo pipelineLayout{};
    pipelineLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayout.setLayoutCount = 1;
    pipelineLayout.pSetLayouts = &m_descSetLayout;
    pipelineLayout.pushConstantRangeCount = 1;
    pipelineLayout.pPushConstantRanges = &push;
    if (m_devFuncs->vkCreatePipelineLayout(device, &pipelineLayout, nullptr, &m_pipelineLayout) != VK_SUCCESS)
        qFatal("Failed to create Vulkan pipeline layout");

    const VkShaderModule vertexShader = createShader(QStringLiteral(":/qtgameengine/vulkan_shader_modules/texture_vert.spv"));
    const VkShaderModule fragmentShader = createShader(QStringLiteral(":/qtgameengine/vulkan_shader_modules/texture_frag.spv"));
    if (!vertexShader || !fragmentShader)
        qFatal("Failed to load Vulkan sprite shaders");

    VkPipelineShaderStageCreateInfo stages[2]{};
    stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = vertexShader;
    stages[0].pName = "main";
    stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].module = fragmentShader;
    stages[1].pName = "main";

    VkVertexInputBindingDescription vertexBinding{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX};
    VkVertexInputAttributeDescription attributes[2]{
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0},
        {1, 0, VK_FORMAT_R32G32_SFLOAT, 3 * sizeof(float)}
    };
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInput.vertexBindingDescriptionCount = 1;
    vertexInput.pVertexBindingDescriptions = &vertexBinding;
    vertexInput.vertexAttributeDescriptionCount = 2;
    vertexInput.pVertexAttributeDescriptions = attributes;

    VkPipelineInputAssemblyStateCreateInfo assembly{};
    assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

    VkPipelineViewportStateCreateInfo viewport{};
    viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport.viewportCount = 1;
    viewport.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo raster{};
    raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    raster.polygonMode = VK_POLYGON_MODE_FILL;
    raster.cullMode = VK_CULL_MODE_NONE;
    raster.frontFace = VK_FRONT_FACE_CLOCKWISE;
    raster.lineWidth = 1.f;

    VkPipelineMultisampleStateCreateInfo multisample{};
    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depth{};
    depth.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth.depthTestEnable = VK_FALSE;
    depth.depthWriteEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState attachment{};
    attachment.blendEnable = VK_TRUE;
    attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    attachment.colorBlendOp = VK_BLEND_OP_ADD;
    attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    attachment.alphaBlendOp = VK_BLEND_OP_ADD;
    attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    VkPipelineColorBlendStateCreateInfo blend{};
    blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blend.attachmentCount = 1;
    blend.pAttachments = &attachment;

    VkDynamicState dynamicStates[]{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamic{};
    dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic.dynamicStateCount = 2;
    dynamic.pDynamicStates = dynamicStates;

    VkGraphicsPipelineCreateInfo pipeline{};
    pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline.stageCount = 2;
    pipeline.pStages = stages;
    pipeline.pVertexInputState = &vertexInput;
    pipeline.pInputAssemblyState = &assembly;
    pipeline.pViewportState = &viewport;
    pipeline.pRasterizationState = &raster;
    pipeline.pMultisampleState = &multisample;
    pipeline.pDepthStencilState = &depth;
    pipeline.pColorBlendState = &blend;
    pipeline.pDynamicState = &dynamic;
    pipeline.layout = m_pipelineLayout;
    pipeline.renderPass = m_window->defaultRenderPass();
    if (m_devFuncs->vkCreateGraphicsPipelines(device, m_pipelineCache, 1, &pipeline, nullptr, &m_pipeline) != VK_SUCCESS)
        qFatal("Failed to create Vulkan sprite pipeline");

    m_devFuncs->vkDestroyShaderModule(device, vertexShader, nullptr);
    m_devFuncs->vkDestroyShaderModule(device, fragmentShader, nullptr);
}

void VulkanGameRenderer::initSwapChainResources()
{
    m_projection = m_window->clipCorrectionMatrix();
    m_projection.ortho(0.f, float(m_scene.roomSize.width()),
                       float(m_scene.roomSize.height()), 0.f, -1.f, 1.f);
}

void VulkanGameRenderer::releaseSwapChainResources()
{
}

void VulkanGameRenderer::releaseResources()
{
    if (!m_devFuncs)
        return;
    const VkDevice device = m_window->device();

    if (m_pipeline)
        m_devFuncs->vkDestroyPipeline(device, m_pipeline, nullptr);
    if (m_pipelineLayout)
        m_devFuncs->vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
    if (m_pipelineCache)
        m_devFuncs->vkDestroyPipelineCache(device, m_pipelineCache, nullptr);
    if (m_descPool)
        m_devFuncs->vkDestroyDescriptorPool(device, m_descPool, nullptr);
    if (m_descSetLayout)
        m_devFuncs->vkDestroyDescriptorSetLayout(device, m_descSetLayout, nullptr);
    if (m_sampler)
        m_devFuncs->vkDestroySampler(device, m_sampler, nullptr);
    m_pipeline = VK_NULL_HANDLE;
    m_pipelineLayout = VK_NULL_HANDLE;
    m_pipelineCache = VK_NULL_HANDLE;
    m_descPool = VK_NULL_HANDLE;
    m_descSetLayout = VK_NULL_HANDLE;
    m_sampler = VK_NULL_HANDLE;

    for (Texture &texture : m_textures)
    {
        if (texture.view)
            m_devFuncs->vkDestroyImageView(device, texture.view, nullptr);
        if (texture.image)
            m_devFuncs->vkDestroyImage(device, texture.image, nullptr);
        if (texture.memory)
            m_devFuncs->vkFreeMemory(device, texture.memory, nullptr);
        if (texture.stagingImage)
            m_devFuncs->vkDestroyImage(device, texture.stagingImage, nullptr);
        if (texture.stagingMemory)
            m_devFuncs->vkFreeMemory(device, texture.stagingMemory, nullptr);
    }
    m_textures.clear();

    if (m_vertexBuffer)
        m_devFuncs->vkDestroyBuffer(device, m_vertexBuffer, nullptr);
    if (m_vertexMemory)
        m_devFuncs->vkFreeMemory(device, m_vertexMemory, nullptr);
    m_vertexBuffer = VK_NULL_HANDLE;
    m_vertexMemory = VK_NULL_HANDLE;
}

void VulkanGameRenderer::startNextFrame()
{
    const VkCommandBuffer command = m_window->currentCommandBuffer();
    const QSize size = m_window->swapChainImageSize();
    ensureTextures();

    VkClearValue clearValues[2]{};
    clearValues[0].color.float32[0] = 1.f;
    clearValues[0].color.float32[1] = 1.f;
    clearValues[0].color.float32[2] = 1.f;
    clearValues[0].color.float32[3] = 1.f;
    clearValues[1].depthStencil = {1.f, 0};

    VkRenderPassBeginInfo begin{};
    begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    begin.renderPass = m_window->defaultRenderPass();
    begin.framebuffer = m_window->currentFramebuffer();
    begin.renderArea.extent = {uint32_t(size.width()), uint32_t(size.height())};
    begin.clearValueCount = 2;
    begin.pClearValues = clearValues;
    m_devFuncs->vkCmdBeginRenderPass(command, &begin, VK_SUBPASS_CONTENTS_INLINE);

    m_devFuncs->vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    const VkDeviceSize vertexOffset = 0;
    m_devFuncs->vkCmdBindVertexBuffers(command, 0, 1, &m_vertexBuffer, &vertexOffset);

    VkViewport viewport{};
    viewport.width = float(size.width());
    viewport.height = float(size.height());
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;
    m_devFuncs->vkCmdSetViewport(command, 0, 1, &viewport);
    VkRect2D scissor{};
    scissor.extent = {uint32_t(size.width()), uint32_t(size.height())};
    m_devFuncs->vkCmdSetScissor(command, 0, 1, &scissor);

    for (const GameScene::Instance *instance : m_scene.drawOrder())
    {
        if (!instance->visible || !instance->spriteIndex)
            continue;
        const std::size_t textureIndex = m_scene.textureFor(*instance);
        const Texture &texture = m_textures.at(textureIndex);
        const QPoint &origin = m_scene.sprites.at(*instance->spriteIndex).origin;

        QMatrix4x4 matrix = m_projection;
        matrix.translate(float(instance->position.x() - origin.x()),
                         float(instance->position.y() - origin.y()));
        matrix.scale(float(texture.size.width()), float(texture.size.height()));
        m_devFuncs->vkCmdPushConstants(command, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT,
                                       0, 16 * sizeof(float), matrix.constData());
        m_devFuncs->vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                            m_pipelineLayout, 0, 1, &texture.descriptorSet, 0, nullptr);
        m_devFuncs->vkCmdDraw(command, 4, 1, 0, 0);
    }

    m_devFuncs->vkCmdEndRenderPass(command);
    m_window->frameReady();
}
