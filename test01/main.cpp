
#include <iostream>
#include <string>
#include <vector>
#include <cstring>

#include <vulkan/vulkan.h>

namespace vulkan {

    // struct to 'parse' the vulkan version specification
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
            default:                                os << "Unknown Error Code :" << result; break;
        }
        
        return os;
    }
    
    std::ostream& operator<< (std::ostream &os, VkPhysicalDeviceType deviceType) {
    
        switch (deviceType) {
            case VK_PHYSICAL_DEVICE_TYPE_CPU:               os << "VK_PHYSICAL_DEVICE_TYPE_CPU"; break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:      os << "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU"; break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:    os << "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU"; break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:       os << "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU"; break;
            case VK_PHYSICAL_DEVICE_TYPE_OTHER:             os << "VK_PHYSICAL_DEVICE_TYPE_OTHER"; break;
            default:                                        os << "Unknown Device Type: " << deviceType; break;
        }
        
        return os;
    }
    
    std::ostream& write_stream (std::ostream &os, VkQueueFlags flags) {
        // VkQueueFlags conflicts with uint32_t, so we cannot 
        // define a new operator<< function overload.
    
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

// import operator<< 
using namespace vulkan; 

class Test01App {
public:
    void run() {
        bool running = initialize();
        
        if (running) {
            for (int i=0; i<120; i++) {
                render();
            }
        }
        
        terminate();
    }

protected:
    bool initialize() {
    
        // create a vulkan instance.
        VkInstanceCreateInfo info = {};
        
        info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        
        VkResult result;
        
        result = ::vkCreateInstance(&info, nullptr, &instance);
        
        // get available physical devices in the created instance
        uint32_t deviceCount;
        
        std::vector<VkPhysicalDevice> devices;
        
        result = ::vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        
        if (deviceCount > 0) {
            devices.resize(deviceCount);
            result = ::vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
        } 
        
        std::cout << "Found " << deviceCount << " Vulkan device(s)";
        if (deviceCount > 0) {
            std::cout << ":" << std::endl;
        }
        
        // display the information on all available physical devices
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
            
            // get available queue family properties
            std::vector<VkQueueFamilyProperties> families;
            std::uint32_t familyCount;
            
            ::vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
            
            if (familyCount > 0) {
                families.resize(familyCount);
                ::vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, families.data());
            }
            
            // display queue family properties
            for (VkQueueFamilyProperties properties : families) {
                std::cout << std::endl;
                std::cout << "  Queue count: "          << properties.queueCount            << std::endl;
                std::cout << "  Timestamp Valid Bits: " << properties.timestampValidBits    << std::endl;
                
                std::cout << "  Queue Flags: ";
                write_stream(std::cout, properties.queueFlags); 
                std::cout << std::endl;
            }
        }
        
        // create a logical device from the first physical device
        const float priorities[] = {1.0f};
        
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.queueFamilyIndex = 0;
        queueCreateInfo.pQueuePriorities = priorities;
        
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        
        result = ::vkCreateDevice(devices[0], &createInfo, nullptr, &device);
        std::cout << "CreateDevice result: " << result << std::endl;
        
        // create a single command pool
        VkCommandPoolCreateInfo poolCreateInfo = {};
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolCreateInfo.queueFamilyIndex = 0;
        
        result = ::vkCreateCommandPool(device, &poolCreateInfo, nullptr, &commandPool);
        std::cout << "CreateCommandPool result: " << result << std::endl;

        // create the primary command buffer
        VkCommandBufferAllocateInfo cmdAllocateInfo = {};
        cmdAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdAllocateInfo.commandPool = commandPool;
        cmdAllocateInfo.commandBufferCount = 1;
        cmdAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        
        result = ::vkAllocateCommandBuffers(device, &cmdAllocateInfo, &commandBuffers[0]);

        // create the secondary command buffer
        // cmdAllocateInfo.commandPool = commandPool;
        // cmdAllocateInfo.commandBufferCount = 1;
        // cmdAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        // result = ::vkAllocateCommandBuffers(device, &cmdAllocateInfo, &commandBuffers[1]);

        // start the recording of the first command buffer
        VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
        cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        
        result = ::vkBeginCommandBuffer(commandBuffers[0], &cmdBufferBeginInfo);
        
        // end the recording of the second command buffer
        ::vkEndCommandBuffer(commandBuffers[0]);
        
        // get the single queue created along the logical device
        ::vkGetDeviceQueue(device, 0, 0, &queue);
        
        return true;
    }
    
    void render() {    
        VkResult errorCode = ::vkQueueWaitIdle(queue);
        
        if (errorCode) {
            std::cout << "QueueWaitIdle result: " << errorCode << std::endl;
        }
    }
    
    void terminate() {
        // free the command buffers
        ::vkFreeCommandBuffers(device, commandPool, 1, commandBuffers);
        
        // destroy the command pool
        if (commandPool) {
            ::vkDestroyCommandPool(device, commandPool, nullptr);
            commandPool = nullptr;
        }
        
        // destroy the device
        if (device) {
            ::vkDestroyDevice(device, nullptr);
            device = nullptr;
        }
        
        // finalize the vulkan instance
        if (instance) {
            ::vkDestroyInstance(instance, nullptr);
            instance = nullptr;
        }
    }
    
private:
    VkInstance instance;
    VkDevice device;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffers[2];
    VkQueue queue;
};

int main(int argc, char *argv[]) {
    
    Test01App app;
    
    app.run();
    
    return 0;
}
