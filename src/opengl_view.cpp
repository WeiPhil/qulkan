#include "opengl_view.h"

#include <array>
#include <glm/glm.hpp>
#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "framework/compiler.hpp"
#include "framework/semantics.h"
#include "framework/vertex.h"
#include "qulkan/logger.h"
#include "utils/stb_image.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define ASSERT(boolean, message)                                                                                                                               \
    {                                                                                                                                                          \
        if (boolean != true) {                                                                                                                                 \
            std::cout << "[ASSERT FAILED] " << message << std::endl;                                                                                           \
            assert(false);                                                                                                                                     \
        }                                                                                                                                                      \
    }

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

    namespace buffer {
        enum type { VERTEX, ELEMENT, MAX };
    } // namespace buffer

    namespace texture {
        enum type { DIFFUSE, /*COLORBUFFER, RENDERBUFFER,*/ MAX };
    } // namespace texture

    namespace program {
        enum type { DEFAULT, FBO, MAX };
    } // namespace program

    namespace shader {
        enum type { VERT_DEFAULT, FRAG_DEFAULT, VERT_FBO, FRAG_FBO, MAX };
    } // namespace shader
} // namespace

OpenGLView::OpenGLView() : error(false), initialized(false) {
    programName.resize(program::MAX);
    vertexArrayName.resize(program::MAX);
    bufferName.resize(buffer::MAX);
    textureName.resize(texture::MAX);
}

void OpenGLView::initProgram() {
    std::array<GLuint, shader::MAX> shaderName;

    Compiler compiler;

    shaderName[shader::VERT_DEFAULT] = compiler.create(GL_VERTEX_SHADER, VERT_SHADER_SOURCE_DEFAULT, "--version 330 --profile core");
    shaderName[shader::FRAG_DEFAULT] = compiler.create(GL_FRAGMENT_SHADER, FRAG_SHADER_SOURCE_DEFAULT, "--version 330 --profile core");

    programName[program::DEFAULT] = glCreateProgram();
    glAttachShader(programName[program::DEFAULT], shaderName[shader::VERT_DEFAULT]);
    glAttachShader(programName[program::DEFAULT], shaderName[shader::FRAG_DEFAULT]);
    glLinkProgram(programName[program::DEFAULT]);

    error = error && compiler.check();
    error = error && compiler.check_program(programName[program::DEFAULT]);
    // Validated = Validated && Compiler.check_program(this->programName[program::SPLASH]);

    return;
}

void OpenGLView::initBuffer() {

    glGenBuffers(buffer::MAX, &bufferName[0]);

    // Create vertex array object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferName[buffer::ELEMENT]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementSize, elementData, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Create vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, bufferName[buffer::VERTEX]);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return;
}

void OpenGLView::initTexture() {

    glGenTextures(texture::MAX, &textureName[0]);
    glBindTexture(GL_TEXTURE_2D, textureName[texture::DIFFUSE]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *texData = stbi_load(TEXTURE_DIFFUSE, &width, &height, &nrChannels, 0);

    if (texData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        Qulkan::Logger::Error("Failed to load texture");
    }

    stbi_image_free(texData);

    return;
}

bool OpenGLView::initVertexArray() {
    glGenVertexArrays(program::MAX, &vertexArrayName[0]);
    glBindVertexArray(vertexArrayName[program::DEFAULT]);
    glBindBuffer(GL_ARRAY_BUFFER, bufferName[buffer::VERTEX]);
    glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v3fv3f), BUFFER_OFFSET(0));
    glVertexAttribPointer(semantic::attr::COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v3fv3f), BUFFER_OFFSET(sizeof(glm::vec3)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(semantic::attr::POSITION);
    glEnableVertexAttribArray(semantic::attr::COLOR);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferName[buffer::ELEMENT]);
    glBindVertexArray(0);

    return true;
}

void OpenGLView::init() {
    Qulkan::Logger::Info("OpenGLView: Initialisation\n");

    initProgram();
    initBuffer();
    initTexture();
    initVertexArray();
    if (!error) {
        Qulkan::Logger::Info("OpenGLView: Initialisation Done\n");
        initialized = true;
    } else
        Qulkan::Logger::Error("OpenGLView: An error Occured during initialisation\n");
}

void *OpenGLView::renderToTexture() { return (void *)(intptr_t)textureName[0]; }

/* Renders a simple OpenGL triangle in the rendering view */
void OpenGLView::render() {
    ASSERT(initialized, "OpenGLView: You need to init the view first");

    glUseProgram(programName[program::DEFAULT]);

    glBindVertexArray(vertexArrayName[program::DEFAULT]);

    glDrawElementsInstancedBaseVertex(GL_TRIANGLES, elementCount, GL_UNSIGNED_SHORT, 0, 1, 0);
    // glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}