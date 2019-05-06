#pragma once

#ifndef RENDER_VIEW_H
#define RENDER_VIEW_H

class RenderView {

  public:
    virtual ~RenderView() {}

    /* Inits the render view */
    virtual void init() = 0;

    /* Renders the rendering view */
    virtual void render() = 0;

    virtual void *renderToTexture() = 0;
};

#endif