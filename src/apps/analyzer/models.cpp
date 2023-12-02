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

#include "models.h"

#include "reone/graphics/animation.h"
#include "reone/graphics/model.h"
#include "reone/graphics/options.h"
#include "reone/resource/format/mdlmdxreader.h"
#include "reone/resource/provider/models.h"
#include "reone/resource/provider/textures.h"
#include "reone/resource/resources.h"
#include "reone/system/stream/memoryinput.h"

using namespace reone;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

template <class T>
struct MinMaxPair {
    T min {std::numeric_limits<T>::max()};
    T max {std::numeric_limits<T>::min()};
};

using ControllerType = int;

template <class T>
using ControllerTypeToMinMax = std::map<ControllerType, MinMaxPair<T>>;

struct ModelNodeStats {
    ControllerTypeToMinMax<float> ctrlTypeToMinMax;

    void append(ControllerType type, float value) {
        if (ctrlTypeToMinMax.count(type) == 0) {
            ctrlTypeToMinMax[type] = MinMaxPair<float> {value, value};
        } else {
            auto &current = ctrlTypeToMinMax.at(type);
            current.min = std::min(current.min, value);
            current.max = std::max(current.max, value);
        }
    }

    void extend(const ModelNodeStats &other) {
        for (const auto &[type, minMax] : other.ctrlTypeToMinMax) {
            if (ctrlTypeToMinMax.count(type) == 0) {
                ctrlTypeToMinMax[type] = minMax;
            } else {
                auto &current = ctrlTypeToMinMax.at(type);
                current.min = std::min(current.min, minMax.min);
                current.max = std::max(current.max, minMax.max);
            }
        }
    }
};
struct ModelStats {
    ModelNodeStats mesh;
    ModelNodeStats light;
    ModelNodeStats emitter;

    void extend(const ModelStats &other) {
        mesh.extend(other.mesh);
        light.extend(other.light);
        emitter.extend(other.emitter);
    }
};

static ModelStats analyzeModel(graphics::Model &model) {
    ModelStats stats;
    std::stack<std::reference_wrapper<ModelNode>> stack;
    stack.push(std::ref(*model.rootNode()));
    for (auto &[name, anim] : model.animations()) {
        stack.push(*anim->rootNode());
    }
    while (!stack.empty()) {
        auto &node = stack.top().get();
        stack.pop();
        for (const auto &child : node.children()) {
            stack.push(std::ref(*child));
        }
        ModelNodeStats *nodeStats;
        if (node.isMesh()) {
            nodeStats = &stats.mesh;
        } else if (node.isLight()) {
            nodeStats = &stats.light;
        } else if (node.isEmitter()) {
            nodeStats = &stats.emitter;
        } else {
            continue;
        }
        for (const auto &[type, frame] : node.controllers()) {
            for (const auto &[time, values] : frame) {
                for (const auto &val : values) {
                    nodeStats->append(type, val);
                }
            }
        }
    }
    return stats;
}

static ModelStats analyzeModels(const std::filesystem::path &gameDir) {
    ModelStats stats;
    Resources resources;
    resources.addKEY(gameDir / std::string("chitin.key"));
    GraphicsOptions graphicsOpt;
    Textures textures(graphicsOpt, resources);
    Models models(textures, resources);
    for (const auto &[container, local] : resources.containers()) {
        const auto &resIds = container->resourceIds();
        for (const auto &resId : resIds) {
            if (resId.type != ResType::Mdl) {
                continue;
            }
            try {
                auto mdlData = container->findResourceData(ResourceId(resId.resRef, ResType::Mdl));
                auto mdxData = container->findResourceData(ResourceId(resId.resRef, ResType::Mdx));
                if (!mdlData || !mdxData) {
                    continue;
                }
                auto mdl = MemoryInputStream(*mdlData);
                auto mdx = MemoryInputStream(*mdxData);
                auto reader = MdlMdxReader(mdl, mdx, models, textures);
                reader.load();
                auto model = reader.model();
                stats.extend(analyzeModel(*model));
            } catch (const std::exception &e) {
                std::cerr << e.what() << std::endl;
            }
        }
    }
    return stats;
}

void printModelNodeStats(const std::string &nodeType, const ModelNodeStats &stats) {
    std::cout << "Node type " << nodeType << ":" << std::endl;
    std::cout << std::setprecision(4);
    for (const auto &[type, minMax] : stats.ctrlTypeToMinMax) {
        std::cout
            << "  "
            << "Controller type " << static_cast<int>(type) << ": "
            << "min=" << minMax.min << ": "
            << "max=" << minMax.max
            << std::endl;
    }
}

void analyzeModels(const std::filesystem::path &k1Dir, const std::filesystem::path &k2Dir) {
    ModelStats stats;
    std::cout << "Analyzing KotOR models" << std::endl;
    stats.extend(analyzeModels(k1Dir));
    std::cout << "Analyzing TSL models" << std::endl;
    stats.extend(analyzeModels(k2Dir));
    std::cout << std::endl;
    printModelNodeStats("mesh", stats.mesh);
    std::cout << std::endl;
    printModelNodeStats("light", stats.light);
    std::cout << std::endl;
    printModelNodeStats("emitter", stats.emitter);
}

} // namespace reone
