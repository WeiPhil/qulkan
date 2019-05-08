#pragma once

#ifndef WINDOWS_H
#define WINDOWS_H

#include "render_view.h"

namespace Qulkan {

    /* Constructs the docking space */
    void dockingSpace();

    /* Constructs the rendering windows */
    void renderWindow(RenderView &renderView);

    /* Constructs the panel for the handlers of each view*/
    void configurationView(std::vector<RenderView *> &renderViews);

} // namespace Qulkan

#endif