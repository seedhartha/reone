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
#include "../../render/models.h"
#include "../../resource/resources.h"
#include "../../scene/types.h"
#include "../../script/execution.h"

#include "../game.h"
#include "../script/routines.h"

#include "colorutil.h"

using namespace std;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

static constexpr float kDefaultEntryDuration = 1000.0f;

static const char kControlTagTopFrame[] = "TOP";
static const char kControlTagBottomFrame[] = "BOTTOM";
static const char kControlTagMessage[] = "LBL_MESSAGE";
static const char kControlTagReplies[] = "LB_REPLIES";

static const char kCreatureTagOwner[] = "owner";

static bool g_allEntriesSkippable = false;

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

enum EndEntryFlags {
    kEndEntryOnAnimFinish = 1,
    kEndEntryOnAudioStop = 2
};

DialogGUI::DialogGUI(Game *game) :
    GUI(game->version(), game->options().graphics),
    _game(game) {

    _resRef = getResRef("dialog");
    _scaling = ScalingMode::Stretch;
}

void DialogGUI::load() {
    GUI::load();

    configureMessage();
    configureReplies();
    loadTopFrame();
    loadBottomFrame();
}

void DialogGUI::configureMessage() {
    auto &message = getControl(kControlTagMessage);

    Control::Extent extent(message.extent());
    extent.top = -_rootControl->extent().top;

    message.setExtent(move(extent));
    message.setTextColor(getBaseColor(_version));
}

void DialogGUI::configureReplies() {
    auto &replies = getControl<ListBox>(kControlTagReplies);

    Control &protoItem = replies.protoItem();
    protoItem.setHilightColor(getHilightColor(_version));
    protoItem.setTextColor(getBaseColor(_version));
}

void DialogGUI::loadTopFrame() {
    addFrame(kControlTagTopFrame, -_rootControl->extent().top, getControl(kControlTagMessage).extent().height);
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

void DialogGUI::loadBottomFrame() {
    int rootTop = _rootControl->extent().top;
    int height = _gfxOpts.height - rootTop;

    addFrame(kControlTagBottomFrame, _gfxOpts.height - rootTop - height, height);
}

void DialogGUI::startDialog(const shared_ptr<SpatialObject> &owner, const string &resRef) {
    shared_ptr<GffStruct> dlg(Resources::instance().getGFF(resRef, ResourceType::Dlg));
    if (!dlg) {
        warn("Dialog: dialog not found: " + resRef);
        _game->openInGame();
        return;
    }
    debug("Dialog: start " + resRef);

    // TODO: cache in Dialogs
    _dialog = make_shared<Dialog>();
    _dialog->load(resRef, *dlg);

    _owner = owner;
    _currentSpeaker = _owner;

    loadDialogBackground();
    loadCameraModel();
    loadStuntParticipants();
    loadStartEntry();
}

static BackgroundType getBackgroundType(ComputerType compType) {
    switch (compType) {
        case ComputerType::Rakatan:
            return BackgroundType::Computer1;
        default:
            return BackgroundType::Computer0;
    }
}

void DialogGUI::loadDialogBackground() {
    if (_dialog->conversationType() == ConversationType::Computer) {
        loadBackground(getBackgroundType(_dialog->computerType()));
    } else {
        loadBackground(BackgroundType::None);
    }
}

void DialogGUI::loadCameraModel() {
    string modelResRef(_dialog->cameraModel());
    if (modelResRef.empty()) return;

    auto &camera = static_cast<AnimatedCamera &>(_game->module()->area()->getCamera(CameraType::Animated));
    camera.setModel(modelResRef);
}

void DialogGUI::loadStuntParticipants() {
    if (!_dialog->isAnimatedCutscene()) return;

    _participantByTag.clear();

    for (auto &stunt : _dialog->stunts()) {
        shared_ptr<Creature> creature;
        if (stunt.participant == kCreatureTagOwner) {
            creature = dynamic_pointer_cast<Creature>(_owner);
        } else {
            creature = dynamic_pointer_cast<Creature>(_game->module()->area()->getObjectByTag(stunt.participant));
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

void DialogGUI::loadStartEntry() {
    int entryIdx = -1;
    for (auto &link : _dialog->startEntries()) {
        if (link.active.empty()) {
            entryIdx = link.index;
            break;
        }
        if (checkCondition(link.active)) {
            entryIdx = link.index;
            break;
        }
    }
    if (entryIdx == -1) {
        warn("Dialog: no active entries");
        _game->openInGame();
    } else {
        debug("Dialog: entry selected: " + to_string(entryIdx), 2);
        _currentEntry = make_shared<Dialog::EntryReply>(_dialog->getEntry(entryIdx));
        loadCurrentEntry(true);
    }
}

bool DialogGUI::checkCondition(const string &script) {
    int result = _game->scriptRunner().run(script, _owner->id());
    return result == -1 || result == 1;
}

void DialogGUI::loadCurrentEntry(bool start) {
    if (!_currentEntry->script.empty()) {
        _game->scriptRunner().run(_currentEntry->script, _owner->id());
    }
    setControlText(kControlTagMessage, _currentEntry->text);
    loadCurrentSpeaker();
    playVoiceOver();
    scheduleEndOfEntry();
    loadReplies();
    updateCamera();
    updateParticipantAnimations();

    if (_activeReplies.empty()) {
        debug("Dialog: finish (no active replies)");
        finish();
        return;
    }

    // Dialog is a one-liner if there is exactly one reply with no text and no entries
    bool oneLiner = false;
    if (start && _activeReplies.size() == 1ll) {
        const Dialog::EntryReply &reply = _dialog->getReply(_activeReplies.front());
        oneLiner = reply.text.empty() && reply.entries.empty();
    }
    if (oneLiner) {
        _game->hud().barkBubble().setBarkText(_currentEntry->text, getCurrentEntryDuration());
        debug("Dialog: finish (one-liner)");
        finish();
        return;
    }

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
        auto prevSpeakerCreature = dynamic_pointer_cast<Creature>(_currentSpeaker);
        if (prevSpeakerCreature) {
            prevSpeakerCreature->setTalking(false);
        }
    }
    _currentSpeaker = speaker;

    // Make current speaker face the player, and vice versa
    if (_currentSpeaker) {
        shared_ptr<Creature> player(_game->party().player());
        player->face(*_currentSpeaker);

        auto speakerCreature = dynamic_pointer_cast<Creature>(_currentSpeaker);
        if (speakerCreature) {
            speakerCreature->setTalking(true);
            speakerCreature->face(*player);
        }
    }
}

float DialogGUI::getCurrentEntryDuration() const {
    if (_currentEntry->delay != -1) {
        return 1000.0f * _currentEntry->delay;
    }
    if (_currentVoice) {
        return static_cast<float>(_currentVoice->duration());
    }
    return kDefaultEntryDuration;
}

void DialogGUI::loadReplies() {
    // Select only active replies from the current entry
    _activeReplies.clear();
    for (auto &link : _currentEntry->replies) {
        if (link.active.empty() || checkCondition(link.active)) {
            _activeReplies.push_back(link.index);
        }
    }

    bool singleEmptyReply = false;
    int replyNumber = 0;

    // Populate the replies list box
    auto &replies = getControl<ListBox>("LB_REPLIES");
    replies.clearItems();
    for (auto &replyIdx : _activeReplies) {
        const Dialog::EntryReply &reply = _dialog->getReply(replyIdx);
        string text(reply.text);
        if (text.empty()) {
            if (_activeReplies.size() == 1ll) {
                singleEmptyReply = true;
                break;
            } else {
                text = "[empty]";
            }
        }
        ListBox::Item item;
        item.tag = to_string(replyIdx);
        item.text = str(boost::format("%d. %s") % ++replyNumber % text);
        replies.addItem(move(item));
    }

    // If there is only one reply with no text, pick it automatically when the entry ends
    if (singleEmptyReply) {
        _autoPickReplyIdx = _activeReplies.front();
    }
}

void DialogGUI::updateCamera() {
    shared_ptr<Area> area(_game->module()->area());

    if (_dialog->cameraModel().empty()) {
        shared_ptr<Creature> player(_game->party().player());
        glm::vec3 listenerPosition(player ? getTalkPosition(*player) : glm::vec3(0.0f));
        glm::vec3 speakerPosition(_currentSpeaker ? getTalkPosition(*_currentSpeaker) : glm::vec3(0.0f));
        auto &camera = static_cast<DialogCamera &>(area->getCamera(CameraType::Dialog));
        camera.setListenerPosition(listenerPosition);
        camera.setSpeakerPosition(speakerPosition);
        camera.setVariant(getRandomCameraVariant());
    } else {
        auto &camera = static_cast<AnimatedCamera &>(area->getCamera(CameraType::Animated));
        camera.setFieldOfView(_currentEntry->camFieldOfView != 0.0f ? _currentEntry->camFieldOfView : kDefaultAnimCamFOV);
        camera.playAnimation(_currentEntry->cameraAnimation);
    }
}

glm::vec3 DialogGUI::getTalkPosition(const SpatialObject &object) const {
    shared_ptr<ModelSceneNode> model(object.model());
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

void DialogGUI::playVoiceOver() {
    // Stop previous voice over, if any
    if (_currentVoice) {
        _currentVoice->stop();
        _currentVoice.reset();
    }

    // Play current voice over either from Sound or from VO_ResRef
    shared_ptr<AudioStream> voice;
    if (!_currentEntry->sound.empty()) {
        voice = AudioFiles::instance().get(_currentEntry->sound);
    }
    if (!voice && !_currentEntry->voResRef.empty()) {
        voice = AudioFiles::instance().get(_currentEntry->voResRef);
    }
    if (voice) {
        _currentVoice = AudioPlayer::instance().play(voice, AudioType::Sound);
    }
}

void DialogGUI::scheduleEndOfEntry() {
    _entryEnded = false;
    _endEntryFlags = 0;

    if (!_dialog->cameraModel().empty() && (_currentEntry->waitFlags & kDialogWaitAnimFinish)) {
        _endEntryFlags = kEndEntryOnAnimFinish;
        return;
    }
    if (_currentEntry->delay != -1) {
        _endEntryTimeout = 1000.0f * _currentEntry->delay;
        return;
    }
    if (_currentVoice) {
        _endEntryFlags = kEndEntryOnAudioStop;
        _endEntryTimeout = static_cast<float>(_currentVoice->duration());
        return;
    }
    _endEntryTimeout = kDefaultEntryDuration;
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
                participant.creature->playAnimation(animation);
            }
        } else {
            shared_ptr<Creature> participant;
            if (anim.participant == "owner") {
                participant = dynamic_pointer_cast<Creature>(_owner);
            } else {
                participant = dynamic_pointer_cast<Creature>(_game->module()->area()->getObjectByTag(anim.participant));
            }
            if (!participant) {
                warn("Dialog: participant creature not found by tag: " + anim.participant);
                continue;
            }
            AnimationType animType = getAnimationType(anim.animation);
            if (animType != AnimationType::Invalid) {
                participant->playAnimation(animType);
            }
        }
    }
}

string DialogGUI::getStuntAnimationName(int ordinal) const {
    return str(boost::format("cut%03dw") % (ordinal - 1200 + 1));
}

AnimationType DialogGUI::getAnimationType(int ordinal) const {
    shared_ptr<TwoDaTable> animations(Resources::instance().get2DA("dialoganimations"));
    const vector<TwoDaRow> &rows = animations->rows();
    int index = ordinal - 10000;

    if (index < 0 || index >= static_cast<int>(rows.size())) {
        warn("Dialog: animation index out of bounds: " + to_string(index));
        return AnimationType::Invalid;
    }

    string name(boost::to_lower_copy(rows[index].getString("name")));
    auto maybeAnimType = g_animTypeByName.find(name);

    return maybeAnimType != g_animTypeByName.end() ? maybeAnimType->second : AnimationType::Invalid;
}

void DialogGUI::repositionMessage() {
    Control &message = getControl(kControlTagMessage);
    Control::Extent extent(message.extent());
    Control::Text text(message.text());

    if (_entryEnded) {
        text.align = Control::TextAlign::CenterBottom;
        extent.top = -_rootControl->extent().top;
    } else {
        text.align = Control::TextAlign::CenterTop;
        Control &replies = getControl(kControlTagReplies);
        extent.top = replies.extent().top;
    }

    message.setText(move(text));
    message.setExtent(move(extent));
}

void DialogGUI::finish() {
    if (_dialog->isAnimatedCutscene()) {
        releaseStuntParticipants();
    }
    if (!_dialog->endScript().empty()) {
        _game->scriptRunner().run(_dialog->endScript(), _owner->id());
    }
    if (_currentSpeaker) {
        auto speakerCreature = dynamic_pointer_cast<Creature>(_currentSpeaker);
        if (speakerCreature) {
            speakerCreature->setTalking(false);
        }
    }
    _game->openInGame();
}

void DialogGUI::releaseStuntParticipants() {
    for (auto &participant : _participantByTag) {
        participant.second.creature->stopStuntMode();
    }
}

bool DialogGUI::handle(const SDL_Event &event) {
    if (!_entryEnded &&
        (g_allEntriesSkippable || _dialog->isSkippable()) &&
        event.type == SDL_MOUSEBUTTONDOWN) {

        endCurrentEntry();
        return true;
    }

    return GUI::handle(event);
}

void DialogGUI::endCurrentEntry() {
    _entryEnded = true;

    if (_currentVoice) {
        _currentVoice->stop();
        _currentVoice.reset();
    }
    if (_autoPickReplyIdx != -1) {
        int replyIdx = _autoPickReplyIdx;
        _autoPickReplyIdx = -1;
        pickReply(replyIdx);
    } else {
        showControl(kControlTagReplies);
        updateCamera();
        repositionMessage();
    }
}

void DialogGUI::pickReply(uint32_t index) {
    debug("Dialog: pick reply " + to_string(index), 2);
    const Dialog::EntryReply &reply = _dialog->getReply(index);

    if (!reply.script.empty()) {
        _game->scriptRunner().run(reply.script, _owner->id());
    }
    if (reply.entries.empty()) {
        debug("Dialog: finish (no entries)");
        finish();
        return;
    }
    int entryIdx = -1;

    for (auto &link : reply.entries) {
        if (link.active.empty()) {
            entryIdx = link.index;
            break;
        }
        if (checkCondition(link.active)) {
            entryIdx = link.index;
            break;
        }
    }
    if (entryIdx == -1) {
        debug("Dialog: finish (no entry selected)");
        finish();
    } else {
        debug("Dialog: entry selected: " + to_string(entryIdx), 2);
        _currentEntry = make_shared<Dialog::EntryReply>(_dialog->getEntry(entryIdx));
        loadCurrentEntry();
    }
}

bool DialogGUI::handleKeyUp(SDL_Scancode key) {
    if (!_entryEnded) return false;

    if (key >= SDL_SCANCODE_1 && key <= SDL_SCANCODE_9) {
        auto &replies = getControl<ListBox>(kControlTagReplies);
        int itemIdx = key - SDL_SCANCODE_1;
        if (itemIdx < replies.itemCount()) {
            const ListBox::Item &item = replies.getItemAt(itemIdx);
            onReplyClicked(stoi(item.tag));
            return true;
        }
    }

    return false;
}

void DialogGUI::update(float dt) {
    GUI::update(dt);

    if (_currentSpeaker && _game->cameraType() == CameraType::Dialog) {
        auto &camera = static_cast<DialogCamera &>(_game->module()->area()->getCamera(CameraType::Dialog));
        camera.setSpeakerPosition(getTalkPosition(*_currentSpeaker));
    }
    if (!_entryEnded) {
        bool endOnAnimFinish = (_endEntryFlags & kEndEntryOnAnimFinish) != 0;
        if (endOnAnimFinish) {
            shared_ptr<Area> area(_game->module()->area());
            auto &camera = static_cast<AnimatedCamera &>(area->getCamera(CameraType::Animated));
            if (camera.isAnimationFinished()) {
                endCurrentEntry();
            }
        } else {
            _endEntryTimeout = glm::max(0.0f, _endEntryTimeout - dt);
            if (_endEntryTimeout == 0.0f) {
                endCurrentEntry();
            } else {
                bool endOnAudioStop = (_endEntryFlags & kEndEntryOnAudioStop) != 0;
                bool stopped = _currentVoice && _currentVoice->isStopped();
                if (endOnAudioStop && stopped) {
                    endCurrentEntry();
                }
            }
        }
    }
}

CameraType DialogGUI::getCamera(int &cameraId) const {
    string cameraModel(_dialog->cameraModel());
    if (!cameraModel.empty()) {
        return CameraType::Animated;
    }
    if (_currentEntry->cameraId != -1) {
        cameraId = _currentEntry->cameraId;
        return CameraType::Static;
    }

    return CameraType::Dialog;
}

void DialogGUI::onListBoxItemClick(const string &control, const string &item) {
    if (control == kControlTagReplies) {
        int replyIdx = stoi(item);
        onReplyClicked(replyIdx);
    }
}

void DialogGUI::onReplyClicked(int index) {
    pickReply(index);
}

} // namespace game

} // namespace reone
