#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "qulkan/render_view.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "framework/opengl/buffermanager.h"
#include "framework/opengl/ebomanager.h"
#include "framework/opengl/framebuffermanager.h"
#include "framework/opengl/programmanager.h"
#include "framework/opengl/shadermanager.h"
#include "framework/opengl/texturemanager.h"
#include "framework/opengl/vaomanager.h"
#include "framework/opengl/vertex.h"

namespace OpenGLExamples {

    class Camera : public Qulkan::RenderView {

      private:
        BufferManager bufferManager;
        FramebufferManager framebufferManager;
        TextureManager textureManager;
        ShaderManager shaderManager;
        ProgramManager programManager;
        VAOManager<glf::vertex_v3fv2f> vaoManager;

        std::vector<glm::vec3> cubePositions;

        // Camera variables
        float yaw = -90.0f;
        float pitch = 0.0;

        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraFront =
            glm::vec3(cos(glm::radians(pitch)) * cos(glm::radians(yaw)), sin(glm::radians(pitch)), cos(glm::radians(pitch)) * sin(glm::radians(yaw)));
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);

        void createCube();

      protected:
        void initHandles();

        void initProgram();

        void initBuffer();

        void initTexture();

        void initVertexArray();

        void initFramebuffer();

        void initOpenGLOptions();

        void clean();

      public:
        Camera(const char *viewName = "OpenGL Example: Transformations", int initialRenderWidth = 1920, int initialRenderHeight = 1080);
        virtual ~Camera() { clean(); }

        /* Inits an OpenGL view */
        virtual void init();

        /* Returns a texture as a ImTextureID pointer for ImGui to render to a renderview */
        virtual void render(int actualRenderWidth, int actualRenderHeight);
    };
} // namespace OpenGLExamples

#endif