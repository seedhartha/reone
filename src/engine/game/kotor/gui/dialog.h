/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "../../../gui/control/label.h"
#include "../../../gui/control/listbox.h"

#include "../../core/camera/dialog.h"
#include "../../core/object/creature.h"

#include "conversation.h"

namespace reone {

namespace game {

class DialogGUI : public Conversation {
public:
    DialogGUI(
        Game *game,
        ActionFactory &actionFactory,
        Classes &classes,
        Combat &combat,
        Feats &feats,
        FootstepSounds &footstepSounds,
        GUISounds &guiSounds,
        ObjectFactory &objectFactory,
        Party &party,
        Portraits &portraits,
        Reputes &reputes,
        ScriptRunner &scriptRunner,
        SoundSets &soundSets,
        Surfaces &surfaces,
        audio::AudioFiles &audioFiles,
        audio::AudioPlayer &audioPlayer,
        graphics::Context &context,
        graphics::Features &features,
        graphics::Fonts &fonts,
        graphics::Lips &lips,
        graphics::Materials &materials,
        graphics::Meshes &meshes,
        graphics::Models &models,
        graphics::PBRIBL &pbrIbl,
        graphics::Shaders &shaders,
        graphics::Textures &textures,
        graphics::Walkmeshes &walkmeshes,
        graphics::Window &window,
        resource::Resources &resources,
        resource::Strings &strings);

    void load() override;
    void update(float dt) override;

private:
    struct Participant {
        std::shared_ptr<graphics::Model> model;
        std::shared_ptr<Creature> creature;
    };

    struct Binding {
        std::shared_ptr<gui::Label> lblMessage;
        std::shared_ptr<gui::ListBox> lbReplies;
    } _binding;

    std::shared_ptr<SpatialObject> _currentSpeaker;
    std::map<std::string, Participant> _participantByTag;

    void bindControls();
    void addFrame(std::string tag, int top, int height);
    void configureMessage();
    void configureReplies();
    void repositionMessage();

    void updateCamera();
    void updateParticipantAnimations();

    glm::vec3 getTalkPosition(const SpatialObject &object) const;
    DialogCamera::Variant getRandomCameraVariant() const;
    std::string getStuntAnimationName(int ordinal) const;
    AnimationType getStuntAnimationType(int ordinal) const;

    void setMessage(std::string message) override;
    void setReplyLines(std::vector<std::string> lines) override;

    void onStart() override;
    void onFinish() override;
    void onLoadEntry() override;
    void onEntryEnded() override;

    // Loading

    void loadFrames();
    void loadCurrentSpeaker();

    // END Loading

    // Participants

    void loadStuntParticipants();
    void releaseStuntParticipants();

    // END Participants
};

} // namespace game

} // namespace reone
