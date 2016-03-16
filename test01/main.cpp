
#include <iostream>
#include <string>
#include <vector>
#include <cstring>

#include <vulkan/vulkan.h>

namespace vulkan {

    struct Version {
        std::uint32_t major:10;
        std::uint32_t minor:10;
        std::uint32_t patch:12;
        
        explicit Version(std::uint32_t value) {
            std::memcpy(this, &value, sizeof(value));
        }
        
        friend std::ostream& operator<< (std::ostream &os, Version version) {
            os << version.major << "." << version.minor << "." << version.patch;
            return os;
        }
    };

    std::ostream& operator<< (std::ostream &os, VkResult result) {
        switch (result) {
            case VK_SUCCESS:                        os << "VK_SUCCESS"; break;
            case VK_ERROR_OUT_OF_HOST_MEMORY:       os << "VK_ERROR_OUT_OF_HOST_MEMORY"; break;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:     os << "VK_ERROR_OUT_OF_DEVICE_MEMORY"; break;
            case VK_ERROR_INITIALIZATION_FAILED:    os << "VK_ERROR_INITIALIZATION_FAILED"; break;
            case VK_ERROR_LAYER_NOT_PRESENT:        os << "VK_ERROR_LAYER_NOT_PRESENT"; break;
            case VK_ERROR_EXTENSION_NOT_PRESENT:    os << "VK_ERROR_EXTENSION_NOT_PRESENT"; break;
            case VK_ERROR_INCOMPATIBLE_DRIVER:      os << "VK_ERROR_INCOMPATIBLE_DRIVER"; break;
            default:                                os << "Unknown Error Code"; break;
        }
        
        return os;
    }
    
    std::ostream& operator<< (std::ostream &os, VkPhysicalDeviceType deviceType) {
    
        switch (deviceType) {
            case VK_PHYSICAL_DEVICE_TYPE_CPU: os << "VK_PHYSICAL_DEVICE_TYPE_CPU"; break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: os << "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU"; break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: os << "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU"; break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: os << "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU"; break;
            case VK_PHYSICAL_DEVICE_TYPE_OTHER: os << "VK_PHYSICAL_DEVICE_TYPE_OTHER"; break;
            default: os << "Unknown Device Type"; break;
        }
        
        return os;
    }
    
    std::ostream& write_stream (std::ostream &os, VkQueueFlags flags) {
    
        if (flags&VK_QUEUE_GRAPHICS_BIT) {
            os << "VK_QUEUE_GRAPHICS_BIT ";
        }
        
        if (flags&VK_QUEUE_COMPUTE_BIT) {
            os << "VK_QUEUE_COMPUTE_BIT ";
        }
        
        if (flags&VK_QUEUE_TRANSFER_BIT) {
            os << "VK_QUEUE_TRANSFER_BIT ";
        }
        
        if (flags&VK_QUEUE_SPARSE_BINDING_BIT) {
            os << "VK_QUEUE_SPARSE_BINDING_BIT";
        }
        
        return os;
    }
}

int main(int argc, char *argv[]) {

    using namespace vulkan;

    VkInstance instance;
    VkInstanceCreateInfo info = {};
    
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    
    VkResult result;
    
    result = ::vkCreateInstance(&info, nullptr, &instance);
    
    uint32_t deviceCount;
    
    std::vector<VkPhysicalDevice> devices;
    
    result = ::vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    
    if (deviceCount == 0) {
        std::cout << "No Vulkan devices found!" << std::endl;
        
    } else {
        devices.resize(deviceCount);
        result = ::vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
        
        std::cout << "Found " << deviceCount << " Vulkan device(s):" << std::endl;
        
        for (VkPhysicalDevice device : devices) {
        
            // display device properties
            VkPhysicalDeviceProperties deviceProperties;
            
            ::vkGetPhysicalDeviceProperties(device, &deviceProperties);
            
            std::cout << "  API version: "          << vulkan::Version(deviceProperties.apiVersion) << std::endl;
            std::cout << "  Driver version: "       << deviceProperties.driverVersion << std::endl;
            std::cout << "  Vendor ID: "            << deviceProperties.vendorID << std::endl;
            std::cout << "  Device ID: "            << deviceProperties.deviceID << std::endl;
            std::cout << "  Device Type: "          << deviceProperties.deviceType << std::endl;
            std::cout << "  Device Name: "          << deviceProperties.deviceName << std::endl;
            std::cout << "  Pipeline Cache UUID: "  << deviceProperties.pipelineCacheUUID << std::endl;
            
            // display queue family properties
            std::vector<VkQueueFamilyProperties> families;
            std::uint32_t familyCount;
            
            ::vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
            
            families.resize(familyCount);
            
            if (familyCount > 0) {
                ::vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, families.data());
            }
            
            for (VkQueueFamilyProperties properties : families) {
                std::cout << std::endl;
                std::cout << "  Queue count: "          << properties.queueCount            << std::endl;
                std::cout << "  Timestamp Valid Bits: " << properties.timestampValidBits    << std::endl;
                
                std::cout << "  Queue Flags: ";
                write_stream(std::cout, properties.queueFlags); 
                std::cout << std::endl;
            }
        }
    }
    
    ::vkDestroyInstance(instance, nullptr);
    
    return 0;
}
