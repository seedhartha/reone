# reone

reone is a free and open source game engine, capable of running Star Wars: Knights of the Old Republic and its sequel, The Sith Lords. The project aim is to create an engine that is modern, cross-platform, extensible and very mod friendly.

Currently, reone has functional resource management, rendering, audio, GUI and networking subsystems – actual gameplay implementation is very limited. See project [gallery](https://github.com/seedhartha/reone/wiki/Gallery) and [roadmap](https://github.com/seedhartha/reone/wiki/Roadmap) in the Wiki.

The project author is strongly against software piracy. You must legally own a game to run it using reone.

## Installation

Those wishing to test reone, can either download a prebuilt executable from the [releases](https://github.com/seedhartha/reone/releases) page, or build it from source. Building it requires a modern C++ compiler and a set of free and open source libraries. See detailed instructions for your operating system [here](https://github.com/seedhartha/reone/wiki/Installation).

Install [Visual C++ Redistributable](https://aka.ms/vs/16/release/vc_redist.x64.exe) to run prebuilt Windows executables.

## Usage

Specify a game path to run the game, e.g. `reone --game C:\swkotor`

Controls:
* Use WASD keys to move around
* Click on doors to open them
* Press V to toggle camera mode

## Configuration

reone can be configured from either a command line or a configuration file. Configuration file, named "reone.cfg", must be located in the current directory. See complete list of options [here](https://github.com/seedhartha/reone/wiki/Program-options).
