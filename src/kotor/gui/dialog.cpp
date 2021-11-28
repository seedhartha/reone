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

#include "dialog.h"

#include "../../audio/files.h"
#include "../../audio/player.h"
#include "../../audio/soundhandle.h"
#include "../../common/logutil.h"
#include "../../common/randomutil.h"
#include "../../game/party.h"
#include "../../game/script/routine/routines.h"
#include "../../game/services.h"
#include "../../graphics/models.h"
#include "../../gui/control/panel.h"
#include "../../resource/2da.h"
#include "../../resource/2das.h"
#include "../../scene/types.h"
#include "../../script/execution.h"

#include "../kotor.h"

using namespace std;

using namespace reone::audio;
using namespace reone::game;
using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace kotor {

static const char kControlTagTopFrame[] = "TOP";
static const char kControlTagBottomFrame[] = "BOTTOM";
static const char kObjectTagOwner[] = "owner";

static const unordered_map<string, AnimationType> g_animTypeByName {
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

DialogGUI::DialogGUI(KotOR &game, Services &services) :
    Conversation(game, services) {
    _resRef = getResRef("dialog");
    _scaling = ScalingMode::Stretch;
}

void DialogGUI::load() {
    Conversation::load();
    bindControls();
    configureMessage();
    configureReplies();
    loadFrames();

    _binding.lbReplies->setOnItemClick([this](const string &item) {
        int replyIdx = stoi(item);
        pickReply(replyIdx);
    });
}

void DialogGUI::bindControls() {
    _binding.lblMessage = getControl<Label>("LBL_MESSAGE");
    _binding.lbReplies = getControl<ListBox>("LB_REPLIES");
}

void DialogGUI::loadFrames() {
    int rootTop = _rootControl->extent().top;
    int messageHeight = _binding.lblMessage->extent().height;

    addFrame(kControlTagTopFrame, -rootTop, messageHeight);
    addFrame(kControlTagBottomFrame, 0, _options.height - rootTop);
}

void DialogGUI::addFrame(string tag, int top, int height) {
    auto frame = newControl(ControlType::Panel, tag);

    Control::Extent extent;
    extent.left = -_rootControl->extent().left;
    extent.top = top;
    extent.width = _options.width;
    extent.height = height;

    frame->setExtent(move(extent));
    frame->setBorderFill("blackfill");

    _controlByTag.insert(make_pair(tag, frame.get()));
    _controls.insert(_controls.begin(), move(frame));
}

void DialogGUI::configureMessage() {
    _binding.lblMessage->setExtentTop(-_rootControl->extent().top);
    _binding.lblMessage->setTextColor(_game.getGUIColorBase());
}

void DialogGUI::configureReplies() {
    _binding.lbReplies->setProtoMatchContent(true);
    _binding.lbReplies->protoItem().setHilightColor(_game.getGUIColorHilight());
    _binding.lbReplies->protoItem().setTextColor(_game.getGUIColorBase());
}

void DialogGUI::onStart() {
    _currentSpeaker = _owner;
    loadStuntParticipants();

    auto &camera = _game.module()->area()->getCamera<AnimatedCamera>(CameraType::Animated);
    camera.setModel(_cameraModel);
}

void DialogGUI::loadStuntParticipants() {
    if (!_dialog->isAnimatedCutscene())
        return;

    _participantByTag.clear();

    for (auto &stunt : _dialog->stunts()) {
        shared_ptr<Creature> creature;
        if (stunt.participant == kObjectTagOwner) {
            creature = dynamic_pointer_cast<Creature>(_owner);
        } else {
            creature = dynamic_pointer_cast<Creature>(_game.module()->area()->getObjectByTag(stunt.participant));
        }
        if (!creature) {
            warn("Dialog: participant creature not found by tag: " + stunt.participant);
            continue;
        }
        Participant participant;
        participant.creature = creature;

        if (_dialog->isAnimatedCutscene()) {
            shared_ptr<Model> model(_services.models.get(stunt.stuntModel));
            if (!model) {
                warn("Dialog: stunt model not found: " + stunt.stuntModel);
                continue;
            }
            participant.model = model;
            creature->startStuntMode();
        }

        _participantByTag.insert(make_pair(stunt.participant, move(participant)));
    }
}

void DialogGUI::onLoadEntry() {
    loadCurrentSpeaker();
    updateCamera();
    updateParticipantAnimations();
    repositionMessage();

    _binding.lbReplies->setVisible(false);
}

void DialogGUI::loadCurrentSpeaker() {
    shared_ptr<Area> area(_game.module()->area());
    shared_ptr<SpatialObject> speaker;

    if (!_currentEntry->speaker.empty()) {
        speaker = area->getObjectByTag(_currentEntry->speaker);
    }
    if (!speaker) {
        speaker = _owner;
    }

    // Make previous speaker stop talking, if any
    if (_currentSpeaker && _currentSpeaker != speaker) {
        auto speakerCreature = dynamic_pointer_cast<Creature>(_currentSpeaker);
        if (speakerCreature) {
            speakerCreature->stopTalking();
        }
    }
    _currentSpeaker = speaker;

    // Make current speaker face the player, and vice versa
    if (_currentSpeaker) {
        shared_ptr<Creature> player(_game.party().player());
        player->face(*_currentSpeaker);

        auto speakerCreature = dynamic_pointer_cast<Creature>(_currentSpeaker);
        if (speakerCreature) {
            speakerCreature->startTalking(_lipAnimation);
            speakerCreature->face(*player);
        }
    }
}

void DialogGUI::updateCamera() {
    shared_ptr<Area> area(_game.module()->area());

    if (_dialog->cameraModel().empty()) {
        shared_ptr<Creature> player(_game.party().player());
        glm::vec3 listenerPosition(player ? getTalkPosition(*player) : glm::vec3(0.0f));
        glm::vec3 speakerPosition(_currentSpeaker ? getTalkPosition(*_currentSpeaker) : glm::vec3(0.0f));
        auto &camera = area->getCamera<DialogCamera>(CameraType::Dialog);
        camera.setListenerPosition(listenerPosition);
        camera.setSpeakerPosition(speakerPosition);
        camera.setVariant(getRandomCameraVariant());
    } else {
        auto &camera = area->getCamera<AnimatedCamera>(CameraType::Animated);
        camera.setFieldOfView(_currentEntry->camFieldOfView != 0.0f ? _currentEntry->camFieldOfView : kDefaultAnimCamFOV);
        camera.playAnimation(_currentEntry->cameraAnimation);
    }
}

glm::vec3 DialogGUI::getTalkPosition(const SpatialObject &object) const {
    auto model = static_pointer_cast<ModelSceneNode>(object.sceneNode());
    if (!model)
        return object.position();

    shared_ptr<ModelNode> talkDummy(model->model().getNodeByNameRecursive("talkdummy"));
    if (!talkDummy)
        return model->getWorldCenterOfAABB();

    return (model->absoluteTransform() * talkDummy->absoluteTransform())[3];
}

DialogCamera::Variant DialogGUI::getRandomCameraVariant() const {
    int r = random(0, 2);
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
            string animName(getStuntAnimationName(anim.animation));
            shared_ptr<Animation> animation(participant.model->getAnimation(animName));
            if (animation) {
                AnimationProperties properties;
                properties.scale = 1.0f;
                participant.creature->playAnimation(animation, move(properties));
            }
        } else {
            shared_ptr<Creature> participant;
            if (anim.participant == "owner") {
                participant = dynamic_pointer_cast<Creature>(_owner);
            } else {
                participant = dynamic_pointer_cast<Creature>(_game.module()->area()->getObjectByTag(anim.participant));
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

string DialogGUI::getStuntAnimationName(int ordinal) const {
    return str(boost::format("cut%03dw") % (ordinal - 1200 + 1));
}

AnimationType DialogGUI::getStuntAnimationType(int ordinal) const {
    shared_ptr<TwoDA> animations(_services.twoDas.get("dialoganimations"));
    int index = ordinal - 10000;

    if (index < 0 || index >= animations->getRowCount()) {
        warn("Dialog: animation index out of bounds: " + to_string(index));
        return AnimationType::Invalid;
    }

    string name(boost::to_lower_copy(animations->getString(index, "name")));
    auto maybeAnimType = g_animTypeByName.find(name);

    return maybeAnimType != g_animTypeByName.end() ? maybeAnimType->second : AnimationType::Invalid;
}

void DialogGUI::repositionMessage() {
    Control::Text text(_binding.lblMessage->text());
    int top;

    if (_entryEnded) {
        text.align = Control::TextAlign::CenterBottom;
        top = -_rootControl->extent().top;
    } else {
        text.align = Control::TextAlign::CenterTop;
        top = _binding.lbReplies->extent().top;
    }

    _binding.lblMessage->setText(move(text));
    _binding.lblMessage->setExtentTop(top);
}

void DialogGUI::onFinish() {
    if (_dialog->isAnimatedCutscene()) {
        releaseStuntParticipants();
    }

    // Make current speaker stop talking, if any
    auto speakerCreature = dynamic_pointer_cast<Creature>(_currentSpeaker);
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
    _binding.lbReplies->setVisible(true);

    updateCamera();
    repositionMessage();
}

void DialogGUI::setMessage(string message) {
    _binding.lblMessage->setTextMessage(message);
}

void DialogGUI::setReplyLines(vector<string> lines) {
    _binding.lbReplies->clearItems();

    for (size_t i = 0; i < lines.size(); ++i) {
        ListBox::Item item;
        item.tag = to_string(i);
        item.text = lines[i];
        _binding.lbReplies->addItem(move(item));
    }
}

void DialogGUI::update(float dt) {
    Conversation::update(dt);

    // Dialog camera follows the current speaker, if any
    if (_currentSpeaker && _game.cameraType() == CameraType::Dialog) {
        auto &camera = _game.module()->area()->getCamera<DialogCamera>(CameraType::Dialog);
        camera.setSpeakerPosition(getTalkPosition(*_currentSpeaker));
    }
}

} // namespace kotor

} // namespace reone
