#pragma once

#ifndef RENDER_VIEW_H
#define RENDER_VIEW_H

#include "imgui.h"

class RenderView {

  protected:
    int renderWidth;
    int renderHeight;

  public:
    RenderView(int renderWidth = 1920, int renderHeight = 1080) : renderWidth(renderWidth), renderHeight(renderHeight) {}
    virtual ~RenderView(){};

    /* Inits the render view */
    virtual void init() = 0;

    /* Returns a texture/rendered image as a ImTextureID pointer for ImGui to render to a renderview */
    virtual ImTextureID render() = 0;

    int width() { return renderWidth; }
    int height() { return renderHeight; }
};

#endif