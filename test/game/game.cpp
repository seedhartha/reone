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

#include <boost/test/unit_test.hpp>

#include "reone/game/game.h"
#include "reone/system/stream/fileoutput.h"

#include "../fixtures/audio.h"
#include "../fixtures/engine.h"
#include "../fixtures/game.h"
#include "../fixtures/graphics.h"
#include "../fixtures/movie.h"
#include "../fixtures/resource.h"
#include "../fixtures/scene.h"
#include "../fixtures/script.h"
#include "../fixtures/system.h"

using namespace std;

using namespace reone;
using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::movie;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

using namespace testing;

BOOST_AUTO_TEST_SUITE(game)

BOOST_AUTO_TEST_CASE(should_play_legal_movie_on_launch) {
    // given
    auto gamePath = boost::filesystem::temp_directory_path();
    gamePath.append("reone_test_game");
    boost::filesystem::create_directory(gamePath);

    auto modulesPath = gamePath;
    modulesPath.append("modules");
    boost::filesystem::create_directory(modulesPath);

    auto engine = make_unique<TestEngine>();
    engine->init();

    auto game = Game(GameID::KotOR, gamePath, engine->options(), engine->services());
    game.init();

    auto legalMovie = make_shared<Movie>(engine->services().graphics, engine->services().audio);
    engine->movieModule().movies().whenGetThenReturn("legal", legalMovie);

    EXPECT_CALL(engine->graphicsModule().window(), isInFocus())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(engine->graphicsModule().window(), processEvents(_))
        .WillOnce(Invoke([](bool &quit) { quit = false; }))
        .WillRepeatedly(Invoke([](bool &quit) { quit = true; }));

    // when
    game.run();

    // then
    BOOST_TEST((game.currentScreen() == Game::Screen::MainMenu));
    auto movie = game.movie();
    BOOST_TEST(static_cast<bool>(movie));
    BOOST_TEST((movie == legalMovie));
}

BOOST_AUTO_TEST_SUITE_END()
