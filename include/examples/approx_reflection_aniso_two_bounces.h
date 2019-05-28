#pragma once

#ifndef APPROX_REFLECTION_ANISO_TWO_BOUNCES_H
#define APPROX_REFLECTION_ANISO_TWO_BOUNCES_H

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
#include "imgui.h"

class ApproxReflectionAnisoTwoBounces : public Qulkan::RenderView {

  private:
    BufferManager bufferManager;
    FramebufferManager framebufferManager;
    TextureManager textureManager;
    ShaderManager shaderManager;
    ProgramManager programManager;
    VAOManager<glf::vertex_v3fv2f> vaoManager;
    EBOManager eboManager;

    float nextEventTime;
    int currX = 0;
    int currY = 0;

    float alphaxs[11] = {0.001f, 0.005f, 0.01f, 0.05f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f};
    float alphays[11] = {0.001f, 0.005f, 0.01f, 0.05f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f};

    void readFileTo(std::ifstream &in, std::vector<float> &data);

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
    ApproxReflectionAnisoTwoBounces(const char *viewName = "ApproxReflectionAnisoTwoBounces View", int initialRenderWidth = 1920,
                                    int initialRenderHeight = 1080);
    virtual ~ApproxReflectionAnisoTwoBounces() { clean(); }

    /* Inits an OpenGL view */
    virtual void init();

    /* Returns a texture as a ImTextureID pointer for ImGui to render to a renderview */
    virtual void render(int actualRenderWidth, int actualRenderHeight);
};

#endif