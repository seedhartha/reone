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

#include <unordered_map>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "SDL2/SDL_timer.h"

#include "../../audio/files.h"
#include "../../audio/player.h"
#include "../../audio/soundhandle.h"
#include "../../common/log.h"
#include "../../common/random.h"
#include "../../gui/control/listbox.h"
#include "../../gui/control/panel.h"
#include "../../graphics/model/models.h"
#include "../../resource/resources.h"
#include "../../scene/types.h"
#include "../../script/execution.h"

#include "../game.h"
#include "../objectconverter.h"
#include "../script/routines.h"

#include "colorutil.h"

using namespace std;

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
static const char kControlTagMessage[] = "LBL_MESSAGE";
static const char kControlTagReplies[] = "LB_REPLIES";

static const char kObjectTagOwner[] = "owner";

static const unordered_map<string, AnimationType> g_animTypeByName {
    { "dead", AnimationType::LoopingDead },
    { "taunt", AnimationType::FireForgetTaunt },
    { "greeting", AnimationType::FireForgetGreeting },
    { "listen", AnimationType::LoopingListen },
    { "worship", AnimationType::LoopingWorship },
    { "salute", AnimationType::FireForgetSalute },
    { "bow", AnimationType::FireForgetBow },
    { "talk_normal", AnimationType::LoopingTalkNormal },
    { "talk_pleading", AnimationType::LoopingTalkPleading },
    { "talk_forceful", AnimationType::LoopingTalkForceful },
    { "talk_laughing", AnimationType::LoopingTalkLaughing },
    { "talk_sad", AnimationType::LoopingTalkSad },
    { "victory", AnimationType::FireForgetVictory1 },
    { "scratch_head", AnimationType::FireForgetPauseScratchHead },
    { "drunk", AnimationType::LoopingPauseDrunk },
    { "inject", AnimationType::FireForgetInject },
    { "flirt", AnimationType::LoopingFlirt },
    { "use_computer_lp", AnimationType::LoopingUseComputer },
    { "horror", AnimationType::LoopingHorror },
    { "use_computer", AnimationType::FireForgetUseComputer },
    { "persuade", AnimationType::FireForgetPersuade },
    { "activate", AnimationType::FireForgetActivate },
    { "sleep", AnimationType::LoopingSleep },
    { "prone", AnimationType::LoopingProne },
    { "ready", AnimationType::LoopingReady },
    { "pause", AnimationType::LoopingPause },
    { "choked", AnimationType::LoopingChoke },
    { "talk_injured", AnimationType::LoopingTalkInjured },
    { "listen_injured", AnimationType::LoopingListenInjured },
    { "kneel_talk_angry", AnimationType::LoopingKneelTalkAngry },
    { "kneel_talk_sad", AnimationType::LoopingKneelTalkSad }
};

DialogGUI::DialogGUI(Game *game) : Conversation(game) {
    _resRef = getResRef("dialog");
    _scaling = ScalingMode::Stretch;
    _repliesControlTag = kControlTagReplies;
}

void DialogGUI::load() {
    Conversation::load();

    configureMessage();
    configureReplies();
    loadFrames();
}

void DialogGUI::loadFrames() {
    int rootTop = _rootControl->extent().top;
    int messageHeight = getControl(kControlTagMessage).extent().height;

    addFrame(kControlTagTopFrame, -rootTop, messageHeight);
    addFrame(kControlTagBottomFrame, 0, _gfxOpts.height - rootTop);
}

void DialogGUI::addFrame(string tag, int top, int height) {
    auto frame = make_unique<Panel>(this);

    Control::Extent extent;
    extent.left = -_rootControl->extent().left;
    extent.top = top;
    extent.width = _gfxOpts.width;
    extent.height = height;

    frame->setExtent(move(extent));
    frame->setBorderFill("blackfill");

    _controlByTag.insert(make_pair(tag, frame.get()));
    _controls.insert(_controls.begin(), move(frame));
}

void DialogGUI::configureMessage() {
    auto &message = getControl(kControlTagMessage);
    message.setExtentTop(-_rootControl->extent().top);
    message.setTextColor(getBaseColor(_gameId));
}

void DialogGUI::configureReplies() {
    auto &replies = getControl<ListBox>(kControlTagReplies);
    replies.setProtoMatchContent(true);

    Control &protoItem = replies.protoItem();
    protoItem.setHilightColor(getHilightColor(_gameId));
    protoItem.setTextColor(getBaseColor(_gameId));
}

void DialogGUI::onStart() {
    _currentSpeaker = _owner;
    loadStuntParticipants();

    auto &camera = _game->module()->area()->getCamera<AnimatedCamera>(CameraType::Animated);
    camera.setModel(_cameraModel);
}

void DialogGUI::loadStuntParticipants() {
    if (!_dialog->isAnimatedCutscene()) return;

    _participantByTag.clear();

    for (auto &stunt : _dialog->stunts()) {
        shared_ptr<Creature> creature;
        if (stunt.participant == kObjectTagOwner) {
            creature = ObjectConverter::toCreature(_owner);
        } else {
            creature = ObjectConverter::toCreature(_game->module()->area()->getObjectByTag(stunt.participant));
        }
        if (!creature) {
            warn("Dialog: participant creature not found by tag: " + stunt.participant);
            continue;
        }
        Participant participant;
        participant.creature = creature;

        if (_dialog->isAnimatedCutscene()) {
            shared_ptr<Model> model(Models::instance().get(stunt.stuntModel));
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
    hideControl(kControlTagReplies);
}

void DialogGUI::loadCurrentSpeaker() {
    shared_ptr<Area> area(_game->module()->area());
    shared_ptr<SpatialObject> speaker;

    if (!_currentEntry->speaker.empty()) {
        speaker = area->getObjectByTag(_currentEntry->speaker);
    }
    if (!speaker) {
        speaker = _owner;
    }

    // Make previous speaker stop talking, if any
    if (_currentSpeaker && _currentSpeaker != speaker) {
        auto speakerCreature = ObjectConverter::toCreature(_currentSpeaker);
        if (speakerCreature) {
            speakerCreature->stopTalking();
        }
    }
    _currentSpeaker = speaker;

    // Make current speaker face the player, and vice versa
    if (_currentSpeaker) {
        shared_ptr<Creature> player(_game->party().player());
        player->face(*_currentSpeaker);

        auto speakerCreature = ObjectConverter::toCreature(_currentSpeaker);
        if (speakerCreature) {
            speakerCreature->startTalking(_lipAnimation);
            speakerCreature->face(*player);
        }
    }
}

void DialogGUI::updateCamera() {
    shared_ptr<Area> area(_game->module()->area());

    if (_dialog->cameraModel().empty()) {
        shared_ptr<Creature> player(_game->party().player());
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
    shared_ptr<ModelSceneNode> model(object.getModelSceneNode());
    if (model) {
        glm::vec3 hookPosition(0.0f);
        if (model->getNodeAbsolutePosition("talkdummy", hookPosition)) {
            return object.position() + hookPosition;
        }
        return model->getCenterOfAABB();
    }

    return object.position();
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
                properties.flags = AnimationFlags::propagateHead;
                properties.scale = 1.0f;
                participant.creature->playAnimation(animation, move(properties));
            }
        } else {
            shared_ptr<Creature> participant;
            if (anim.participant == "owner") {
                participant = ObjectConverter::toCreature(_owner);
            } else {
                participant = ObjectConverter::toCreature(_game->module()->area()->getObjectByTag(anim.participant));
            }
            if (!participant) {
                warn("Dialog: participant creature not found by tag: " + anim.participant);
                continue;
            }
            AnimationType animType = getStuntAnimationType(anim.animation);
            if (animType != AnimationType::Invalid) {
                participant->playAnimation(animType, AnimationProperties::fromFlags(AnimationFlags::propagateHead));
            }
        }
    }
}

string DialogGUI::getStuntAnimationName(int ordinal) const {
    return str(boost::format("cut%03dw") % (ordinal - 1200 + 1));
}

AnimationType DialogGUI::getStuntAnimationType(int ordinal) const {
    shared_ptr<TwoDA> animations(Resources::instance().get2DA("dialoganimations"));
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
    Control &message = getControl(kControlTagMessage);
    Control::Text text(message.text());
    int top;

    if (_entryEnded) {
        text.align = Control::TextAlign::CenterBottom;
        top = -_rootControl->extent().top;
    } else {
        text.align = Control::TextAlign::CenterTop;
        top = getControl(kControlTagReplies).extent().top;
    }

    message.setText(move(text));
    message.setExtentTop(top);
}

void DialogGUI::onFinish() {
    if (_dialog->isAnimatedCutscene()) {
        releaseStuntParticipants();
    }

    // Make current speaker stop talking, if any
    auto speakerCreature = ObjectConverter::toCreature(_currentSpeaker);
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
    showControl(kControlTagReplies);
    updateCamera();
    repositionMessage();
}

void DialogGUI::setMessage(string message) {
    setControlText(kControlTagMessage, message);
}

void DialogGUI::update(float dt) {
    Conversation::update(dt);

    // Dialog camera follows the current speaker, if any
    if (_currentSpeaker && _game->cameraType() == CameraType::Dialog) {
        auto &camera = _game->module()->area()->getCamera<DialogCamera>(CameraType::Dialog);
        camera.setSpeakerPosition(getTalkPosition(*_currentSpeaker));
    }
}

} // namespace game

} // namespace reone
