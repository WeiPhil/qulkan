#pragma once

#ifndef GT_REFLECTION_ANISO_TWO_BOUNCES_H
#define GT_REFLECTION_ANISO_TWO_BOUNCES_H

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
#include "imgui.h"

class GTReflectionAnisoTwoBounces : public Qulkan::RenderView {

  private:
    BufferManager bufferManager;
    FramebufferManager framebufferManager;
    TextureManager textureManager;
    ShaderManager shaderManager;
    ProgramManager programManager;
    VAOManager<glf::vertex_v3fv2f> vaoManager;
    EBOManager eboManager;

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
    GTReflectionAnisoTwoBounces(const char *viewName = "GTReflectionAnisoTwoBounces View", int initialRenderWidth = 1920, int initialRenderHeight = 1080);
    virtual ~GTReflectionAnisoTwoBounces() { clean(); }

    /* Inits an OpenGL view */
    virtual void init();

    /* Returns a texture as a ImTextureID pointer for ImGui to render to a renderview */
    virtual void render(int actualRenderWidth, int actualRenderHeight);
};

#endif