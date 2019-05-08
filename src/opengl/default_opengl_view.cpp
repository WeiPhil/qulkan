#include "opengl/default_opengl_view.h"
#include "opengl/opengl_view.h"

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

#include "framework/texturemanager.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace {

    char const *VERT_SHADER_SOURCE_DEFAULT("../data/shaders/default.vert");
    char const *FRAG_SHADER_SOURCE_DEFAULT("../data/shaders/default.frag");
    char const *VERT_SHADER_SOURCE_FBO("../data/shaders/fbo.vert");
    char const *FRAG_SHADER_SOURCE_FBO("../data/shaders/fbo.frag");
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

    namespace texture {
        enum type { IMAGE, RENDERVIEW, /*RENDERBUFFER ,*/ MAX };
    } // namespace texture

    namespace program {
        enum type { DEFAULT, FBO, MAX };
    } // namespace program

    namespace shader {
        enum type { VERT_DEFAULT, FRAG_DEFAULT, VERT_FBO, FRAG_FBO, MAX };
    } // namespace shader
} // namespace

DefaultOpenGLView::DefaultOpenGLView(const char *viewName, int renderWidth, int renderHeight) : OpenGLView(viewName, renderWidth, renderHeight) {
    programNames.resize(program::MAX);
    vertexArrayNames.resize(program::MAX);
    bufferNames.resize(buffer::MAX);
    textureNames.resize(texture::MAX);
    framebufferNames.resize(framebuffer::MAX);
}

void DefaultOpenGLView::initProgram() {
    std::array<GLuint, shader::MAX> shaderName;

    Compiler compiler;

    shaderName[shader::VERT_DEFAULT] = compiler.create(GL_VERTEX_SHADER, VERT_SHADER_SOURCE_DEFAULT, "--version 330 --profile core");
    shaderName[shader::FRAG_DEFAULT] = compiler.create(GL_FRAGMENT_SHADER, FRAG_SHADER_SOURCE_DEFAULT, "--version 330 --profile core");

    programNames[program::DEFAULT] = glCreateProgram();
    glAttachShader(programNames[program::DEFAULT], shaderName[shader::VERT_DEFAULT]);
    glAttachShader(programNames[program::DEFAULT], shaderName[shader::FRAG_DEFAULT]);
    glLinkProgram(programNames[program::DEFAULT]);

    error = error && compiler.check();
    error = error && compiler.check_program(programNames[program::DEFAULT]);
    // Validated = Validated && Compiler.check_program(this-> programNames[program::SPLASH]);

    return;
}

void DefaultOpenGLView::initBuffer() {

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

void DefaultOpenGLView::initTexture() {

    glGenTextures(texture::MAX, &textureNames[0]);
    // Image texture
    glBindTexture(GL_TEXTURE_2D, textureNames[texture::IMAGE]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    int width, height, nrChannels;
    unsigned char *texData = stbi_load(TEXTURE_DIFFUSE, &width, &height, &nrChannels, 0);

    if (texData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        Qulkan::Logger::Error("Failed to load texture");
    }

    stbi_image_free(texData);

    // Render texture
    glBindTexture(GL_TEXTURE_2D, textureNames[texture::RENDERVIEW]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderWidth, renderHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return;
}

void DefaultOpenGLView::initVertexArray() {
    glGenVertexArrays(program::MAX, &vertexArrayNames[0]);
    glBindVertexArray(vertexArrayNames[program::DEFAULT]);
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

void DefaultOpenGLView::initFramebuffer() {

    // Create framebuffer and attach color texture
    glGenFramebuffers(framebuffer::MAX, &framebufferNames[framebuffer::RENDERVIEW]);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferNames[framebuffer::RENDERVIEW]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureNames[texture::RENDERVIEW], 0);

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

void DefaultOpenGLView::init() {
    Qulkan::Logger::Info("%s: Initialisation\n", name());

    initProgram();
    initBuffer();
    initTexture();
    initVertexArray();
    initFramebuffer();
    if (!error) {
        Qulkan::Logger::Info("%s: Initialisation Done\n", name());
        initialized = true;
    } else
        Qulkan::Logger::Error("%s: An error Occured during initialisation\n", name());
}

void DefaultOpenGLView::clean() {
    glDeleteFramebuffers(framebuffer::MAX, &framebufferNames[0]);
    glDeleteProgram(programNames[program::DEFAULT]);
    glDeleteProgram(programNames[program::FBO]);

    glDeleteBuffers(buffer::MAX, &bufferNames[0]);
    glDeleteTextures(texture::MAX, &textureNames[0]);
    glDeleteVertexArrays(program::MAX, &vertexArrayNames[0]);
}

/* Renders a simple OpenGL triangle in the rendering view */
ImTextureID DefaultOpenGLView::render() {
    ASSERT(initialized, std::string(name()) + ": You need to init the view first");

    glBindFramebuffer(GL_FRAMEBUFFER, framebufferNames[framebuffer::RENDERVIEW]);

    glViewport(0, 0, renderWidth, renderHeight);
    glClearColor(0.5f, 0.2f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
    glEnable(GL_DEPTH_TEST);

    glUseProgram(programNames[program::DEFAULT]);
    glBindVertexArray(vertexArrayNames[program::DEFAULT]);
    glDrawElementsInstancedBaseVertex(GL_TRIANGLES, elementCount, GL_UNSIGNED_SHORT, 0, 1, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return (ImTextureID)(intptr_t)textureNames[texture::RENDERVIEW];
}
