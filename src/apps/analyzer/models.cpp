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
#include "reone/graphics/format/mdlmdxreader.h"
#include "reone/graphics/model.h"
#include "reone/graphics/options.h"
#include "reone/graphics/statistic.h"
#include "reone/resource/provider/models.h"
#include "reone/resource/provider/textures.h"
#include "reone/resource/resources.h"
#include "reone/system/stream/memoryinput.h"

using namespace reone;
using namespace reone::graphics;
using namespace reone::resource;

template <>
struct std::less<glm::vec3> {
    bool operator()(const glm::vec3 &lhs, const glm::vec3 &rhs) const {
        if (lhs.x < rhs.x) {
            return true;
        }
        if (lhs.x > rhs.x) {
            return false;
        }
        if (lhs.y < rhs.y) {
            return true;
        }
        if (lhs.y > rhs.y) {
            return false;
        }
        if (lhs.z < rhs.z) {
            return true;
        }
        if (lhs.z > rhs.z) {
            return false;
        }
        return false;
    }
};

namespace reone {

static const std::unordered_map<ControllerType, std::string> g_ctrlTypeToNameDummy {
    {{8, "position"},
     {20, "orientation"},
     {36, "scale"}}};

static const std::unordered_map<ControllerType, std::string> g_ctrlTypeToNameMesh {
    {{8, "position"},
     {20, "orientation"},
     {36, "scale"},

     // Mesh
     {100, "selfIllumColor"},
     {132, "alpha"}}};

static const std::unordered_map<ControllerType, std::string> g_ctrlTypeToNameLight {
    {{8, "position"},
     {20, "orientation"},
     {36, "scale"},

     // Light
     {76, "color"},
     {88, "radius"},
     {96, "shadowRadius"},
     {100, "verticalDisplacement"},
     {140, "multiplier"}}};

static const std::unordered_map<ControllerType, std::string> g_ctrlTypeToNameEmitter {
    {{8, "position"},
     {20, "orientation"},
     {36, "scale"},

     // Emitter
     {80, "alphaEnd"},
     {84, "alphaStart"},
     {88, "birthrate"},
     {92, "bounceCo"},
     {96, "combineTime"},
     {100, "drag"},
     {104, "fps"},
     {108, "frameEnd"},
     {112, "frameStart"},
     {116, "grav"},
     {120, "lifeExp"},
     {124, "mass"},
     {128, "p2pBezier2"},
     {132, "p2pBezier3"},
     {136, "particleRot"},
     {140, "randVel"},
     {144, "sizeStart"},
     {148, "sizeEnd"},
     {152, "sizeStartY"},
     {156, "sizeEndY"},
     {160, "spread"},
     {164, "threshold"},
     {168, "velocity"},
     {172, "xSize"},
     {176, "ySize"},
     {180, "blurLength"},
     {184, "lightingDelay"},
     {188, "lightingRadius"},
     {192, "lightingScale"},
     {196, "lightingSubDiv"},
     {200, "lighitngZigZag"},
     {216, "alphaMid"},
     {220, "percentStart"},
     {224, "percentMid"},
     {228, "percentEnd"},
     {232, "sizeMid"},
     {236, "sizeMidY"},
     {240, "randomBirthRate"},
     {252, "targetSize"},
     {256, "numControlPts"},
     {260, "controlPtRadius"},
     {264, "controlPtDelay"},
     {268, "tangentSpread"},
     {272, "tangentLength"},
     {284, "colorMid"},
     {380, "colorEnd"},
     {392, "colorStart"},
     {502, "detonate"}}};

template <class T, class U = T, class Comparer = std::less<T>>
struct ControllerStats {
    T min {std::numeric_limits<U>::max()};
    T max {std::numeric_limits<U>::lowest()};
    std::vector<T> values;

    T median() const {
        if (values.empty()) {
            return T();
        }
        if (values.size() == 1ll) {
            return values[0];
        }
        auto valuesCopy = values;
        std::sort(valuesCopy.begin(), valuesCopy.end(), Comparer());
        int midIdx = valuesCopy.size() / 2;
        return valuesCopy[midIdx];
    }
};

template <class T, class U = T>
using ControllerStatsMap = std::map<ControllerType, ControllerStats<T, U>>;

struct ModelNodeStats {
public:
    ControllerStatsMap<float> floatCtrlTypeToStats;
    ControllerStatsMap<glm::vec3, float> vecCtrlTypeToStats;
    ControllerStatsMap<glm::vec3, float> quatCtrlTypeToStats;

    void appendFloat(ControllerType type, float value) {
        append(floatCtrlTypeToStats, type, value);
    }

    void appendVector(ControllerType type, glm::vec3 value) {
        append(vecCtrlTypeToStats, type, value);
    }

    void appendQuaternion(ControllerType type, glm::quat value) {
        append(quatCtrlTypeToStats, type, glm::vec3 {value.x, value.y, value.z});
    }

    void extend(const ModelNodeStats &other) {
        extend(floatCtrlTypeToStats, other.floatCtrlTypeToStats);
        extend(vecCtrlTypeToStats, other.vecCtrlTypeToStats);
        extend(quatCtrlTypeToStats, other.quatCtrlTypeToStats);
    }

private:
    template <class T, class U = T>
    void append(ControllerStatsMap<T, U> &map, ControllerType type, T value) {
        if (map.count(type) == 0) {
            map[type] = {value, value, {value}};
        } else {
            auto &current = map.at(type);
            current.min = glm::min(current.min, value);
            current.max = glm::max(current.max, value);
            current.values.push_back(std::move(value));
        }
    }

    template <class T, class U = T>
    void extend(ControllerStatsMap<T, U> &map, const ControllerStatsMap<T, U> &otherMap) {
        for (const auto &[type, stats] : otherMap) {
            if (map.count(type) == 0) {
                map[type] = stats;
            } else {
                auto &current = map.at(type);
                current.min = glm::min(current.min, stats.min);
                current.max = glm::max(current.max, stats.max);
                for (const auto &value : stats.values) {
                    current.values.push_back(value);
                }
            }
        }
    }
};

struct ModelStats {
    ModelNodeStats dummy;
    ModelNodeStats mesh;
    ModelNodeStats light;
    ModelNodeStats emitter;

    void extend(const ModelStats &other) {
        dummy.extend(other.dummy);
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
        auto nodeBase = model.getNodeByName(node.name());
        ModelNodeStats *nodeStats;
        if (nodeBase && nodeBase->isMesh()) {
            nodeStats = &stats.mesh;
        } else if (nodeBase && nodeBase->isLight()) {
            nodeStats = &stats.light;
        } else if (nodeBase && nodeBase->isEmitter()) {
            nodeStats = &stats.emitter;
        } else {
            nodeStats = &stats.dummy;
        }
        for (const auto &[type, track] : node.quaternionTracks()) {
            for (const auto &[time, value] : track.keyframes()) {
                nodeStats->appendQuaternion(type, value);
            }
        }
        for (const auto &[type, track] : node.vectorTracks()) {
            for (const auto &[time, value] : track.keyframes()) {
                nodeStats->appendVector(type, value);
            }
        }
        for (const auto &[type, track] : node.floatTracks()) {
            for (const auto &[time, value] : track.keyframes()) {
                nodeStats->appendFloat(type, value);
            }
        }
    }
    return stats;
}

static ModelStats analyzeModels(const std::filesystem::path &gameDir) {
    ModelStats stats;
    Resources resources;
    resources.addKEY(gameDir / std::string("chitin.key"));
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
                Statistic statistic;
                auto reader = MdlMdxReader(mdl, mdx, statistic);
                reader.load();
                auto model = reader.model();
                stats.extend(analyzeModel(*model));
            } catch (const std::exception &e) {
                std::cerr << "Model " << resId.resRef.value() << ": " << e.what() << std::endl;
            }
        }
    }
    return stats;
}

void printModelNodeStats(const std::string &nodeType,
                         const ModelNodeStats &stats,
                         const std::unordered_map<ControllerType, std::string> &ctrlTypeToName) {
    std::cout << "Node type " << nodeType << ":" << std::endl;
    std::cout << std::setprecision(4);
    for (auto &[type, ctrlStats] : stats.floatCtrlTypeToStats) {
        std::string name;
        if (ctrlTypeToName.count(type) > 0) {
            name = ctrlTypeToName.at(type);
        } else {
            name = "unknown";
        }
        auto median = ctrlStats.median();
        std::cout
            << "  "
            << "Controller type " << static_cast<int>(type) << " (" << name << "): "
            << "min=" << ctrlStats.min << " "
            << "max=" << ctrlStats.max << " "
            << "median=" << median
            << std::endl;
    }
    for (auto &[type, ctrlStats] : stats.vecCtrlTypeToStats) {
        std::string name;
        if (ctrlTypeToName.count(type) > 0) {
            name = ctrlTypeToName.at(type);
        } else {
            name = "unknown";
        }
        auto median = ctrlStats.median();
        std::cout
            << "  "
            << "Controller type " << static_cast<int>(type) << " (" << name << "): "
            << "min=[" << ctrlStats.min.x << ", " << ctrlStats.min.y << ", " << ctrlStats.min.z << "] "
            << "max=[" << ctrlStats.max.x << ", " << ctrlStats.max.y << ", " << ctrlStats.max.z << "] "
            << "median=[" << median.x << ", " << median.y << ", " << median.z << "]"
            << std::endl;
    }
    for (auto &[type, ctrlStats] : stats.quatCtrlTypeToStats) {
        std::string name;
        if (ctrlTypeToName.count(type) > 0) {
            name = ctrlTypeToName.at(type);
        } else {
            name = "unknown";
        }
        auto median = ctrlStats.median();
        std::cout
            << "  "
            << "Controller type " << static_cast<int>(type) << " (" << name << "): "
            << "min=[" << ctrlStats.min.x << ", " << ctrlStats.min.y << ", " << ctrlStats.min.z << "] "
            << "max=[" << ctrlStats.max.x << ", " << ctrlStats.max.y << ", " << ctrlStats.max.z << "] "
            << "median=[" << median.x << ", " << median.y << ", " << median.z << "]"
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
    printModelNodeStats("dummy", stats.dummy, g_ctrlTypeToNameDummy);
    std::cout << std::endl;
    printModelNodeStats("mesh", stats.mesh, g_ctrlTypeToNameMesh);
    std::cout << std::endl;
    printModelNodeStats("light", stats.light, g_ctrlTypeToNameLight);
    std::cout << std::endl;
    printModelNodeStats("emitter", stats.emitter, g_ctrlTypeToNameEmitter);
}

} // namespace reone
