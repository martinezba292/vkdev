#include "vkloader.h"
#include "vkinstance.h"
#include "vkdevice.h"
#include "window.h"
#include <iostream>


int main() {
    std::cout << "Hello World" << std::endl;
    vkdev::PrepareVulkan();
    vkdev::Instance app_instance("VulkanTest");
    vkdev::Window app_window(app_instance);
    app_window.createWindow(800, 600);
    vkdev::Device app_device(app_instance, app_window);

    int j = 0;
    while(!app_window.windowShouldClose()) {
        j++;
        std::cout << j << std::endl;
    }
    
    app_window.~Window();
    app_device.~Device();
    app_instance.~Instance();
    vkdev::CloseVulkan();

return 0;
}
