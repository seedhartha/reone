/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "util.h"

#include "../audio/player.h"
#include "../resources/resources.h"

using namespace std;

using namespace reone::audio;
using namespace reone::resources;

namespace reone {

namespace game {

shared_ptr<SoundInstance> playMusic(const string &resRef) {
    shared_ptr<AudioStream> stream(Resources.findAudio(resRef));
    return TheAudioPlayer.play(stream, AudioType::Music);
}

} // namespace util

} // namespace reone
