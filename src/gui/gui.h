#pragma once

#include "../render/font.h"
#include "../resources/gfffile.h"

#include "control/control.h"

namespace reone {

namespace gui {

class GUI : public render::IRenderRoot, public render::IEventHandler {
public:
    bool handle(const SDL_Event &event) override;
    void initGL() override;
    void render() const override;

    virtual void onClick(const std::string &control);
    virtual void onItemClicked(const std::string &control, const std::string &item);

protected:
    enum class ScalingMode {
        Center,
        Scale
    };

    render::GraphicsOptions _opts;
    ScalingMode _scaling { ScalingMode::Center };
    glm::vec3 _screenCenter { 0.0f };
    glm::mat4 _controlTransform { 1.0f };
    std::shared_ptr<render::Font> _font;
    std::shared_ptr<render::Texture> _background;
    std::shared_ptr<Control> _rootControl;
    std::vector<std::shared_ptr<Control>> _controls;
    std::shared_ptr<Control> _focus;

    GUI(const render::GraphicsOptions &opts);

    void load(const std::string &resRef, BackgroundType background);
    void loadFont();
    void loadBackground(BackgroundType type);
    void showControl(const std::string &tag);
    void hideControl(const std::string &tag);

    Control &getControl(const std::string &tag) const;

private:
    GUI(const GUI &) = delete;
    GUI &operator=(const GUI &) = delete;

    std::unique_ptr<Control> makeControl(const resources::GffStruct &gffs) const;
    glm::vec2 screenToControlCoords(int x, int y) const;
    void updateFocus(int x, int y);
    void renderBackground() const;
};

} // namespace gui

} // namespace reone
