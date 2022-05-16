/*
 * Copyright (c) 2022 The reone project contributors
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
#include "inireader.h"
#include "../../common/logutil.h"

using namespace std;

namespace reone {

namespace resource {

static const char kSoundOpt[] = "[Sound Options]";
static const char kGameOpt[]  = "[Game Options]";
static const char kMoviesSh[] = "[Movies Shown]";
static const char kAlias[]    = "[Alias]";
static const char kGraphicsOpt[] = "[Graphics Options]";
static const char kDisplayOpt[]  = "[Graphics Options]";
static const char kAutoPsOpt[]   = "[Autopause Options]";
static const char kKeymapping[]  = "[Keymapping]";

INIreader::INIreader(){
}

void INIreader::load(boost::filesystem::path path){

    ifstream iniFile(path);
    string line;
    map<string, string> * current=nullptr;
    while(getline(iniFile,line)) {
        if(!line.empty()) {

            //Drop \r at the end
            auto pos = line.find("\r", 0);
            line = line.substr(0,pos);

            if(!line.compare(kSoundOpt))
                current = &_soundOptions;
            else if(!line.compare(kGameOpt))
                current = &_gameOptions;
            else if(!line.compare(kMoviesSh))
                current = &_moviesShown;
            else if(!line.compare(kAlias))
                current = &_alias;
            else if(!line.compare(kGraphicsOpt) || !line.compare(kDisplayOpt))
                current = &_graphicsOptions;
            else if(!line.compare(kAutoPsOpt))
                current = &_autoPauseOptions;
            else if(!line.compare(kKeymapping))
                current = &_keyMappings;
            else {
                if ( current != nullptr) { // If none of the previous headers has been detected, do nothing
                    debug(line.c_str());
                    auto pos = line.find("=", 0);
                    if(pos != string::npos) { //If string is properly displayed
                        string key   = line.substr(0,pos);
                        string value = line.substr(pos+1);
                        (*current)[key] = value; //Store pair in map
                    }

                }
            }
        }
    }

}

} // namespace resource

} // namespace reone
