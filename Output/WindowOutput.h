//
// Created by kevin on 25/09/20.
//

#pragma once

#include "RenderOutput.h"
#include "../Utils/Structs.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

class WindowOutput {
public:
    //Init Window and Vulkan instance
    WindowOutput(const int width, const int height, const std::string title, const Buffer<Color>& buffer) {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }

        VkApplicationInfo appinfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
        appinfo.pApplicationName = "Raytracer";
        appinfo.pEngineName      = "Vulkan image presenter";
        appinfo.apiVersion       = VK_MAKE_VERSION(1, 0, 0);

        VkInstanceCreateInfo instanceinfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
        instanceinfo.pApplicationInfo        = &appinfo;
        instanceinfo.enabledExtensionCount   = 0;
        uint32_t glfwExtensioncount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensioncount);
        instanceinfo.enabledExtensionCount = glfwExtensioncount;
        instanceinfo.ppEnabledExtensionNames = glfwExtensions;
        instanceinfo.enabledLayerCount       = static_cast<uint32_t>(validation_layers.size());
        instanceinfo.ppEnabledLayerNames     = validation_layers.data();

        if(vkCreateInstance(&instanceinfo, nullptr, &instance) != VK_SUCCESS){
            throw std::runtime_error("Failed to create instance");
        }

        //Pick the physical device
        uint32_t pdevicecount = 0;
        vkEnumeratePhysicalDevices(instance, &pdevicecount, nullptr);
        if(pdevicecount == 0){
            throw std::runtime_error("No physical device found");
        }

        std::vector<VkPhysicalDevice> pdevices(pdevicecount);
        vkEnumeratePhysicalDevices(instance, &pdevicecount, pdevices.data());

        {
            std::cout << "Found devices \n";
            for(auto dev: pdevices){
                VkPhysicalDeviceProperties dprops;
                vkGetPhysicalDeviceProperties(dev, &dprops);
                std::cout << "\t " << dprops.deviceName << "\n";
            }
        }
        pdevice = pdevices[0];

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(pdevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueprops;
        vkGetPhysicalDeviceQueueFamilyProperties(pdevice, &queueFamilyCount, queueprops.data());

        uint32_t graphicsindex = 1;
        {
            int i = 0;
            std::cout << "Queues \n";
            for (auto props : queueprops) {
                std::cout << "\t" << props.queueFlags << "\n";
                if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    graphicsindex = i;
                }
                ++i;
            }
        }

        std::cout << "Graphic queue: " << graphicsindex << "\n";

        //Logical device
        VkDeviceQueueCreateInfo queueinfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        queueinfo.queueCount = 1;
        queueinfo.queueFamilyIndex = graphicsindex;
        float qpriorities = {1.0f};
        queueinfo.pQueuePriorities = &qpriorities;

        VkPhysicalDeviceFeatures devfeatures{};
        VkDeviceCreateInfo deviceinfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
        deviceinfo.queueCreateInfoCount = 1;
        deviceinfo.pQueueCreateInfos = &queueinfo;
        deviceinfo.pEnabledFeatures = &devfeatures;
        deviceinfo.enabledExtensionCount = static_cast<uint32_t>(device_layers.size());
        deviceinfo.ppEnabledExtensionNames = device_layers.data();
        deviceinfo.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        deviceinfo.ppEnabledLayerNames= validation_layers.data();

        if(vkCreateDevice(pdevice, &deviceinfo, nullptr, &device)){
            throw std::runtime_error("Logical device creation failed");
        }

        VkQueue graphicsQueue;
        vkGetDeviceQueue(device, graphicsindex, 0, &graphicsQueue);

        if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS){
            throw std::runtime_error("Surface creation failed");
        }

        uint32_t formatcount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(pdevice, surface, &formatcount, nullptr);
        std::vector<VkSurfaceFormatKHR> surfaceformats(formatcount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(pdevice, surface, &formatcount, surfaceformats.data());

        VkBool32 supported;
        vkGetPhysicalDeviceSurfaceSupportKHR(pdevice, graphicsindex, surface, &supported);


        VkSwapchainCreateInfoKHR createinfo{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
        createinfo.surface = surface;
        createinfo.minImageCount = 4;
        createinfo.imageFormat = surfaceformats[0].format;
        createinfo.imageColorSpace = surfaceformats[0].colorSpace;
        createinfo.imageExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
        createinfo.imageArrayLayers = 1;
        createinfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createinfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createinfo.queueFamilyIndexCount = 0;
        createinfo.pQueueFamilyIndices = nullptr;
        createinfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createinfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        createinfo.clipped = VK_TRUE;
        createinfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        createinfo.oldSwapchain = VK_NULL_HANDLE;

        format = surfaceformats[0].format;
        extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        if(vkCreateSwapchainKHR(device, &createinfo, nullptr, &swapchain) != VK_SUCCESS){
            throw std::runtime_error("Failed to create swapchain");
        }

        uint32_t imagecount;
        vkGetSwapchainImagesKHR(device, swapchain, &imagecount, nullptr);
        std::vector<VkImage> swapchain_images(imagecount);
        vkGetSwapchainImagesKHR(device, swapchain, &imagecount, swapchain_images.data());

        for (auto image : swapchain_images) {
            VkImageViewCreateInfo ivinfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
            ivinfo.image = image;
            ivinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            ivinfo.format = format;
            ivinfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            ivinfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            ivinfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            ivinfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            ivinfo.subresourceRange.baseMipLevel = 0;
            ivinfo.subresourceRange.levelCount = 1;
            ivinfo.subresourceRange.baseArrayLayer = 0;
            ivinfo.subresourceRange.layerCount = 1;
            VkImageView iv;
            if(vkCreateImageView(device, &ivinfo, nullptr, &iv) != VK_SUCCESS){
                throw std::runtime_error("Failed to create swapchain");
            }
            swapchain_imageviews.push_back(iv);
        }

        auto code = readShaderFile("./shaders/image.frag");
        VkShaderModuleCreateInfo sdinfo{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
        sdinfo.codeSize = code.size();
        sdinfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if(vkCreateShaderModule(device, &sdinfo, nullptr, &fragment_module) != VK_SUCCESS){
            throw std::runtime_error("Failed to create shader module");
        }

        code = readShaderFile("./shaders/vert.vert");
        VkShaderModuleCreateInfo sdtinfo = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
        sdtinfo.codeSize = code.size();
        sdtinfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if(vkCreateShaderModule(device, &sdtinfo, nullptr, &vertex_module) != VK_SUCCESS){
            throw std::runtime_error("Failed to create shader module");
        }

        VkPipelineShaderStageCreateInfo fragmentinfo{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
        fragmentinfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentinfo.module = fragment_module;
        fragmentinfo.pName = "main";

        VkPipelineShaderStageCreateInfo vertexinfo{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
        vertexinfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexinfo.module = vertex_module;
        vertexinfo.pName = "main";

        VkPipelineVertexInputStateCreateInfo vinfo{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
        vinfo.vertexAttributeDescriptionCount = 0;
        vinfo.pVertexAttributeDescriptions = nullptr;
        vinfo.vertexBindingDescriptionCount = 0;
        vinfo.pVertexBindingDescriptions = nullptr;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) extent.width;
        viewport.height = (float) extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = extent;

        VkPipelineViewportStateCreateInfo vs{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
        vs.viewportCount = 1;
        vs.pViewports = &viewport;
        vs.scissorCount = 1;
        vs.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE; //Set to true since we don't need to operate on any geometry
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        if(vkCreateRenderPass(device, &renderPassInfo, nullptr, &render_pass) != VK_SUCCESS){
            throw std::runtime_error("Failed render pass creation");
        }

        VkGraphicsPipelineCreateInfo pipelineinfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
        pipelineinfo.stageCount = 2;
        std::vector<VkPipelineShaderStageCreateInfo> modules({vertexinfo, fragmentinfo});
        pipelineinfo.pStages = modules.data();
        pipelineinfo.pVertexInputState = &vinfo;
        pipelineinfo.pInputAssemblyState = &inputAssembly;
        pipelineinfo.pViewportState = &vs;
        pipelineinfo.pRasterizationState = &rasterizer;
        pipelineinfo.pMultisampleState = &multisampling;
        pipelineinfo.pDepthStencilState = nullptr;
        pipelineinfo.pColorBlendState = &colorBlending;
        pipelineinfo.pDynamicState = nullptr;
        pipelineinfo.layout = pipelineLayout;
        pipelineinfo.renderPass = render_pass;
        pipelineinfo.subpass = 0;
        pipelineinfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineinfo.basePipelineIndex = -1;

        if(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineinfo, nullptr, &pipeline) != VK_SUCCESS){
            throw std::runtime_error("Failed to create pipeline");
        }

        swapchainbuffers.resize(swapchain_imageviews.size());
        for (size_t i = 0; i < swapchain_imageviews.size(); ++i) {
            VkImageView attachments[] = { swapchain_imageviews[i] };
            VkFramebufferCreateInfo fbinfo{};
            fbinfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            fbinfo.renderPass = render_pass;
            //The input of the render pass is the swapchain imageviews
            fbinfo.attachmentCount = 1;
            fbinfo.pAttachments = attachments;
            fbinfo.width = extent.width;
            fbinfo.height = extent.height;
            fbinfo.layers = 1;

            if(vkCreateFramebuffer(device, &fbinfo, nullptr, &swapchainbuffers[i]) != VK_SUCCESS){
                throw std::runtime_error("Failed to create framebuffer");
            }

        }

        VkCommandPoolCreateInfo poolinfo{};
        poolinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolinfo.queueFamilyIndex = graphicsindex;
        poolinfo.flags = 0;

        if(vkCreateCommandPool(device, &poolinfo, nullptr, &commandpool) != VK_SUCCESS){
            throw std::runtime_error("Failed to create command pool");
        }

        commandbuffers.resize(swapchainbuffers.size());
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.commandPool = commandpool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) commandbuffers.size();

        if(vkAllocateCommandBuffers(device, &allocInfo, commandbuffers.data()) != VK_SUCCESS){
            throw std::runtime_error("Failed to allocate command buffers");
        }

        //record command buffers
        for (size_t i = 0; i < commandbuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0; // Optional
            beginInfo.pInheritanceInfo = nullptr; // Optional

            if (vkBeginCommandBuffer(commandbuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderpassbegin{};
            renderpassbegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderpassbegin.renderPass = render_pass;
            renderpassbegin.framebuffer = swapchainbuffers[i];
            renderpassbegin.renderArea.offset = {0, 0};
            renderpassbegin.renderArea.extent = extent;

            VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 0.0f};
            renderpassbegin.clearValueCount = 1;
            renderpassbegin.pClearValues = &clearColor;

            vkCmdBeginRenderPass(commandbuffers[i], &renderpassbegin, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(commandbuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
            vkCmdDraw(commandbuffers[i], 3, 1, 0, 0);
            vkCmdEndRenderPass(commandbuffers[i]);
            if(vkEndCommandBuffer(commandbuffers[i]) != VK_SUCCESS){
                throw std::runtime_error("failed to end recording command buffer!");
            }
        }


        //Draw frame

    };

    std::vector<char> readShaderFile(const std::string& filename) const {
        //Load fragment shader
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if(!file.is_open()){
            throw std::runtime_error("File not found");
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> content(fileSize);
        file.seekg(0);
        file.read(content.data(), fileSize);
        file.close();
        return content;
    }

    //Change window contents
    void write(const Buffer<Color>& buffer) const {

        uint32_t image_size = buffer.getWidth() * buffer.getHeight() * 3;

        //Allocate memory on the gpu and transfer the buffer data
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        createBuffer(image_size,
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     staging_buffer,
                     staging_buffer_memory);

        void* data;
        auto *pixels = new unsigned char[image_size];
        int index = 0;
        for (int i = 0; i < buffer.getHeight(); ++i) {
            for (int j = 0; j < buffer.getWidth(); ++j) {
                pixels[index] = static_cast<uint8_t>(buffer(i, j).r * 255.0f);
                pixels[index] = static_cast<uint8_t>(buffer(i, j).g * 255.0f);
                pixels[index] = static_cast<uint8_t>(buffer(i, j).b * 255.0f);
                ++index;
            }
        }
        vkMapMemory(device, staging_buffer_memory, 0, image_size, 0, &data); //Points data to the first byte of mapped memory
        memcpy(data, pixels, static_cast<uint32_t>(image_size));
        vkUnmapMemory(device, staging_buffer_memory);
        delete[](pixels);

        //Setup the image object
        VkImage texture;
        VkDeviceMemory texture_memory;
        VkImageCreateInfo iminfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
        iminfo.imageType = VK_IMAGE_TYPE_2D;
        iminfo.extent = {static_cast<uint32_t>(buffer.getWidth()), static_cast<uint32_t>(buffer.getHeight()), 1};
        iminfo.mipLevels = 1;
        iminfo.arrayLayers = 1;
        iminfo.format = VK_FORMAT_R8G8B8_SRGB;
        iminfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        iminfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        iminfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT; //We want to transfer to and sample the texture
        iminfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        iminfo.samples = VK_SAMPLE_COUNT_1_BIT;
        iminfo.flags = 0;

        if(vkCreateImage(device, &iminfo, nullptr, &texture) != VK_SUCCESS){
            throw std::runtime_error("Failed to creat texture");
        }

        VkMemoryRequirements memReqs;
        vkGetImageMemoryRequirements(device, texture, &memReqs);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReqs.size;
        allocInfo.memoryTypeIndex = findMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if(vkAllocateMemory(device, &allocInfo, nullptr, &texture_memory) != VK_SUCCESS){
            throw std::runtime_error("Failed to allocate texture memory");
        }

        vkBindImageMemory(device, texture, texture_memory, 0);

        //Now we need to fill the texture_memory on the GPU with the data that we have in staging_buffer_memory that is in a Host coherent area

    }

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        //Get the GPU memory requirements for the buffer
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties); //find the index of a suitable memory type

        if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(device, buffer, bufferMemory, 0);
    }
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(pdevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    ~WindowOutput(){
        vkDestroyCommandPool(device, commandpool, nullptr);
        for(auto framebuffer : swapchainbuffers){
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }
        vkDestroyPipeline(device, pipeline, nullptr);
        vkDestroyRenderPass(device, render_pass, nullptr);
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        vkDestroyShaderModule(device, vertex_module, nullptr);
        vkDestroyShaderModule(device, fragment_module, nullptr);
        for (auto iv : swapchain_imageviews) {
            vkDestroyImageView(device, iv, nullptr);
        }
        vkDestroySwapchainKHR(device, swapchain, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyDevice(device, nullptr);
        vkDestroyInstance(instance, nullptr);

        if(window != nullptr){
            glfwDestroyWindow(window);
        }
        glfwTerminate();
    }

private:
    //GLFW window management variables
    GLFWwindow* window = nullptr;

    //Vulkan instance variables
    VkInstance instance;
    VkPhysicalDevice pdevice;
    VkDevice device;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkFormat format;
    VkExtent2D extent;
    std::vector<VkImageView> swapchain_imageviews;
    VkShaderModule fragment_module;
    VkShaderModule vertex_module;
    VkPipelineLayout pipelineLayout;
    VkRenderPass render_pass;
    VkPipeline pipeline;
    std::vector<VkFramebuffer> swapchainbuffers;
    VkCommandPool commandpool;
    std::vector<VkCommandBuffer> commandbuffers;


    const std::vector<const char*> extension_layers = { "VK_LAYER_KHRONOS_validation" };
    const std::vector<const char*> device_layers = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    const std::vector<const char*> validation_layers = { "VK_LAYER_KHRONOS_validation" };
};
