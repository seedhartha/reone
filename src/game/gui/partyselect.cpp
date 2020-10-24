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

#include "partyselect.h"

#include "../../system/gui/control/label.h"
#include "../../system/resource/resources.h"

#include "../game.h"
#include "../portraits.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

static map<int, string> g_portraitByAppearance = {
    { 378, "po_ptrask"}
};

PartySelection::PartySelection(Game *game, GameVersion version, const GraphicsOptions &opts) :
    GUI(version, opts),
    _game(game) {

    _resRef = getResRef("partyselection");
    _backgroundType = BackgroundType::Menu;
}

void PartySelection::load() {
    GUI::load();
}

void PartySelection::update() {
    Party &party = _game->party();

    for (int i = 0; i < kNpcCount; ++i) {
        Label &lblChar = static_cast<Label &>(getControl("LBL_CHAR" + to_string(i)));
        Label &lblNa = static_cast<Label &>(getControl("LBL_NA" + to_string(i)));

        if (party.isMemberAvailable(i)) {
            string blueprintResRef(party.getAvailableMember(i));

            shared_ptr<CreatureBlueprint> blueprint(Resources.findCreatureBlueprint(blueprintResRef));
            int appearance = blueprint->appearance();

            string portrait;

            auto maybePortrait = g_portraitByAppearance.find(appearance);
            if (maybePortrait != g_portraitByAppearance.end()) {
                portrait = maybePortrait->second;
            } else {
                portrait = findPortrait(blueprint->appearance());
            }

            lblChar.setBorderFill(Resources.findTexture(portrait, TextureType::GUI));
            lblNa.setVisible(false);

        } else {
            lblChar.setBorderFill(shared_ptr<Texture>(nullptr));
            lblNa.setVisible(true);
        }
    }
}

void PartySelection::onClick(const string &control) {
    if (control == "BTN_ACCEPT") {
    } else if (control == "BTN_DONE") {
        _game->openInGame();
    } else if (control == "BTN_BACK") {
        _game->openInGame();
    }
}

} // namespace game

} // namespace reone
