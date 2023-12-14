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

#include "reone/game/gui/dialog.h"

#include "reone/audio/player.h"
#include "reone/audio/source.h"
#include "reone/graphics/di/services.h"
#include "reone/gui/control/panel.h"
#include "reone/resource/2da.h"
#include "reone/resource/di/services.h"
#include "reone/resource/provider/2das.h"
#include "reone/resource/provider/audioclips.h"
#include "reone/resource/provider/models.h"
#include "reone/scene/types.h"
#include "reone/script/execution.h"
#include "reone/system/logutil.h"
#include "reone/system/randomutil.h"

#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/party.h"

using namespace reone::audio;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

static const char kControlTagTopFrame[] = "TOP";
static const char kControlTagBottomFrame[] = "BOTTOM";
static const char kObjectTagOwner[] = "owner";

static const std::unordered_map<std::string, AnimationType> g_animTypeByName {
    {"dead", AnimationType::LoopingDead},
    {"taunt", AnimationType::FireForgetTaunt},
    {"greeting", AnimationType::FireForgetGreeting},
    {"listen", AnimationType::LoopingListen},
    {"worship", AnimationType::LoopingWorship},
    {"salute", AnimationType::FireForgetSalute},
    {"bow", AnimationType::FireForgetBow},
    {"talk_normal", AnimationType::LoopingTalkNormal},
    {"talk_pleading", AnimationType::LoopingTalkPleading},
    {"talk_forceful", AnimationType::LoopingTalkForceful},
    {"talk_laughing", AnimationType::LoopingTalkLaughing},
    {"talk_sad", AnimationType::LoopingTalkSad},
    {"victory", AnimationType::FireForgetVictory1},
    {"scratch_head", AnimationType::FireForgetPauseScratchHead},
    {"drunk", AnimationType::LoopingPauseDrunk},
    {"inject", AnimationType::FireForgetInject},
    {"flirt", AnimationType::LoopingFlirt},
    {"use_computer_lp", AnimationType::LoopingUseComputer},
    {"horror", AnimationType::LoopingHorror},
    {"use_computer", AnimationType::FireForgetUseComputer},
    {"persuade", AnimationType::FireForgetPersuade},
    {"activate", AnimationType::FireForgetActivate},
    {"sleep", AnimationType::LoopingSleep},
    {"prone", AnimationType::LoopingProne},
    {"ready", AnimationType::LoopingReady},
    {"pause", AnimationType::LoopingPause},
    {"choked", AnimationType::LoopingChoke},
    {"talk_injured", AnimationType::LoopingTalkInjured},
    {"listen_injured", AnimationType::LoopingListenInjured},
    {"kneel_talk_angry", AnimationType::LoopingKneelTalkAngry},
    {"kneel_talk_sad", AnimationType::LoopingKneelTalkSad}};

void DialogGUI::preload(IGUI &gui) {
    gui.setScaling(GUI::ScalingMode::Stretch);
}

void DialogGUI::onGUILoaded() {
    bindControls();
    configureMessage();
    configureReplies();
    loadFrames();

    _controls.LB_REPLIES->setOnItemClick([this](const std::string &item) {
        int replyIdx = stoi(item);
        pickReply(replyIdx);
    });
}

void DialogGUI::loadFrames() {
    int rootTop = _gui->rootControl().extent().top;
    int messageHeight = _controls.LBL_MESSAGE->extent().height;

    addFrame(kControlTagTopFrame, -rootTop, messageHeight);
    addFrame(kControlTagBottomFrame, 0, _game.options().graphics.height - rootTop);
}

void DialogGUI::addFrame(std::string tag, int top, int height) {
    auto frame = _gui->newControl(ControlType::Panel, tag);

    Control::Extent extent;
    extent.left = -_gui->rootControl().extent().left;
    extent.top = top;
    extent.width = _game.options().graphics.width;
    extent.height = height;

    frame->setExtent(std::move(extent));
    frame->setBorderFill("blackfill");

    _gui->addControl(std::move(frame));
}

void DialogGUI::configureMessage() {
    _controls.LBL_MESSAGE->setExtentTop(-_gui->rootControl().extent().top);
    _controls.LBL_MESSAGE->setTextColor(_baseColor);
}

void DialogGUI::configureReplies() {
    _controls.LB_REPLIES->setProtoMatchContent(true);
    _controls.LB_REPLIES->protoItem().setHilightColor(_hilightColor);
    _controls.LB_REPLIES->protoItem().setTextColor(_baseColor);
}

void DialogGUI::onStart() {
    _currentSpeaker = _owner;
    loadStuntParticipants();

    auto camera = _game.module()->area()->getCamera<AnimatedCamera>(CameraType::Animated);
    camera->setModel(_cameraModel);
}

void DialogGUI::loadStuntParticipants() {
    if (!_dialog->isAnimatedCutscene())
        return;

    _participantByTag.clear();

    for (auto &stunt : _dialog->stunts) {
        std::shared_ptr<Creature> creature;
        if (stunt.participant == kObjectTagOwner) {
            creature = std::dynamic_pointer_cast<Creature>(_owner);
        } else {
            creature = std::dynamic_pointer_cast<Creature>(_game.module()->area()->getObjectByTag(stunt.participant));
        }
        if (!creature) {
            warn("Dialog: participant creature not found by tag: " + stunt.participant);
            continue;
        }
        Participant participant;
        participant.creature = creature;

        if (_dialog->isAnimatedCutscene()) {
            std::shared_ptr<Model> model(_services.resource.models.get(stunt.stuntModel));
            if (!model) {
                warn("Dialog: stunt model not found: " + stunt.stuntModel);
                continue;
            }
            participant.model = model;
            creature->startStuntMode();
        }

        _participantByTag.insert(std::make_pair(stunt.participant, std::move(participant)));
    }
}

void DialogGUI::onLoadEntry() {
    loadCurrentSpeaker();
    updateCamera();
    updateParticipantAnimations();
    repositionMessage();

    _controls.LB_REPLIES->setVisible(false);
}

void DialogGUI::loadCurrentSpeaker() {
    std::shared_ptr<Area> area(_game.module()->area());
    std::shared_ptr<Object> speaker;

    if (!_currentEntry->speaker.empty()) {
        speaker = area->getObjectByTag(_currentEntry->speaker);
    }
    if (!speaker) {
        speaker = _owner;
    }

    // Make previous speaker stop talking, if any
    if (_currentSpeaker && _currentSpeaker != speaker) {
        auto speakerCreature = std::dynamic_pointer_cast<Creature>(_currentSpeaker);
        if (speakerCreature) {
            speakerCreature->stopTalking();
        }
    }
    _currentSpeaker = speaker;

    // Make current speaker face the player, and vice versa
    if (_currentSpeaker) {
        std::shared_ptr<Creature> player(_game.party().player());
        player->face(*_currentSpeaker);

        auto speakerCreature = std::dynamic_pointer_cast<Creature>(_currentSpeaker);
        if (speakerCreature) {
            speakerCreature->startTalking(_lipAnimation);
            speakerCreature->face(*player);
        }
    }
}

void DialogGUI::updateCamera() {
    std::shared_ptr<Area> area(_game.module()->area());

    if (_dialog->cameraModel.empty()) {
        std::shared_ptr<Creature> player(_game.party().player());
        glm::vec3 listenerPosition(player ? getTalkPosition(*player) : glm::vec3(0.0f));
        glm::vec3 speakerPosition(_currentSpeaker ? getTalkPosition(*_currentSpeaker) : glm::vec3(0.0f));
        auto camera = area->getCamera<DialogCamera>(CameraType::Dialog);
        camera->setListenerPosition(listenerPosition);
        camera->setSpeakerPosition(speakerPosition);
        camera->setVariant(getRandomCameraVariant());
    } else {
        auto camera = area->getCamera<AnimatedCamera>(CameraType::Animated);
        camera->setFieldOfView(_currentEntry->camFieldOfView != 0.0f ? _currentEntry->camFieldOfView : kDefaultAnimCamFOV);
        camera->playAnimation(_currentEntry->cameraAnimation);
    }
}

glm::vec3 DialogGUI::getTalkPosition(const Object &object) const {
    auto model = std::static_pointer_cast<ModelSceneNode>(object.sceneNode());
    if (!model)
        return object.position();

    std::shared_ptr<ModelNode> talkDummy(model->model().getNodeByNameRecursive("talkdummy"));
    if (!talkDummy)
        return model->getWorldCenterOfAABB();

    return (model->absoluteTransform() * talkDummy->absoluteTransform())[3];
}

DialogCamera::Variant DialogGUI::getRandomCameraVariant() const {
    int r = randomInt(0, 2);
    switch (r) {
    case 0:
        return _entryEnded ? DialogCamera::Variant::ListenerClose : DialogCamera::Variant::SpeakerClose;
    case 1:
        return _entryEnded ? DialogCamera::Variant::ListenerFar : DialogCamera::Variant::SpeakerFar;
    default:
        return DialogCamera::Variant::Both;
    }
}

void DialogGUI::updateParticipantAnimations() {
    for (auto &anim : _currentEntry->animations) {
        if (_dialog->isAnimatedCutscene()) {
            auto maybeParticipant = _participantByTag.find(anim.participant);
            if (maybeParticipant == _participantByTag.end()) {
                warn("Dialog: participant not found by tag: " + anim.participant);
                continue;
            }
            const Participant &participant = maybeParticipant->second;
            std::string animName(getStuntAnimationName(anim.animation));
            std::shared_ptr<Animation> animation(participant.model->getAnimation(animName));
            if (animation) {
                AnimationProperties properties;
                properties.scale = 1.0f;
                participant.creature->playAnimation(animation, std::move(properties));
            }
        } else {
            std::shared_ptr<Creature> participant;
            if (anim.participant == "owner") {
                participant = std::dynamic_pointer_cast<Creature>(_owner);
            } else {
                participant = std::dynamic_pointer_cast<Creature>(_game.module()->area()->getObjectByTag(anim.participant));
            }
            if (!participant) {
                warn("Dialog: participant creature not found by tag: " + anim.participant);
                continue;
            }
            AnimationType animType = getStuntAnimationType(anim.animation);
            if (animType != AnimationType::Invalid) {
                participant->playAnimation(animType);
            }
        }
    }
}

std::string DialogGUI::getStuntAnimationName(int ordinal) const {
    return str(boost::format("cut%03dw") % (ordinal - 1200 + 1));
}

AnimationType DialogGUI::getStuntAnimationType(int ordinal) const {
    std::shared_ptr<TwoDa> animations(_services.resource.twoDas.get("dialoganimations"));
    int index = ordinal - 10000;

    if (index < 0 || index >= animations->getRowCount()) {
        warn("Dialog: animation index out of bounds: " + std::to_string(index));
        return AnimationType::Invalid;
    }

    std::string name(boost::to_lower_copy(animations->getString(index, "name")));
    auto maybeAnimType = g_animTypeByName.find(name);

    return maybeAnimType != g_animTypeByName.end() ? maybeAnimType->second : AnimationType::Invalid;
}

void DialogGUI::repositionMessage() {
    Control::Text text(_controls.LBL_MESSAGE->text());
    int top;

    if (_entryEnded) {
        text.align = Control::TextAlign::CenterBottom;
        top = -_gui->rootControl().extent().top;
    } else {
        text.align = Control::TextAlign::CenterTop;
        top = _controls.LB_REPLIES->extent().top;
    }

    _controls.LBL_MESSAGE->setText(std::move(text));
    _controls.LBL_MESSAGE->setExtentTop(top);
}

void DialogGUI::onFinish() {
    if (_dialog->isAnimatedCutscene()) {
        releaseStuntParticipants();
    }

    // Make current speaker stop talking, if any
    auto speakerCreature = std::dynamic_pointer_cast<Creature>(_currentSpeaker);
    if (speakerCreature) {
        speakerCreature->stopTalking();
    }
}

void DialogGUI::releaseStuntParticipants() {
    for (auto &participant : _participantByTag) {
        participant.second.creature->stopStuntMode();
    }
}

void DialogGUI::onEntryEnded() {
    _controls.LB_REPLIES->setVisible(true);

    updateCamera();
    repositionMessage();
}

void DialogGUI::setMessage(std::string message) {
    _controls.LBL_MESSAGE->setTextMessage(message);
}

void DialogGUI::setReplyLines(std::vector<std::string> lines) {
    _controls.LB_REPLIES->clearItems();

    for (size_t i = 0; i < lines.size(); ++i) {
        ListBox::Item item;
        item.tag = std::to_string(i);
        item.text = lines[i];
        _controls.LB_REPLIES->addItem(std::move(item));
    }
}

void DialogGUI::update(float dt) {
    Conversation::update(dt);

    // Dialog camera follows the current speaker, if any
    if (_currentSpeaker && _game.cameraType() == CameraType::Dialog) {
        auto camera = _game.module()->area()->getCamera<DialogCamera>(CameraType::Dialog);
        camera->setSpeakerPosition(getTalkPosition(*_currentSpeaker));
    }
}

} // namespace game

} // namespace reone
