/*
 * Copyright (c) 2020-2023 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "reone/graphics/di/module.h"

namespace reone {

namespace graphics {

class ToolkitGraphicsModule : public GraphicsModule {
public:
    ToolkitGraphicsModule(graphics::GraphicsOptions &options) :
        GraphicsModule(options) {
    }

private:
    struct StubWindow : public IWindow, boost::noncopyable {
    public:
        void processEvents(bool &quit) {}
        void swapBuffers() const {}
        bool isInFocus() const { return false; }
        glm::mat4 getOrthoProjection(float near, float far) const { return glm::mat4(1.0f); };
        void setEventHandler(IEventHandler *eventHandler) {}
        void setRelativeMouseMode(bool enabled) {}
        uint32_t mouseState(int *x, int *y) { return 0; }
        void showCursor(bool show) {}
    };

    std::unique_ptr<IWindow> newWindow() override {
        return std::make_unique<StubWindow>();
    }
};

} // namespace graphics

} // namespace reone
