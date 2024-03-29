#include "examples/opengl/basic/camera.h"

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

    void Camera::createCube() {

        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)));

        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f)));

        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));

        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));

        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));

        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));
    }

    Camera::Camera(const char *viewName, int initialRenderWidth, int initialRenderHeight)
        : Qulkan::RenderView(viewName, initialRenderWidth, initialRenderHeight) {
        createCube();
        cubePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        cubePositions.push_back(glm::vec3(2.0f, 5.0f, -15.0f));
        cubePositions.push_back(glm::vec3(-1.5f, -2.2f, -2.5f));
        cubePositions.push_back(glm::vec3(-3.8f, -2.0f, -12.3f));
        cubePositions.push_back(glm::vec3(2.4f, -0.4f, -3.5f));
        cubePositions.push_back(glm::vec3(-1.7f, 3.0f, -7.5f));
        cubePositions.push_back(glm::vec3(1.3f, -2.0f, -2.5f));
        cubePositions.push_back(glm::vec3(1.5f, 2.0f, -2.5f));
        cubePositions.push_back(glm::vec3(1.5f, 0.2f, -1.5f));
        cubePositions.push_back(glm::vec3(-1.3f, 1.0f, -1.5f));
    }

    void Camera::initHandles() {

        Handle mix("Mix", Type::FLOAT_HANDLE, 0.5f);
        handleManager.addHandle(mix);

        Handle fov("FoV", Type::FLOAT_HANDLE, 45.0f, 10.0f, 180.0f);
        handleManager.addHandle(fov);

        Handle nearPlane("Near Plane", Type::FLOAT_HANDLE, 0.1f, 0.0001f, 50.0f);
        handleManager.addHandle(nearPlane);
        Handle farPlane("Far Plane", Type::FLOAT_HANDLE, 100.0f, 0.0001f, 1000.0f);
        handleManager.addHandle(farPlane);

        Handle cameraSpeed("Camera Speed", Type::FLOAT_HANDLE, 4.5f, 0.01f, 20.0f);
        handleManager.addHandle(cameraSpeed);
        Handle cameraSensitivity("Camera Sensitivity", Type::FLOAT_HANDLE, 0.1f, 0.001f, 1.0f);
        handleManager.addHandle(cameraSensitivity);
    }

    void Camera::initProgram() {

        Compiler compiler;

        shaderManager.addShader("VERT_DEFAULT", "../data/shaders/examples/basic/coordinatesystems.vert", GL_VERTEX_SHADER, compiler);
        shaderManager.addShader("FRAG_DEFAULT", "../data/shaders/examples/basic/coordinatesystems.frag", GL_FRAGMENT_SHADER, compiler);

        programManager.addProgram("DEFAULT");

        glAttachShader(programManager("DEFAULT"), shaderManager("VERT_DEFAULT"));
        glAttachShader(programManager("DEFAULT"), shaderManager("FRAG_DEFAULT"));
        glLinkProgram(programManager("DEFAULT"));

        error = compiler.check() && error;
        error = compiler.check_program(programManager("DEFAULT")) && error;

        return;
    }

    void Camera::initBuffer() {

        bufferManager.addBuffer("VERTEX");

        glGenBuffers(bufferManager.size(), &bufferManager.buffers[0]);

        // Create vertex array object
        glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX"));
        glBufferData(GL_ARRAY_BUFFER, vaoManager.getVertexDataSize(), &vaoManager.vertexData[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return;
    }

    void Camera::initTexture() {

        textureManager.addTexture("IMAGE_VULKAN1", "../data/images/raw_vulkan.jpg");
        textureManager.addTexture("IMAGE_VULKAN2", "../data/images/vulkan_mountain.png");
        textureManager.addTexture("RENDERVIEW");

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

    void Camera::initVertexArray() {
        glGenVertexArrays(1, &vaoManager.id);
        glBindVertexArray(vaoManager.id);
        glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX"));

        glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, vaoManager.getVertexSize(), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(semantic::attr::POSITION);

        glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, vaoManager.getVertexSize(), BUFFER_OFFSET(sizeof(glm::vec3)));
        glEnableVertexAttribArray(semantic::attr::TEXCOORD);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        return;
    }

    void Camera::initFramebuffer() { return; }

    void Camera::init() {
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

    void Camera::initOpenGLOptions() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
    }

    void Camera::clean() {
        glDeleteFramebuffers(framebufferManager.size(), &framebufferManager.framebuffers[0]);
        glDeleteProgram(programManager("DEFAULT"));

        glDeleteBuffers(bufferManager.size(), &bufferManager.buffers[0]);
        glDeleteTextures(textureManager.size(), &textureManager.textures[0]);
        glDeleteVertexArrays(1, &vaoManager.id);
    }

    /* Renders a simple OpenGL triangle in the rendering view */
    void Camera::render(int actualRenderWidth, int actualRenderHeight) {
        ASSERT(initialized, std::string(name()) + ": You need to init the view first");

        glViewport(0, 0, actualRenderWidth, actualRenderHeight);

        glClearColor(0.233f, 0.233f, 0.233f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Camera settings
        float radius = 10.0f;

        float sensitivity = handleManager("Camera Sensitivity")->getValue<float>();
        if (isMouseDragging(0)) {
            yaw += mouseDelta.x * sensitivity * Qulkan::getDeltaTime();
            pitch += -mouseDelta.y * sensitivity * Qulkan::getDeltaTime();

            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            glm::vec3 front;
            front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
            front.y = sin(glm::radians(pitch));
            front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
            cameraFront = glm::normalize(front);
        }

        if (mouseWheel != 0.0f) {
            auto fov = std::any_cast<float>(&handleManager("FoV")->value);
            if (*fov >= 1.0f && *fov <= 45.0f)
                *fov -= mouseWheel;
            if (*fov <= 1.0f)
                *fov = 1.0f;
            if (*fov >= 45.0f)
                *fov = 45.0f;
        }

        float cameraSpeed = handleManager("Camera Speed")->getValue<float>() * Qulkan::getDeltaTime();
        if (isKeyDown(GLFW_KEY_LEFT_SHIFT))
            cameraSpeed = cameraSpeed * 3.0f;
        if (isKeyDown(GLFW_KEY_W))
            cameraPos += cameraSpeed * cameraFront;
        if (isKeyDown(GLFW_KEY_S))
            cameraPos -= cameraSpeed * cameraFront;
        if (isKeyDown(GLFW_KEY_A))
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (isKeyDown(GLFW_KEY_D))
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 projection;
        float nearPlane = handleManager("Near Plane")->getValue<float>();
        float farPlane = handleManager("Far Plane")->getValue<float>();

        projection =
            glm::perspective(glm::radians(handleManager("FoV")->getValue<float>()), (float)actualRenderWidth / actualRenderHeight, nearPlane, farPlane);

        glUseProgram(programManager("DEFAULT"));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureManager("IMAGE_VULKAN1"));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureManager("IMAGE_VULKAN2"));

        glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "Mix"), handleManager("Mix")->getValue<float>());

        // Bind view and projection matrix to shader program
        glUniformMatrix4fv(glGetUniformLocation(programManager("DEFAULT"), "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(programManager("DEFAULT"), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(vaoManager.id);
        for (unsigned int i = 0; i < 10; i++) {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle) + ((float)std::pow(-1.0f, (i % 2))) * (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
            // Bind a new model for each set of draws
            glUniformMatrix4fv(glGetUniformLocation(programManager("DEFAULT"), "model"), 1, GL_FALSE, glm::value_ptr(model));
            // Draw a cube using draw arrays without any ebo to avoid having to set unique texture coordinates for each face.
            glDrawArrays(GL_TRIANGLES, 0, vaoManager.getVertexCount());
        }

        return;
    }

} // namespace OpenGLExamples