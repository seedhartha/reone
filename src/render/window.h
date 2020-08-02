#pragma once

#include <functional>
#include <memory>

#include "camera/camera.h"

#include "types.h"

namespace reone {

namespace render {

class RenderWindow {
public:
    RenderWindow(const GraphicsOptions &opts, IEventHandler *eventHandler = nullptr);

    void init();
    void deinit();
    void show();
    void processEvents(bool &quit);
    void render(const std::shared_ptr<Camera> &camera) const;

    void setRelativeMouseMode(bool enabled);

    void setRenderWorldFunc(const std::function<void()> &fn);
    void setRenderGUIFunc(const std::function<void()> &fn);

private:
    GraphicsOptions _opts;
    IEventHandler *_eventHandler { nullptr };
    SDL_Window *_window { nullptr };
    SDL_GLContext _context { nullptr };
    std::function<void()> _onRenderWorld;
    std::function<void()> _onRenderGUI;

    bool handleEvent(const SDL_Event &event, bool &quit);
    bool handleKeyDownEvent(const SDL_KeyboardEvent &event, bool &quit);
    void renderWorld(const std::shared_ptr<Camera> &camera) const;
    void renderGUI() const;
};

} // namespace render

} // namespace reone
