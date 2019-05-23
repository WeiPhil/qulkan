#include "opengl/examples/basic/hellotriangle.h"

#include <array>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "framework/compiler.h"
#include "framework/semantics.h"
#include "framework/vertex.h"
#include "qulkan/logger.h"
#include "qulkan/utils.h"
#include "utils/stb_image.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

HelloTriangle::HelloTriangle(const char *viewName, int initialRenderWidth, int initialRenderHeight)
    : Qulkan::RenderView(viewName, initialRenderWidth, initialRenderHeight) {
    vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));   // top
    vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));  // bottom right
    vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))); // bottom left

    eboManager.addTriangle(0, 1, 2);
}

void HelloTriangle::initHandles() {

    Handle transparency("Transparency", Type::FLOAT, 1.0f);

    handleManager.addHandle(transparency);
}

void HelloTriangle::initProgram() {

    Compiler compiler;

    shaderManager.addShader("VERT_DEFAULT", "../data/shaders/default.vert", GL_VERTEX_SHADER, compiler);
    shaderManager.addShader("FRAG_DEFAULT", "../data/shaders/default.frag", GL_FRAGMENT_SHADER, compiler);

    programManager.addProgram("DEFAULT");

    glAttachShader(programManager("DEFAULT"), shaderManager("VERT_DEFAULT"));
    glAttachShader(programManager("DEFAULT"), shaderManager("FRAG_DEFAULT"));
    glLinkProgram(programManager("DEFAULT"));

    error = compiler.check() && error;
    error = compiler.check_program(programManager("DEFAULT")) && error;

    return;
}

void HelloTriangle::initBuffer() {

    bufferManager.addBuffer("ELEMENT");
    bufferManager.addBuffer("VERTEX");

    glGenBuffers(bufferManager.size(), &bufferManager.buffers[0]);

    // Create vertex array object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferManager("ELEMENT"));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboManager.getElementSize(), &eboManager.elementData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Create vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX"));
    glBufferData(GL_ARRAY_BUFFER, vaoManager.getVertexDataSize(), &vaoManager.vertexData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return;
}

void HelloTriangle::initTexture() {

    textureManager.addTexture("IMAGE", "../data/images/raw_vulkan.jpg");

    glGenTextures(textureManager.size(), &textureManager.textures[0]);
    // Image texture
    glBindTexture(GL_TEXTURE_2D, textureManager("IMAGE"));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    int width, height, nrChannels;
    unsigned char *texData = stbi_load(textureManager.texturePath("IMAGE"), &width, &height, &nrChannels, 0);

    if (texData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        Qulkan::Logger::Error("Failed to load texture");
    }

    stbi_image_free(texData);

    return;
}

void HelloTriangle::initVertexArray() {
    glGenVertexArrays(1, &vaoManager.id);
    glBindVertexArray(vaoManager.id);
    glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX"));

    glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, vaoManager.getVertexSize(), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(semantic::attr::POSITION);

    glVertexAttribPointer(semantic::attr::COLOR, 3, GL_FLOAT, GL_FALSE, vaoManager.getVertexSize(), BUFFER_OFFSET(sizeof(glm::vec3)));
    glEnableVertexAttribArray(semantic::attr::COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Bin element buffer array to array ob
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferManager("ELEMENT"));
    glBindVertexArray(0);

    return;
}

void HelloTriangle::initFramebuffer() { return; }

void HelloTriangle::init() {
    Qulkan::Logger::Info("%s: Initialisation\n", name());

    initHandles();
    initProgram();
    initBuffer();
    initTexture();
    initVertexArray();
    initFramebuffer();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (!error) {
        Qulkan::Logger::Info("%s: Initialisation Done\n", name());
        initialized = true;
    } else
        Qulkan::Logger::Error("%s: An error Occured during initialisation\n", name());
}

void HelloTriangle::initOpenGLOptions() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void HelloTriangle::clean() {
    glDeleteFramebuffers(framebufferManager.size(), &framebufferManager.framebuffers[0]);
    glDeleteProgram(programManager("DEFAULT"));

    glDeleteBuffers(bufferManager.size(), &bufferManager.buffers[0]);
    glDeleteTextures(textureManager.size(), &textureManager.textures[0]);
    glDeleteVertexArrays(1, &vaoManager.id);
}

/* Renders a simple OpenGL triangle in the rendering view */
void HelloTriangle::render(int actualRenderWidth, int actualRenderHeight) {
    ASSERT(initialized, std::string(name()) + ": You need to init the view first");

    glViewport(0, 0, actualRenderWidth, actualRenderHeight);

    glClearColor(0.233f, 0.233f, 0.233f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
    glEnable(GL_DEPTH_TEST);

    glUseProgram(programManager("DEFAULT"));

    glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "Transparency"), handleManager("Transparency")->getValue<float>());

    glBindVertexArray(vaoManager.id);
    glDrawElementsInstancedBaseVertex(GL_TRIANGLES, eboManager.getElementCount(), GL_UNSIGNED_INT, 0, 1, 0);

    return;
}
