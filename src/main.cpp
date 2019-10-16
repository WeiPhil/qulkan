#include <string.h>
#include <iostream>

int main_opengl3();
#if defined(GLFW_INCLUDE_VULKAN)
int main_vulkan();
#endif

int main(int argc, char* argv[]) {
    if (argc == 2 && strcmp(argv[1], "--vulkan") == 0) {
        #if defined(GLFW_INCLUDE_VULKAN)
        std::cout << "Using Vulkan" << std::endl;
        return main_vulkan();
        #else
        std::cout << "Not compiled to support Vulkan" << std::endl;
        #endif
    } else {
        std::cout << "Using OpenGL3" << std::endl;
        return main_opengl3();
    }
}