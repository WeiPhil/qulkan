#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "qulkan/render_view.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "framework/buffermanager.h"
#include "framework/ebomanager.h"
#include "framework/framebuffermanager.h"
#include "framework/programmanager.h"
#include "framework/shadermanager.h"
#include "framework/texturemanager.h"
#include "framework/vaomanager.h"
#include "framework/vertex.h"

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
        Camera(const char *viewName = "OpenGL Example: Transformations", int renderWidth = 1920, int renderHeight = 1080);
        virtual ~Camera() { clean(); }

        /* Inits an OpenGL view */
        virtual void init();

        /* Returns a texture as a ImTextureID pointer for ImGui to render to a renderview */
        virtual ImTextureID render();
    };
} // namespace OpenGLExamples

#endif