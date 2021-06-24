# Qulkan : A GPU Oriented Prototyping tool

Qulkan is tool created while thinking about all the different ways people are trying to get into the real time rendering world. There exists many tutorials for OpenGL and much less for starting Vulkan, this tool is here to give simple implementation examples using both Vulkan or OpenGl as a computer graphics and computation API. 
Qulkan isn't restricted in it's possibilities since it aims at providing an intuitive and simple Grafical User Interface permitting a versatile implementation of more or less abstract concept. What we mean by this is that it could be as good at prototyping some simple new model as making a game from scratch.
Qulkan is built upon the Imgui library and abstracts further the infinite possibilities of it to permit a new project to be prototyped in just a few minutes. It allows to create multiple windows that work independently and without any need of creating separate framebuffers. Qulkan also permits to create new handles that can be customised and used as quick way to interact with one's implementation. Finally the creation of some "ShaderToy" like shaders can also be easilly done and we permit on-fly recompilation of those shaders, hence Qulkan can also be used to rapidly test some simple shaders.

## Compiling

Compiling Qulkan is rather straight forward for anyone used to cmake based project. 
On Unix-like systems you can clone and build Qulkan in the following way using either Clang or Gcc

```bash
git clone --recursive https://github.com/WeiPhil/qulkan
cd qulkan

# Create an out of tree build directory
mkdir build && cd build
# Configure the project and build it in parralel
cmake ..
make -j
```

TODO? 
On Windows, open the generated ``qulkan.sln`` file after running
``cmake ..`` and proceed building as usual from within Visual Studio.

## Dependencies

If you want to work with vulkan you will need to download the official library which aren't included in the submodules of Qulkan.
You may also need the additional library libpng if not already available on your system.

## About

This project was created by Lucas Ramirez and Philippe Weier. 
Don't hesitate to post any issue or suggestions or directly contact us at philippe.weier@epfl.ch and lucas.ramirez@epfl.ch




