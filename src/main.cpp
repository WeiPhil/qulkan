#include <string.h>
#include <iostream>

int main_opengl3();
int main_vulkan();

int main(int argc, char* argv[]) {
    if (argc == 2 && strcmp(argv[1], "--vulkan") == 0) {
        std::cout << "Using Vulkan" << std::endl;
        // main_vulkan();
    } else {
        std::cout << "Using OpenGL3" << std::endl;
        main_opengl3();
    }
}