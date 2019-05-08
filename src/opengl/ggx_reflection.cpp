#include "opengl/ggx_reflection.h"

#include <array>
#include <glm/glm.hpp>
#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "framework/compiler.h"
#include "framework/semantics.h"
#include "framework/vertex.h"
#include "qulkan/logger.h"
#include "qulkan/utils.h"
#include "utils/stb_image.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace {

    char const *TEXTURE_DIFFUSE("../data/images/raw_vulkan.jpg");

    GLsizei const vertexCount(4);
    GLsizeiptr const vertexSize = vertexCount * sizeof(glf::vertex_v3fv3f);
    glf::vertex_v3fv3f const vertexData[vertexCount] = {
        // positions         // colors
        glf::vertex_v3fv3f(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),   // bottom right
        glf::vertex_v3fv3f(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),  // bottom left
        glf::vertex_v3fv3f(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)), // bottom left
        glf::vertex_v3fv3f(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(0.5f, 0.5f, 1.0f))   // top left
    };

    GLsizei const elementCount(6);
    GLsizeiptr const elementSize = elementCount * sizeof(GLushort);
    GLushort const elementData[elementCount] = {
        0, 1, 2, // triangle 1
        2, 3, 0  // triangle 2
    };

    namespace framebuffer {
        enum type { RENDERVIEW, MAX };
    } // namespace framebuffer

    namespace buffer {
        enum type { VERTEX, ELEMENT, MAX };
    } // namespace buffer
} // namespace

GGXReflection::GGXReflection(const char *viewName, int renderWidth, int renderHeight) : OpenGLView(viewName, renderWidth, renderHeight) {
    vertexArrayNames.resize(1);
    bufferNames.resize(buffer::MAX);
    framebufferNames.resize(framebuffer::MAX);

    textureManager.addTexture("IMAGE", "../data/images/raw_vulkan.jpg");
    textureManager.addTexture("RENDERVIEW");

    Handle transparency("Transparency", Type::FLOAT, 1.0f);
    Handle alphaU("AlphaU", Type::FLOAT, 0.5f);
    Handle alphaUV("AlphaUV", Type::VEC2, glm::vec2(0.1f, 0.2f));

    handleManager.addHandle(transparency);
    handleManager.addHandle(alphaU);
    handleManager.addHandle(alphaUV);
}

void GGXReflection::initProgram() {

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

void GGXReflection::initBuffer() {

    glGenBuffers(buffer::MAX, &bufferNames[0]);

    // Create vertex array object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferNames[buffer::ELEMENT]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementSize, elementData, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Create vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, bufferNames[buffer::VERTEX]);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return;
}

void GGXReflection::initTexture() {

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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        Qulkan::Logger::Error("Failed to load texture");
    }

    stbi_image_free(texData);

    // Render texture
    glBindTexture(GL_TEXTURE_2D, textureManager("RENDERVIEW"));

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWidth, renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return;
}

void GGXReflection::initVertexArray() {
    glGenVertexArrays(programManager.size(), &vertexArrayNames[0]);
    glBindVertexArray(vertexArrayNames[programManager("DEFAULT")]);
    glBindBuffer(GL_ARRAY_BUFFER, bufferNames[buffer::VERTEX]);
    glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v3fv3f), BUFFER_OFFSET(0));
    glVertexAttribPointer(semantic::attr::COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v3fv3f), BUFFER_OFFSET(sizeof(glm::vec3)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(semantic::attr::POSITION);
    glEnableVertexAttribArray(semantic::attr::COLOR);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferNames[buffer::ELEMENT]);
    glBindVertexArray(0);

    return;
}

void GGXReflection::initFramebuffer() {

    // Create framebuffer and attach color texture
    glGenFramebuffers(framebuffer::MAX, &framebufferNames[framebuffer::RENDERVIEW]);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferNames[framebuffer::RENDERVIEW]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureManager("RENDERVIEW"), 0);

    GLint dims[4] = {0};
    glGetIntegerv(GL_VIEWPORT, dims);
    GLint fbWidth = dims[2];
    GLint fbHeight = dims[3];

    // Create a renderbuffer for depth/stencil operation and attach it to the framebuffer
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fbWidth, fbHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        error = true;
        Qulkan::Logger::Error("FRAMEBUFFER:: Framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return;
}

void GGXReflection::init() {
    Qulkan::Logger::Info("DefaultOpenGLView: Initialisation\n");

    initProgram();
    initBuffer();
    initTexture();
    initVertexArray();
    initFramebuffer();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (!error) {
        Qulkan::Logger::Info("DefaultOpenGLView: Initialisation Done\n");
        initialized = true;
    } else
        Qulkan::Logger::Error("DefaultOpenGLView: An error Occured during initialisation\n");
}

void GGXReflection::clean() {
    glDeleteFramebuffers(framebuffer::MAX, &framebufferNames[0]);
    glDeleteProgram(programNames[programManager("DEFAULT")]);

    glDeleteBuffers(buffer::MAX, &bufferNames[0]);
    glDeleteTextures(textureManager.size(), &textureManager.textures[0]);
    glDeleteVertexArrays(programManager.size(), &vertexArrayNames[0]);
}

/* Renders a simple OpenGL triangle in the rendering view */
ImTextureID GGXReflection::render() {
    ASSERT(initialized, " DefaultOpenGLView: You need to init the view first");

    glBindFramebuffer(GL_FRAMEBUFFER, framebufferNames[framebuffer::RENDERVIEW]);

    glViewport(0, 0, renderWidth, renderHeight);

    glClearColor(0.5f, 0.3f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
    glEnable(GL_DEPTH_TEST);

    glUseProgram(programManager("DEFAULT"));

    glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "Transparency"), handleManager("Transparency")->getValue<float>());

    glBindVertexArray(vertexArrayNames[programManager("DEFAULT")]);
    glDrawElementsInstancedBaseVertex(GL_TRIANGLES, elementCount, GL_UNSIGNED_SHORT, 0, 1, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return (ImTextureID)(intptr_t)textureManager("RENDERVIEW");
}
