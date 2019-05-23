#pragma once

#ifndef WINDOWS_H
#define WINDOWS_H

#include "render_view.h"

namespace Qulkan {

    /* Constructs the docking space */
    void dockingSpace(std::vector<RenderView *> &renderViews);

    /* Inits all render views */
    void initViews(std::vector<RenderView *> &renderViews);

    /* Constructs the rendering windows */
    void renderWindows(std::vector<RenderView *> &renderViews);

    /* Constructs the panel for the handlers of each view*/
    void viewConfigurations(std::vector<RenderView *> &renderViews);

} // namespace Qulkan

#endif