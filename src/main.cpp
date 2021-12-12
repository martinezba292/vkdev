#include "vkloader.h"
#include "vkinstance.h"
#include "vkdevice.h"
#include <iostream>


int main() {
    std::cout << "Hello World" << std::endl;
    vkdev::PrepareVulkan();
    vkdev::Instance app_instance("VulkanTest");
    vkdev::Device app_device(app_instance.get());
    
    
return 0;
}
