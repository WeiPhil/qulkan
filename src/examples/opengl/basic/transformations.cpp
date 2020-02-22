#include "examples/opengl/basic/transformations.h"

#include <array>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "framework/opengl/compiler.h"
#include "framework/opengl/semantics.h"
#include "framework/opengl/vertex.h"
#include "qulkan/logger.h"
#include "qulkan/utils.h"
#include "utils/stb_image.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace OpenGLExamples {

    Transformations::Transformations(const char *viewName, int initialRenderWidth, int initialRenderHeight)
        : Qulkan::RenderView(viewName, initialRenderWidth, initialRenderHeight) {
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)));   // top right
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)));  // top left
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f))); // bottom left
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)));  // bottom right

        eboManager.addTriangle(0, 1, 2);
        eboManager.addTriangle(2, 3, 0);
    }

    void Transformations::initHandles() {

        Handle mix("Mix", Type::FLOAT_HANDLE, 0.5f);

        handleManager.addHandle(mix);
    }

    void Transformations::initProgram() {

        Compiler compiler;

        shaderManager.addShader("VERT_DEFAULT", "../data/shaders/examples/basic/transformations.vert", GL_VERTEX_SHADER, compiler);
        shaderManager.addShader("FRAG_DEFAULT", "../data/shaders/examples/basic/transformations.frag", GL_FRAGMENT_SHADER, compiler);

        programManager.addProgram("DEFAULT");

        glAttachShader(programManager("DEFAULT"), shaderManager("VERT_DEFAULT"));
        glAttachShader(programManager("DEFAULT"), shaderManager("FRAG_DEFAULT"));
        glLinkProgram(programManager("DEFAULT"));

        error = compiler.check() && error;
        error = compiler.check_program(programManager("DEFAULT")) && error;

        return;
    }

    void Transformations::initBuffer() {

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

    void Transformations::initTexture() {

        textureManager.addTexture("IMAGE_VULKAN1", "../data/images/raw_vulkan.jpg");
        textureManager.addTexture("IMAGE_VULKAN2", "../data/images/vulkan_mountain.png");

        glGenTextures(textureManager.size(), &textureManager.textures[0]);
        // Image texture 1
        glBindTexture(GL_TEXTURE_2D, textureManager("IMAGE_VULKAN1"));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load first image
        int width, height, nrChannels;
        unsigned char *texData1 = stbi_load(textureManager.texturePath("IMAGE_VULKAN1"), &width, &height, &nrChannels, 0);

        if (texData1) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData1);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            Qulkan::Logger::Error("Failed to load texture 1");
        }
        stbi_image_free(texData1);

        // Image texture 2
        glBindTexture(GL_TEXTURE_2D, textureManager("IMAGE_VULKAN2"));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(glm::vec4(0.0)));

        // Load second image (RGBA)
        unsigned char *texData2 = stbi_load(textureManager.texturePath("IMAGE_VULKAN2"), &width, &height, &nrChannels, 0);

        if (texData2) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData2);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            Qulkan::Logger::Error("Failed to load texture 2");
        }
        stbi_image_free(texData2);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Bind to programm
        glUseProgram(programManager("DEFAULT"));
        glUniform1i(glGetUniformLocation(programManager("DEFAULT"), "texture1"), 0);
        glUniform1i(glGetUniformLocation(programManager("DEFAULT"), "texture2"), 1);

        return;
    }

    void Transformations::initVertexArray() {
        glGenVertexArrays(1, &vaoManager.id);
        glBindVertexArray(vaoManager.id);
        glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX"));

        glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, vaoManager.getVertexSize(), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(semantic::attr::POSITION);

        glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, vaoManager.getVertexSize(), BUFFER_OFFSET(sizeof(glm::vec3)));
        glEnableVertexAttribArray(semantic::attr::TEXCOORD);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Bin element buffer array to array ob
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferManager("ELEMENT"));
        glBindVertexArray(0);

        return;
    }

    void Transformations::initFramebuffer() { return; }

    void Transformations::init() {
        Qulkan::Logger::Info("%s: Initialisation\n", name());

        if (handleManager.getHandles().empty())
            initHandles();
        initProgram();
        initBuffer();
        initTexture();
        initVertexArray();
        initFramebuffer();
        initOpenGLOptions();
        if (!error) {
            Qulkan::Logger::Info("%s: Initialisation Done\n", name());
            initialized = true;
        } else
            Qulkan::Logger::Error("%s: An error Occured during initialisation\n", name());
    }

    void Transformations::initOpenGLOptions() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Transformations::clean() {
        glDeleteFramebuffers(framebufferManager.size(), &framebufferManager.framebuffers[0]);
        glDeleteProgram(programManager("DEFAULT"));

        glDeleteBuffers(bufferManager.size(), &bufferManager.buffers[0]);
        glDeleteTextures(textureManager.size(), &textureManager.textures[0]);
        glDeleteVertexArrays(1, &vaoManager.id);
    }

    /* Renders a simple OpenGL triangle in the rendering view */
    void Transformations::render(int actualRenderWidth, int actualRenderHeight) {
        ASSERT(initialized, std::string(name()) + ": You need to init the view first");

        glViewport(0, 0, actualRenderWidth, actualRenderHeight);

        glClearColor(0.233f, 0.233f, 0.233f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glUseProgram(programManager("DEFAULT"));

        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::scale(trans, glm::vec3(0.75f, 0.75f, 0.0f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        trans = glm::rotate(trans, (float)glfwGetTime() * 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureManager("IMAGE_VULKAN1"));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureManager("IMAGE_VULKAN2"));

        glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "Mix"), handleManager("Mix")->getValue<float>());

        glUniformMatrix4fv(glGetUniformLocation(programManager("DEFAULT"), "transform"), 1, GL_FALSE, glm::value_ptr(trans));

        glBindVertexArray(vaoManager.id);
        glDrawElementsInstancedBaseVertex(GL_TRIANGLES, eboManager.getElementCount(), GL_UNSIGNED_INT, 0, 1, 0);

        return;
    }

} // namespace OpenGLExamples