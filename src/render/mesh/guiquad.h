#pragma once

#include "mesh.h"

namespace reone {

namespace render {

class GUIQuad : public Mesh {
public:
    static GUIQuad &instance();

private:
    GUIQuad();
};

} // namespace render

} // namespace reone
