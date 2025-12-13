#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#ifdef __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#endif
#include <VkBootstrap.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <set>
#include <algorithm>
#include <fstream>
#include <array>

#define VULKAN_RENDERER

#ifdef __APPLE__
// MoltenVK specific definitions
#define VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK ((VkStructureType)1000123000)
typedef struct VkMacOSSurfaceCreateInfoMVK {
    VkStructureType    sType;
    const void*        pNext;
    VkFlags            flags;
    const void*        pView;
} VkMacOSSurfaceCreateInfoMVK;

typedef VkResult (VKAPI_PTR *PFN_vkCreateMacOSSurfaceMVK)(
    VkInstance                        instance,
    const VkMacOSSurfaceCreateInfoMVK* pCreateInfo,
    const VkAllocationCallbacks*      pAllocator,
    VkSurfaceKHR*                     pSurface);
#endif

struct QueueFamilyIndex {
    uint32_t graphicsFamilyIndex = 0;
    uint32_t presentFamilyIndex = 0;
    VkBool32 graphicsSupport = VK_FALSE;
    VkBool32 presentSupport = VK_FALSE;

    bool isComplete() {
        return graphicsSupport && presentSupport;
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class MentalWindowManager
{
private:
    GLFWwindow *pWindow = nullptr;
    int aSizes[2] = {800, 600};
    VkInstance vk_instance = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkDevice logical_device = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkQueue graphics_queue = VK_NULL_HANDLE;
    VkQueue present_queue = VK_NULL_HANDLE;
    
    // Swapchain
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> swapchain_images;
    VkFormat swapchain_image_format;
    VkExtent2D swapchain_extent;
    std::vector<VkImageView> swapchain_image_views;
    
    // Render pass and pipeline
    VkRenderPass render_pass = VK_NULL_HANDLE;
    VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
    VkPipeline graphics_pipeline = VK_NULL_HANDLE;
    
    // Framebuffers and command buffers
    std::vector<VkFramebuffer> swapchain_framebuffers;
    VkCommandPool command_pool = VK_NULL_HANDLE;
    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    
    // Synchronization
    VkSemaphore image_available_semaphore = VK_NULL_HANDLE;
    VkSemaphore render_finished_semaphore = VK_NULL_HANDLE;
    VkFence in_flight_fence = VK_NULL_HANDLE;


    void createVulkanInstance()
    {
        VkApplicationInfo appInfo {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Mental Engine";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Mental";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t glfw_extensions_count = 0;
        const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);

        std::vector<const char*> extensions;
        
        // Add GLFW extensions if available
        for (uint32_t i = 0; i < glfw_extensions_count; i++) {
            extensions.push_back(glfw_extensions[i]);
        }
        
        // Add required extensions manually for MoltenVK
        if (glfw_extensions_count == 0) {
            // GLFW doesn't see Vulkan, add extensions manually
            extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
            #ifdef __APPLE__
            extensions.push_back("VK_MVK_macos_surface");
            #endif
        }
        
        #ifdef __APPLE__
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        #endif
        
        spdlog::info("Using {} Vulkan extensions:", extensions.size());
        for (const auto& ext : extensions) {
            spdlog::info("  - {}", ext);
        }

        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkResult result = vkCreateInstance(&createInfo, nullptr, &this->vk_instance);
        if (result != VK_SUCCESS) {
            spdlog::error("Failed to create vulkan instance: {}", static_cast<int>(result));
            throw std::runtime_error("Failed to create vulkan instance");
        }
        
        spdlog::info("Vulkan instance created successfully");
    }

    void create_surface() {
        spdlog::info("Creating surface after physical device selection...");
        
        #ifdef __APPLE__
        // For MoltenVK, always use manual surface creation for better control
        spdlog::info("Creating MoltenVK surface manually...");
        
        // Ensure window is ready
        glfwPollEvents();
        
        VkMacOSSurfaceCreateInfoMVK createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.pView = glfwGetCocoaWindow(this->pWindow);
        
        if (!createInfo.pView) {
            spdlog::error("Failed to get Cocoa window view");
            throw std::runtime_error("Failed to get Cocoa window view");
        }
        
        spdlog::info("Cocoa window view obtained: {}", reinterpret_cast<uintptr_t>(createInfo.pView));
        
        // Get the function pointer
        PFN_vkCreateMacOSSurfaceMVK vkCreateMacOSSurfaceMVK = 
            (PFN_vkCreateMacOSSurfaceMVK)vkGetInstanceProcAddr(this->vk_instance, "vkCreateMacOSSurfaceMVK");
        
        if (!vkCreateMacOSSurfaceMVK) {
            spdlog::error("vkCreateMacOSSurfaceMVK function not available");
            throw std::runtime_error("Failed to get MoltenVK surface creation function");
        }
        
        VkResult result = vkCreateMacOSSurfaceMVK(this->vk_instance, &createInfo, nullptr, &this->surface);
        if (result != VK_SUCCESS) {
            spdlog::error("MoltenVK surface creation failed: {} ({})", 
                         static_cast<int>(result), getVkResultString(result));
            throw std::runtime_error("Failed to create MoltenVK surface");
        }
        
        spdlog::info("MoltenVK surface created successfully");
        #else
        // Try GLFW surface creation for other platforms
        VkResult result = glfwCreateWindowSurface(this->vk_instance, this->pWindow, nullptr, &this->surface);
        if (result != VK_SUCCESS) {
            spdlog::error("GLFW surface creation failed, VkResult: {} ({})", 
                         static_cast<int>(result), getVkResultString(result));
            throw std::runtime_error("Failed to create GLFW surface.");
        } else {
            spdlog::info("GLFW surface created successfully");
        }
        #endif
        
        spdlog::info("Surface handle: {}", reinterpret_cast<uintptr_t>(surface));
        
        // Verify surface was created properly
        if (surface == VK_NULL_HANDLE) {
            spdlog::error("Surface handle is null after creation!");
            throw std::runtime_error("Surface creation returned null handle");
        }
    }
    
    const char* getVkResultString(VkResult result) {
        switch (result) {
            case VK_SUCCESS: return "VK_SUCCESS";
            case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
            case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
            case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
            case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
            case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR (Surface became invalid)";
            case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
            case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
            case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
            case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
            case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
            case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
            case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
            case VK_ERROR_UNKNOWN: return "VK_ERROR_UNKNOWN";
            case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
            case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
            case VK_ERROR_FRAGMENTATION: return "VK_ERROR_FRAGMENTATION";
            case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
            default: return "UNKNOWN_VK_RESULT";
        }
    }

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;
        
        VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, this->surface, &details.capabilities);
        if (result != VK_SUCCESS) {
            spdlog::error("Failed to get surface capabilities: {}", static_cast<int>(result));
        }

        uint32_t formatCount = 0;
        result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->surface, &formatCount, nullptr);
        if (result != VK_SUCCESS) {
            spdlog::error("Failed to get surface formats count: {}", static_cast<int>(result));
        } else {
            spdlog::info("Surface formats count: {}", formatCount);
        }

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->surface, &formatCount, details.formats.data());
            if (result != VK_SUCCESS) {
                spdlog::error("Failed to get surface formats: {}", static_cast<int>(result));
            }
        }

        uint32_t presentModeCount = 0;
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, this->surface, &presentModeCount, nullptr);
        if (result != VK_SUCCESS) {
            spdlog::error("Failed to get present modes count: {}", static_cast<int>(result));
        }
 
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount); 
            result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, this->surface, &presentModeCount, details.presentModes.data());
            if (result != VK_SUCCESS) {
                spdlog::error("Failed to get present modes: {}", static_cast<int>(result));
            }
        }
 
        return details;
    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        if (availableFormats.empty()) {
            // Fallback for MoltenVK
            VkSurfaceFormatKHR fallback{};
            fallback.format = VK_FORMAT_B8G8R8A8_UNORM;
            fallback.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
            spdlog::info("Using fallback surface format: B8G8R8A8_UNORM");
            return fallback;
        }
        
        // Log available formats for debugging
        spdlog::info("Available surface formats:");
        for (const auto& format : availableFormats) {
            spdlog::info("  - Format: {}, ColorSpace: {}", 
                        static_cast<int>(format.format), static_cast<int>(format.colorSpace));
        }
        
        // Prefer B8G8R8A8_UNORM for better MoltenVK compatibility
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && 
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                spdlog::info("Selected surface format: B8G8R8A8_UNORM");
                return availableFormat;
            }
        }
        
        // Try SRGB format as second choice
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && 
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                spdlog::info("Selected surface format: B8G8R8A8_SRGB");
                return availableFormat;
            }
        }
        
        // Use first available format as last resort
        spdlog::info("Using first available surface format: {}", static_cast<int>(availableFormats[0].format));
        return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        // Log available present modes for debugging
        spdlog::info("Available present modes:");
        for (const auto& mode : availablePresentModes) {
            spdlog::info("  - Present mode: {}", static_cast<int>(mode));
        }
        
        #ifdef __APPLE__
        // For MoltenVK, prefer FIFO for better stability
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR) {
                spdlog::info("Selected present mode: FIFO (MoltenVK optimized)");
                return availablePresentMode;
            }
        }
        
        // Try immediate mode as fallback for MoltenVK
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                spdlog::info("Selected present mode: IMMEDIATE (MoltenVK fallback)");
                return availablePresentMode;
            }
        }
        #else
        // For native Vulkan, prefer mailbox (triple buffering) for better performance
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                spdlog::info("Selected present mode: MAILBOX");
                return availablePresentMode;
            }
        }
        #endif
        
        // FIFO is always available according to Vulkan spec
        spdlog::info("Using fallback present mode: FIFO");
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(pWindow, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, 
                capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, 
                capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void createSwapchain() {
        #ifdef __APPLE__
        // For MoltenVK, recreate surface right before swapchain creation to avoid surface lost errors
        spdlog::info("Recreating surface for MoltenVK swapchain compatibility...");
        
        if (surface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(vk_instance, surface, nullptr);
            surface = VK_NULL_HANDLE;
        }
        
        // Recreate surface
        VkMacOSSurfaceCreateInfoMVK surfaceCreateInfo{};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
        surfaceCreateInfo.pNext = nullptr;
        surfaceCreateInfo.flags = 0;
        surfaceCreateInfo.pView = glfwGetCocoaWindow(pWindow);
        
        if (!surfaceCreateInfo.pView) {
            spdlog::error("Failed to get Cocoa window view for swapchain");
            throw std::runtime_error("Failed to get Cocoa window view for swapchain");
        }
        
        PFN_vkCreateMacOSSurfaceMVK vkCreateMacOSSurfaceMVK = 
            (PFN_vkCreateMacOSSurfaceMVK)vkGetInstanceProcAddr(vk_instance, "vkCreateMacOSSurfaceMVK");
        
        if (!vkCreateMacOSSurfaceMVK) {
            spdlog::error("vkCreateMacOSSurfaceMVK function not available for swapchain");
            throw std::runtime_error("Failed to get MoltenVK surface creation function for swapchain");
        }
        
        VkResult surfaceResult = vkCreateMacOSSurfaceMVK(vk_instance, &surfaceCreateInfo, nullptr, &surface);
        if (surfaceResult != VK_SUCCESS) {
            spdlog::error("Failed to recreate MoltenVK surface for swapchain: {} ({})", 
                         static_cast<int>(surfaceResult), getVkResultString(surfaceResult));
            throw std::runtime_error("Failed to recreate MoltenVK surface for swapchain");
        }
        
        spdlog::info("Surface recreated successfully for swapchain");
        
        // Use hardcoded values for MoltenVK to avoid surface queries that might fail
        SwapChainSupportDetails swapChainSupport;
        swapChainSupport.capabilities.minImageCount = 2;
        swapChainSupport.capabilities.maxImageCount = 3;
        swapChainSupport.capabilities.currentExtent = {static_cast<uint32_t>(aSizes[0]), static_cast<uint32_t>(aSizes[1])};
        swapChainSupport.capabilities.minImageExtent = {1, 1};
        swapChainSupport.capabilities.maxImageExtent = {4096, 4096};
        swapChainSupport.capabilities.maxImageArrayLayers = 1;
        swapChainSupport.capabilities.supportedTransforms = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapChainSupport.capabilities.currentTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapChainSupport.capabilities.supportedCompositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapChainSupport.capabilities.supportedUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        
        // Use B8G8R8A8_UNORM format for better MoltenVK compatibility
        VkSurfaceFormatKHR format{};
        format.format = VK_FORMAT_B8G8R8A8_UNORM;
        format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        swapChainSupport.formats.push_back(format);
        
        // Use FIFO present mode for MoltenVK stability
        swapChainSupport.presentModes.push_back(VK_PRESENT_MODE_FIFO_KHR);
        
        spdlog::info("Using MoltenVK hardcoded swapchain settings for compatibility");
        #else
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physical_device);
        #endif

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        // Use minimum image count for MoltenVK compatibility
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount;

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndex indices = findQueueFamilies(physical_device);

        // For MoltenVK, always use exclusive mode for better compatibility
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        spdlog::info("Creating swapchain with:");
        spdlog::info("  - Images: {}", imageCount);
        spdlog::info("  - Format: {}", static_cast<int>(surfaceFormat.format));
        spdlog::info("  - ColorSpace: {}", static_cast<int>(surfaceFormat.colorSpace));
        spdlog::info("  - Extent: {}x{}", extent.width, extent.height);
        spdlog::info("  - PresentMode: {}", static_cast<int>(presentMode));
        
        // Add detailed debugging before swapchain creation
        spdlog::info("Swapchain creation details:");
        spdlog::info("  - Surface handle: {}", reinterpret_cast<uintptr_t>(surface));
        spdlog::info("  - Logical device: {}", reinterpret_cast<uintptr_t>(logical_device));
        spdlog::info("  - Physical device: {}", reinterpret_cast<uintptr_t>(physical_device));
        spdlog::info("  - Image sharing mode: {}", static_cast<int>(createInfo.imageSharingMode));
        spdlog::info("  - Pre-transform: {}", static_cast<int>(createInfo.preTransform));
        spdlog::info("  - Composite alpha: {}", static_cast<int>(createInfo.compositeAlpha));
        
        // Verify surface is still valid before swapchain creation
        VkBool32 surfaceSupported = VK_FALSE;
        VkResult surfaceCheckResult = vkGetPhysicalDeviceSurfaceSupportKHR(
            physical_device, indices.presentFamilyIndex, surface, &surfaceSupported);
        
        spdlog::info("Surface validation before swapchain:");
        spdlog::info("  - Surface support check result: {} ({})", 
                    static_cast<int>(surfaceCheckResult), getVkResultString(surfaceCheckResult));
        spdlog::info("  - Surface supported: {}", surfaceSupported ? "YES" : "NO");
        
        if (surfaceCheckResult != VK_SUCCESS || !surfaceSupported) {
            spdlog::error("Surface validation failed before swapchain creation!");
            spdlog::error("  - Check result: {} ({})", 
                         static_cast<int>(surfaceCheckResult), getVkResultString(surfaceCheckResult));
            spdlog::error("  - Surface supported: {}", surfaceSupported ? "YES" : "NO");
        }
        
        VkResult result = vkCreateSwapchainKHR(logical_device, &createInfo, nullptr, &swapchain);
        if (result != VK_SUCCESS) {
            spdlog::error("DETAILED SWAPCHAIN CREATION FAILURE:");
            spdlog::error("  - VkResult: {} ({})", static_cast<int>(result), getVkResultString(result));
            spdlog::error("  - Surface: {}", reinterpret_cast<uintptr_t>(surface));
            spdlog::error("  - Device: {}", reinterpret_cast<uintptr_t>(logical_device));
            spdlog::error("  - Min images: {}", createInfo.minImageCount);
            spdlog::error("  - Format: {}", static_cast<int>(createInfo.imageFormat));
            spdlog::error("  - Color space: {}", static_cast<int>(createInfo.imageColorSpace));
            spdlog::error("  - Extent: {}x{}", createInfo.imageExtent.width, createInfo.imageExtent.height);
            spdlog::error("  - Array layers: {}", createInfo.imageArrayLayers);
            spdlog::error("  - Usage: {}", static_cast<int>(createInfo.imageUsage));
            spdlog::error("  - Sharing mode: {}", static_cast<int>(createInfo.imageSharingMode));
            spdlog::error("  - Pre-transform: {}", static_cast<int>(createInfo.preTransform));
            spdlog::error("  - Composite alpha: {}", static_cast<int>(createInfo.compositeAlpha));
            spdlog::error("  - Present mode: {}", static_cast<int>(createInfo.presentMode));
            spdlog::error("  - Clipped: {}", createInfo.clipped ? "YES" : "NO");
            
            // Try to get more information about the surface state
            VkSurfaceCapabilitiesKHR caps;
            VkResult capsResult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &caps);
            spdlog::error("  - Surface capabilities query: {} ({})", 
                         static_cast<int>(capsResult), getVkResultString(capsResult));
            
            if (capsResult == VK_SUCCESS) {
                spdlog::error("  - Surface min images: {}", caps.minImageCount);
                spdlog::error("  - Surface max images: {}", caps.maxImageCount);
                spdlog::error("  - Surface current extent: {}x{}", caps.currentExtent.width, caps.currentExtent.height);
                spdlog::error("  - Surface transforms: {}", static_cast<int>(caps.supportedTransforms));
                spdlog::error("  - Surface current transform: {}", static_cast<int>(caps.currentTransform));
            }
            
            throw std::runtime_error("Failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(logical_device, swapchain, &imageCount, nullptr);
        swapchain_images.resize(imageCount);
        vkGetSwapchainImagesKHR(logical_device, swapchain, &imageCount, swapchain_images.data());

        swapchain_image_format = surfaceFormat.format;
        swapchain_extent = extent;
        
        spdlog::info("Swapchain created successfully with {} images, format: {}, extent: {}x{}", 
                    imageCount, static_cast<int>(swapchain_image_format), 
                    swapchain_extent.width, swapchain_extent.height);
    }

    bool checkSwapchainExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
 
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
 
        std::vector<std::string> requiredExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        
        #ifdef __APPLE__
        requiredExtensions.push_back("VK_KHR_portability_subset");
        #endif
        
        spdlog::info("  Available device extensions:");
        for (const auto& extension : availableExtensions) {
            spdlog::info("    - {}", extension.extensionName);
        }
        
        for (const auto& required : requiredExtensions) {
            bool found = false;
            for (const auto& extension : availableExtensions) {
                if (required == extension.extensionName) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                spdlog::error("  Required extension {} not found", required);
                return false;
            }
        }
 
        return true;
    }

    bool isDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndex indices = findQueueFamilies(device);
        if(!indices.isComplete()) return false; 
        
        // если не поддерживаются расширения
        if(!checkSwapchainExtensionSupport(device)) return false;
        
        #ifdef __APPLE__
        // For MoltenVK, skip swapchain support queries as they might fail
        // but swapchain will still work. Just check extensions and queues.
        spdlog::info("  MoltenVK device accepted (skipping swapchain queries)");
        return true;
        #else
        // получаем поддержку цепочки буферов
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        // считаем устройство подходящим, если поддерживается хотя бы один формат и хотя бы один режим представления
        return !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        #endif
    }

    void create_logical_device()
    {
        QueueFamilyIndex indices = findQueueFamilies(this->physical_device);
 
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {
            indices.graphicsFamilyIndex,
            indices.presentFamilyIndex
        };
 
        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }
 
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
 
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        
        // Enable swapchain extension
        std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        
        #ifdef __APPLE__
        // Add MoltenVK specific extensions
        deviceExtensions.push_back("VK_KHR_portability_subset");
        #endif
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
 
        if (vkCreateDevice(this->physical_device, &createInfo, nullptr, &this->logical_device) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device!");
        }

        vkGetDeviceQueue(this->logical_device, indices.graphicsFamilyIndex, 0, &this->graphics_queue);
        vkGetDeviceQueue(this->logical_device, indices.presentFamilyIndex, 0, &this->present_queue);
    }

    QueueFamilyIndex findQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndex indices;
 
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
 
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
 
        spdlog::info("Found {} queue families:", queueFamilyCount);
        
        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            spdlog::info("  Queue family {}: flags={}, count={}", 
                        i, static_cast<int>(queueFamily.queueFlags), queueFamily.queueCount);
            
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamilyIndex = i;
                indices.graphicsSupport = VK_TRUE;
                spdlog::info("    - Graphics support: YES");
                
                // Always check present support, even on MoltenVK
                VkBool32 presentSupport = VK_FALSE;
                VkResult presentResult = vkGetPhysicalDeviceSurfaceSupportKHR(device, i, this->surface, &presentSupport);
                spdlog::info("    - Present support check: {} ({}), supported: {}", 
                            static_cast<int>(presentResult), getVkResultString(presentResult), 
                            presentSupport ? "YES" : "NO");
                
                if (presentResult == VK_SUCCESS && presentSupport) {
                    indices.presentFamilyIndex = i;
                    indices.presentSupport = VK_TRUE;
                    spdlog::info("    - Using queue family {} for both graphics and present", i);
                }
            }
            
            // Also check present support on non-graphics queues
            if (!(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
                VkBool32 presentSupport = VK_FALSE;
                VkResult presentResult = vkGetPhysicalDeviceSurfaceSupportKHR(device, i, this->surface, &presentSupport);
                if (presentResult == VK_SUCCESS && presentSupport && !indices.presentSupport) {
                    indices.presentFamilyIndex = i;
                    indices.presentSupport = VK_TRUE;
                    spdlog::info("    - Using separate queue family {} for present", i);
                }
            }
            
            if (indices.isComplete()) {
                break;
            }
            i++;
        }
        
        spdlog::info("Final queue family selection:");
        spdlog::info("  - Graphics: family={}, supported={}", 
                    indices.graphicsFamilyIndex, indices.graphicsSupport ? "YES" : "NO");
        spdlog::info("  - Present: family={}, supported={}", 
                    indices.presentFamilyIndex, indices.presentSupport ? "YES" : "NO");
        
        return indices;
    }

    void select_physical_device_without_surface()
    {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(vk_instance, &device_count, nullptr);

        if (device_count == 0)
            throw std::runtime_error("No selectable GPU.");
        
        spdlog::info("Found {} GPU device(s)", device_count);
        
        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(this->vk_instance, &device_count, devices.data());

        for (const auto& device : devices) {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            spdlog::info("Checking device: {}", deviceProperties.deviceName);
            
            // Check basic device suitability without surface
            bool swapchainSupport = checkSwapchainExtensionSupport(device);
            spdlog::info("  Swapchain extension: {}", swapchainSupport ? "YES" : "NO");
            
            if (swapchainSupport) {
                this->physical_device = device;
                spdlog::info("Selected device: {}", deviceProperties.deviceName);
                break;
            }
        }

        if (this->physical_device == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to find suitable GPU device.");
        }
    }

    void verify_device_surface_compatibility()
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physical_device, &deviceProperties);
        spdlog::info("Verifying surface compatibility for device: {}", deviceProperties.deviceName);
        
        QueueFamilyIndex indices = findQueueFamilies(physical_device);
        spdlog::info("  Graphics support: {}, Present support: {}", 
                    indices.graphicsSupport ? "YES" : "NO",
                    indices.presentSupport ? "YES" : "NO");
        
        if (!indices.isComplete()) {
            spdlog::error("Device does not support required queue families with this surface!");
            throw std::runtime_error("Device does not support required queue families with surface.");
        }
        
        spdlog::info("Device surface compatibility verified successfully");
    }

    void select_physical_device()
    {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(vk_instance, &device_count, nullptr);

        if (device_count == 0)
            throw std::runtime_error("No selectable GPU.");
        
        spdlog::info("Found {} GPU device(s)", device_count);
        
        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(this->vk_instance, &device_count, devices.data());

        for (const auto& device : devices) {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            spdlog::info("Checking device: {}", deviceProperties.deviceName);
            
            QueueFamilyIndex indices = findQueueFamilies(device);
            bool swapchainSupport = checkSwapchainExtensionSupport(device);
            SwapChainSupportDetails swapDetails = querySwapChainSupport(device);
            
            spdlog::info("  Graphics support: {}, Present support: {}", 
                        indices.graphicsSupport ? "YES" : "NO",
                        indices.presentSupport ? "YES" : "NO");
            spdlog::info("  Swapchain extension: {}, Formats: {}, Present modes: {}", 
                        swapchainSupport ? "YES" : "NO",
                        swapDetails.formats.size(),
                        swapDetails.presentModes.size());
            
            if (this->isDeviceSuitable(device)) {
                this->physical_device = device;
                spdlog::info("Selected device: {}", deviceProperties.deviceName);
                break;
            }
        }

        if (this->physical_device == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to find suitable GPU device.");
        }
    }

public:
    MentalWindowManager()
    {
        if (!glfwInit()) {
            spdlog::error("Failed to initialize GLFW library.");
            throw std::runtime_error("Failed to initialize GLFW library.");
        }

        #if defined(VULKAN_RENDERER)
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        #endif

        this->pWindow = glfwCreateWindow(aSizes[0], aSizes[1], "Mental Game engine", nullptr, nullptr);
        if (!this->pWindow) {
            spdlog::error("Failed to create GLFW window");
            throw std::runtime_error("Failed to create GLFW window");
        }
        
        // Ensure window is fully initialized
        glfwShowWindow(this->pWindow);
        glfwPollEvents();
        
        spdlog::info("GLFW window has been created!");
        
        #if defined(VULKAN_RENDERER)
        createVulkanInstance();
        select_physical_device_without_surface();
        create_surface();
        verify_device_surface_compatibility();
        create_logical_device();
        createSwapchain();
        createImageViews();
        createRenderPass();
        createGraphicsPipeline();
        createFramebuffers();
        createCommandPool();
        createCommandBuffer();
        createSyncObjects();
        spdlog::info("Vulkan triangle rendering pipeline initialized successfully");
        #endif
    }

    void run() {
        while (!glfwWindowShouldClose(this->pWindow))
        {
            glfwPollEvents();
            #if defined(VULKAN_RENDERER)
            drawFrame();
            #endif
        }
    }

    void createImageViews() {
        swapchain_image_views.resize(swapchain_images.size());

        for (size_t i = 0; i < swapchain_images.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapchain_images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapchain_image_format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(logical_device, &createInfo, nullptr, &swapchain_image_views[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create image views!");
            }
        }
        
        spdlog::info("Created {} image views", swapchain_image_views.size());
    }

    VkShaderModule createShaderModule(const std::vector<uint32_t>& code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size() * sizeof(uint32_t);
        createInfo.pCode = code.data();

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(logical_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module!");
        }

        return shaderModule;
    }

    void createRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapchain_image_format;
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

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(logical_device, &renderPassInfo, nullptr, &render_pass) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create render pass!");
        }
        
        spdlog::info("Render pass created");
    }

    void createGraphicsPipeline() {
        // Простые встроенные шейдеры для треугольника
        std::vector<uint32_t> vertShaderCode = {
            0x07230203, 0x00010000, 0x00080007, 0x0000002e, 0x00000000, 0x00020011, 0x00000001, 0x0006000b,
            0x00000001, 0x4c534c47, 0x6474732e, 0x3035342e, 0x00000000, 0x0003000e, 0x00000000, 0x00000001,
            0x0008000f, 0x00000000, 0x00000004, 0x6e69616d, 0x00000000, 0x0000000b, 0x0000001b, 0x0000001c,
            0x00030003, 0x00000002, 0x000001c2, 0x00040005, 0x00000004, 0x6e69616d, 0x00000000, 0x00050005,
            0x0000000b, 0x505f6c67, 0x65567265, 0x78657472, 0x00000000, 0x00060006, 0x0000000b, 0x00000000,
            0x505f6c67, 0x7469736f, 0x006e6f69, 0x00070006, 0x0000000b, 0x00000001, 0x505f6c67, 0x746e696f,
            0x657a6953, 0x00000000, 0x00070006, 0x0000000b, 0x00000002, 0x435f6c67, 0x4470696c, 0x61747369,
            0x0065636e, 0x00070006, 0x0000000b, 0x00000003, 0x435f6c67, 0x446c6c75, 0x61747369, 0x0065636e,
            0x00030005, 0x0000000d, 0x00000000, 0x00050005, 0x0000001b, 0x67617266, 0x6f6c6f43, 0x00000072,
            0x00040005, 0x0000001c, 0x6f6c6f63, 0x00000072, 0x00050048, 0x0000000b, 0x00000000, 0x0000000b,
            0x00000000, 0x00050048, 0x0000000b, 0x00000001, 0x0000000b, 0x00000001, 0x00050048, 0x0000000b,
            0x00000002, 0x0000000b, 0x00000003, 0x00050048, 0x0000000b, 0x00000003, 0x0000000b, 0x00000004,
            0x00030047, 0x0000000b, 0x00000002, 0x00040047, 0x0000001b, 0x0000001e, 0x00000000, 0x00040047,
            0x0000001c, 0x0000001e, 0x00000000, 0x00020013, 0x00000002, 0x00030021, 0x00000003, 0x00000002,
            0x00030016, 0x00000006, 0x00000020, 0x00040017, 0x00000007, 0x00000006, 0x00000004, 0x00040015,
            0x00000008, 0x00000020, 0x00000000, 0x0004002b, 0x00000008, 0x00000009, 0x00000001, 0x0004001c,
            0x0000000a, 0x00000006, 0x00000009, 0x0006001e, 0x0000000b, 0x00000007, 0x00000006, 0x0000000a,
            0x0000000a, 0x00040020, 0x0000000c, 0x00000003, 0x0000000b, 0x0004003b, 0x0000000c, 0x0000000d,
            0x00000003, 0x00040015, 0x0000000e, 0x00000020, 0x00000001, 0x0004002b, 0x0000000e, 0x0000000f,
            0x00000000, 0x0004002b, 0x00000008, 0x00000010, 0x00000003, 0x0004001c, 0x00000011, 0x00000007,
            0x00000010, 0x0004002b, 0x00000006, 0x00000013, 0x00000000, 0x0004002b, 0x00000006, 0x00000014,
            0x3f800000, 0x0007002c, 0x00000007, 0x00000015, 0x00000013, 0x00000014, 0x00000013, 0x00000014,
            0x0004002b, 0x00000006, 0x00000016, 0x3f000000, 0x0007002c, 0x00000007, 0x00000017, 0x00000016,
            0x00000013, 0x00000013, 0x00000014, 0x0004002b, 0x00000006, 0x00000018, 0xbf000000, 0x0007002c,
            0x00000007, 0x00000019, 0x00000018, 0x00000013, 0x00000013, 0x00000014, 0x0006002c, 0x00000011,
            0x0000001a, 0x00000015, 0x00000017, 0x00000019, 0x00040020, 0x0000001d, 0x00000003, 0x00000007,
            0x0004003b, 0x0000001d, 0x0000001b, 0x00000003, 0x00040020, 0x0000001f, 0x00000001, 0x00000007,
            0x0004003b, 0x0000001f, 0x0000001c, 0x00000001, 0x00040020, 0x00000021, 0x00000003, 0x00000007,
            0x00050036, 0x00000002, 0x00000004, 0x00000000, 0x00000003, 0x000200f8, 0x00000005, 0x0004003b,
            0x00000021, 0x00000022, 0x00000007, 0x00050041, 0x00000021, 0x00000023, 0x0000000d, 0x0000000f,
            0x0004003d, 0x0000000e, 0x00000024, 0x00000025, 0x00050041, 0x00000021, 0x00000026, 0x0000001a,
            0x00000024, 0x0004003d, 0x00000007, 0x00000027, 0x00000026, 0x0003003e, 0x00000022, 0x00000027,
            0x0004003d, 0x00000007, 0x00000028, 0x00000022, 0x0003003e, 0x00000023, 0x00000028, 0x0004003d,
            0x00000007, 0x00000029, 0x0000001c, 0x0003003e, 0x0000001b, 0x00000029, 0x000100fd, 0x00010038
        };

        std::vector<uint32_t> fragShaderCode = {
            0x07230203, 0x00010000, 0x00080007, 0x00000014, 0x00000000, 0x00020011, 0x00000001, 0x0006000b,
            0x00000001, 0x4c534c47, 0x6474732e, 0x3035342e, 0x00000000, 0x0003000e, 0x00000000, 0x00000001,
            0x0007000f, 0x00000004, 0x00000004, 0x6e69616d, 0x00000000, 0x00000009, 0x0000000b, 0x00030010,
            0x00000004, 0x00000007, 0x00030003, 0x00000002, 0x000001c2, 0x00040005, 0x00000004, 0x6e69616d,
            0x00000000, 0x00050005, 0x00000009, 0x4374756f, 0x726f6c6f, 0x00000000, 0x00050005, 0x0000000b,
            0x67617266, 0x6f6c6f43, 0x00000072, 0x00040047, 0x00000009, 0x0000001e, 0x00000000, 0x00040047,
            0x0000000b, 0x0000001e, 0x00000000, 0x00020013, 0x00000002, 0x00030021, 0x00000003, 0x00000002,
            0x00030016, 0x00000006, 0x00000020, 0x00040017, 0x00000007, 0x00000006, 0x00000004, 0x00040020,
            0x00000008, 0x00000003, 0x00000007, 0x0004003b, 0x00000008, 0x00000009, 0x00000003, 0x00040020,
            0x0000000a, 0x00000001, 0x00000007, 0x0004003b, 0x0000000a, 0x0000000b, 0x00000001, 0x00050036,
            0x00000002, 0x00000004, 0x00000000, 0x00000003, 0x000200f8, 0x00000005, 0x0004003d, 0x00000007,
            0x0000000c, 0x0000000b, 0x0003003e, 0x00000009, 0x0000000c, 0x000100fd, 0x00010038
        };

        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapchain_extent.width;
        viewport.height = (float) swapchain_extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapchain_extent;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        if (vkCreatePipelineLayout(logical_device, &pipelineLayoutInfo, nullptr, &pipeline_layout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.layout = pipeline_layout;
        pipelineInfo.renderPass = render_pass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(logical_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphics_pipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(logical_device, fragShaderModule, nullptr);
        vkDestroyShaderModule(logical_device, vertShaderModule, nullptr);
        
        spdlog::info("Graphics pipeline created");
    }

    void createFramebuffers() {
        swapchain_framebuffers.resize(swapchain_image_views.size());

        for (size_t i = 0; i < swapchain_image_views.size(); i++) {
            VkImageView attachments[] = {
                swapchain_image_views[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = render_pass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = swapchain_extent.width;
            framebufferInfo.height = swapchain_extent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(logical_device, &framebufferInfo, nullptr, &swapchain_framebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create framebuffer!");
            }
        }
        
        spdlog::info("Created {} framebuffers", swapchain_framebuffers.size());
    }

    void createCommandPool() {
        QueueFamilyIndex queueFamilyIndices = findQueueFamilies(physical_device);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamilyIndex;

        if (vkCreateCommandPool(logical_device, &poolInfo, nullptr, &command_pool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool!");
        }
        
        spdlog::info("Command pool created");
    }

    void createCommandBuffer() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = command_pool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(logical_device, &allocInfo, &command_buffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
        
        spdlog::info("Command buffer created");
    }

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = render_pass;
        renderPassInfo.framebuffer = swapchain_framebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchain_extent;

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);
        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer!");
        }
    }

    void createSyncObjects() {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateSemaphore(logical_device, &semaphoreInfo, nullptr, &image_available_semaphore) != VK_SUCCESS ||
            vkCreateSemaphore(logical_device, &semaphoreInfo, nullptr, &render_finished_semaphore) != VK_SUCCESS ||
            vkCreateFence(logical_device, &fenceInfo, nullptr, &in_flight_fence) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create synchronization objects!");
        }
        
        spdlog::info("Synchronization objects created");
    }

    void drawFrame() {
        vkWaitForFences(logical_device, 1, &in_flight_fence, VK_TRUE, UINT64_MAX);
        vkResetFences(logical_device, 1, &in_flight_fence);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(logical_device, swapchain, UINT64_MAX, image_available_semaphore, VK_NULL_HANDLE, &imageIndex);

        vkResetCommandBuffer(command_buffer, 0);
        recordCommandBuffer(command_buffer, imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {image_available_semaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &command_buffer;

        VkSemaphore signalSemaphores[] = {render_finished_semaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphics_queue, 1, &submitInfo, in_flight_fence) != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(present_queue, &presentInfo);
    }

    ~MentalWindowManager() {
        #if defined(VULKAN_RENDERER)
        if (this->logical_device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(logical_device);
            
            // Clean up synchronization objects
            if (image_available_semaphore != VK_NULL_HANDLE) {
                vkDestroySemaphore(logical_device, image_available_semaphore, nullptr);
            }
            if (render_finished_semaphore != VK_NULL_HANDLE) {
                vkDestroySemaphore(logical_device, render_finished_semaphore, nullptr);
            }
            if (in_flight_fence != VK_NULL_HANDLE) {
                vkDestroyFence(logical_device, in_flight_fence, nullptr);
            }
            
            // Clean up command pool
            if (command_pool != VK_NULL_HANDLE) {
                vkDestroyCommandPool(logical_device, command_pool, nullptr);
            }
            
            // Clean up framebuffers
            for (auto framebuffer : swapchain_framebuffers) {
                vkDestroyFramebuffer(logical_device, framebuffer, nullptr);
            }
            
            // Clean up pipeline
            if (graphics_pipeline != VK_NULL_HANDLE) {
                vkDestroyPipeline(logical_device, graphics_pipeline, nullptr);
            }
            if (pipeline_layout != VK_NULL_HANDLE) {
                vkDestroyPipelineLayout(logical_device, pipeline_layout, nullptr);
            }
            if (render_pass != VK_NULL_HANDLE) {
                vkDestroyRenderPass(logical_device, render_pass, nullptr);
            }
            
            // Clean up image views
            for (auto imageView : swapchain_image_views) {
                vkDestroyImageView(logical_device, imageView, nullptr);
            }
            
            // Clean up swapchain
            if (swapchain != VK_NULL_HANDLE) {
                vkDestroySwapchainKHR(logical_device, swapchain, nullptr);
            }
            
            vkDestroyDevice(this->logical_device, nullptr);
        }
        if (this->surface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(this->vk_instance, this->surface, nullptr);
        }
        if (this->vk_instance != VK_NULL_HANDLE) {
            vkDestroyInstance(this->vk_instance, nullptr);
        }
        #endif
        if (this->pWindow) glfwDestroyWindow(this->pWindow);
        glfwTerminate();
    }
};

int main()
{
    try {
        spdlog::info("Mental Engine starting...");
        MentalWindowManager wm;
        wm.run();
    } catch (const std::exception& e) {
        spdlog::error("Exception: {}", e.what());
        return -1;
    }
    
    spdlog::info("Mental Engine shutting down");
    return 0;
}