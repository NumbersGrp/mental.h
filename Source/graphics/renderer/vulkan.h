#ifndef mental_vulkan_renderer_h
#define mental_vulkan_renderer_h

#define VULKAN_RENDERER

#ifdef __APPLE__
    #define VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK ((VkStructureType)1000123000)
    typedef struct VkMacOSSurfaceCreateInfoMVK {
        VkStructureType    sType;
        const void*        pNext;
        VkFlags            flags;
        const void*        pView;
    } VkMacOSSurfaceCreateInfoMVK;

    typedef VkResult (VKAPI_PTR *PFN_vkCreateMacOSSurfaceMVK) (
        VkInstance                        instance,
        const VkMacOSSurfaceCreateInfoMVK* pCreateInfo,
        const VkAllocationCallbacks*      pAllocator,
        VkSurfaceKHR*                     pSurface);
#endif // __APPLE__

struct QueueFamilyIndex {
    uint32_t graphicsFamilyIndex;
    uint32_t presentFamilyIndex;
    VkBook32 graphicsSupport;
    VkBool32 presentSupport;
};

bool qfi_isComplete(QueueFamilyIndex* qfi) {
    return qfi->graphicsSupport && qfi->presentSupport;
}

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR* formats;
    VkPresentModeKHR* presentModes;
    
    uint32_t formats_count;
    uint32_t presentModes_count;
};





#endif // mental_vulkan_renderer_h