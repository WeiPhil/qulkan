#pragma once

#ifndef WINDOWS_H
#define WINDOWS_H

#include "render_view.h"

namespace Qulkan {

    /* Constructs the docking space */
    void dockingSpace();

    /* Constructs the rendering windows */
    void renderWindow(RenderView &renderView, std::vector<RenderView *> &renderViews);

    /* Constructs the panel for the handlers of each view*/
    void viewConfigurations(std::vector<RenderView *> &renderViews);

} // namespace Qulkan

#endif