#include "debug.h"

#include "control/label.h"

using namespace reone::render;

namespace reone {

namespace gui {

DebugGui::DebugGui(const GraphicsOptions &opts) : GUI(opts) {
}

void DebugGui::load() {
    loadFont();
}

void DebugGui::update(const DebugContext &ctx) {
    _controls.clear();

    for (auto &object : ctx.objects) {
        if (object.screenCoords.z >= 1.0f) continue;

        float textWidth = _font->measure(object.text);
        Control::Extent extent(
            _opts.width * object.screenCoords.x - 0.5f * textWidth,
            _opts.height * (1.0f - object.screenCoords.y),
            textWidth,
            _font->height());

        Control::Text text;
        text.text = object.text;
        text.font = _font;
        text.color = glm::vec3(1.0f, 0.0f, 0.0f);

        std::unique_ptr<Label> label(new Label(object.tag));
        label->setExtent(extent);
        label->setText(text);

        _controls.push_back(std::move(label));
    }
}

} // namespace gui

} // namespace reone
