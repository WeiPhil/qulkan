#include "opengl/ggx_reflection.h"

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

namespace {

    GLsizei const vertexCount(4);
    GLsizeiptr const vertexSize = vertexCount * sizeof(glf::vertex_v3fv2f);
    glf::vertex_v3fv2f const vertexData[vertexCount] = {
        // positions         // texcoord
        glf::vertex_v3fv2f(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),   // top right
        glf::vertex_v3fv2f(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),  // bottom left
        glf::vertex_v3fv2f(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)), // bottom left
        glf::vertex_v3fv2f(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f))   // top left
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

    Handle wi("wi", Type::VEC2, glm::vec2(0.0f, 0.f));
    Handle u_alpha_x_1("u_alpha_x_1", Type::FLOAT, 0.1f);
    Handle u_alpha_y_1("u_alpha_y_1", Type::FLOAT, 0.1f);
    Handle u_Scale("u_Scale", Type::FLOAT, 1.0f);
    Handle u_Gamma("u_Gamma", Type::FLOAT, 1.0f);

    handleManager.addHandle(wi);
    handleManager.addHandle(u_alpha_x_1);
    handleManager.addHandle(u_alpha_y_1);
    handleManager.addHandle(u_Scale);
    handleManager.addHandle(u_Gamma);
}

void GGXReflection::initProgram() {

    Compiler compiler;

    shaderManager.addShader("VERT_DEFAULT", "../data/shaders/default.vert", GL_VERTEX_SHADER, compiler);
    shaderManager.addShader("FRAG_DEFAULT", "../data/shaders/gt_reflection_aniso.frag", GL_FRAGMENT_SHADER, compiler);

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

    std::cout << textureManager.texturePath("IMAGE") << std::endl;
    int width, height, nrChannels;
    unsigned char *texData = stbi_load(textureManager.texturePath("IMAGE"), &width, &height, &nrChannels, 0);

    if (texData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
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
    glBindVertexArray(vertexArrayNames[0]);
    glBindBuffer(GL_ARRAY_BUFFER, bufferNames[buffer::VERTEX]);

    glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v3fv2f), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(semantic::attr::POSITION);

    glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v3fv2f), BUFFER_OFFSET(sizeof(glm::vec3)));
    glEnableVertexAttribArray(semantic::attr::TEXCOORD);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferNames[buffer::ELEMENT]);
    glBindVertexArray(0);

    return;
}

void GGXReflection::initFramebuffer() {

    // Create framebuffer and attach color texture
    glGenFramebuffers(framebuffer::MAX, &framebufferNames[framebuffer::RENDERVIEW]);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferNames[framebuffer::RENDERVIEW]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureManager("RENDERVIEW"), 0);

    // Create a renderbuffer for depth/stencil operation and attach it to the framebuffer
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, renderWidth, renderHeight);
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
    Qulkan::Logger::Info("%s: Initialisation\n", name());

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

void GGXReflection::clean() {
    glDeleteFramebuffers(framebuffer::MAX, &framebufferNames[0]);
    glDeleteProgram(programManager("DEFAULT"));

    glDeleteBuffers(buffer::MAX, &bufferNames[0]);
    glDeleteTextures(textureManager.size(), &textureManager.textures[0]);
    glDeleteVertexArrays(programManager.size(), &vertexArrayNames[0]);
}

/* Renders a simple OpenGL triangle in the rendering view */
ImTextureID GGXReflection::render() {
    ASSERT(initialized, std::string(name()) + ": You need to init the view first");

    glBindFramebuffer(GL_FRAMEBUFFER, framebufferNames[framebuffer::RENDERVIEW]);

    glViewport(0, 0, renderWidth, renderHeight);

    glClearColor(0.06f, 0.06f, 0.06f, 0.94f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
    glEnable(GL_DEPTH_TEST);

    glUseProgram(programManager("DEFAULT"));

    glm::vec3 wi = glm::normalize(glm::vec3(handleManager("wi")->getValue<glm::vec2>(), 1.0f));

    glUniform3f(glGetUniformLocation(programManager("DEFAULT"), "wi"), wi.x, wi.y, wi.z);
    glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "u_alpha_x_1"), handleManager("u_alpha_x_1")->getValue<float>());
    glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "u_alpha_y_1"), handleManager("u_alpha_y_1")->getValue<float>());
    glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "u_Scale"), handleManager("u_Scale")->getValue<float>());
    glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "u_Gamma"), handleManager("u_Gamma")->getValue<float>());

    glBindVertexArray(vertexArrayNames[0]);
    glDrawElementsInstancedBaseVertex(GL_TRIANGLES, elementCount, GL_UNSIGNED_SHORT, 0, 1, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return (ImTextureID)(intptr_t)textureManager("RENDERVIEW");
}
