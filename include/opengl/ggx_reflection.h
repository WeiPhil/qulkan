#pragma once

#ifndef GGX_REFLECTION_H
#define GGX_REFLECTION_H

#include "opengl/opengl_view.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "framework/programmanager.h"
#include "framework/shadermanager.h"
#include "framework/texturemanager.h"
#include "framework/vertex.h"
#include "imgui.h"

class GGXReflection : public OpenGLView {

  private:
    TextureManager textureManager;
    ShaderManager shaderManager;
    ProgramManager programManager;

  protected:
    virtual void initProgram();

    virtual void initBuffer();

    virtual void initTexture();

    virtual void initVertexArray();

    virtual void initFramebuffer();

    virtual void clean();

  public:
    GGXReflection(const char *viewName = "GGXReflection View", int renderWidth = 1920, int renderHeight = 1080);
    virtual ~GGXReflection() { clean(); }

    /* Inits an OpenGL view */
    virtual void init();

    /* Returns a texture as a ImTextureID pointer for ImGui to render to a renderview */
    virtual ImTextureID render();
};

#endif