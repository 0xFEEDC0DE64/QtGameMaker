#pragma once

#include <vector>

#include <QImage>
#include <QVulkanWindow>

struct GameScene;

class VulkanGameRenderer : public QVulkanWindowRenderer
{
public:
    VulkanGameRenderer(const GameScene &scene, QVulkanWindow *window);

    void initResources() override;
    void initSwapChainResources() override;
    void releaseSwapChainResources() override;
    void releaseResources() override;
    void startNextFrame() override;

private:
    struct Texture
    {
        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkImage stagingImage = VK_NULL_HANDLE;
        VkDeviceMemory stagingMemory = VK_NULL_HANDLE;
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        QSize size;
        bool layoutPending{};
        bool stagingPending{};
    };

    VkShaderModule createShader(const QString &name);
    bool createTexture(const QImage &image, Texture &texture);
    bool createTextureImage(const QSize &size, VkImageTiling tiling,
                            VkImageUsageFlags usage, VkImage *image, VkDeviceMemory *memory);
    bool writeLinearImage(const QImage &image, VkImage target, VkDeviceMemory memory);
    void ensureTextures();

    const GameScene &m_scene;
    QVulkanWindow *m_window;
    QVulkanDeviceFunctions *m_devFuncs{};

    VkBuffer m_vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_vertexMemory = VK_NULL_HANDLE;
    VkDescriptorPool m_descPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descSetLayout = VK_NULL_HANDLE;
    VkPipelineCache m_pipelineCache = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;
    VkFormat m_texFormat = VK_FORMAT_R8G8B8A8_UNORM;
    std::vector<Texture> m_textures;
    QMatrix4x4 m_projection;
};
