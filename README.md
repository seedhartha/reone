# reone

[![Build Status](https://travis-ci.com/seedhartha/reone.svg?branch=master)](https://travis-ci.com/seedhartha/reone)

reone is a free and open source game engine, capable of running Star Wars: Knights of the Old Republic and its sequel, The Sith Lords. We aim to provide an engine that is flexible and modern, and can be used to:

- Play KotOR and TSL from beginning to end
- Create mods that drastically change the gameplay or improve the visuals

The project is in the early stages of development at this point: resource management, scene management, rendering, audio, GUI and scripting subsystems are already implemented, while game mechanics are actively being worked on. See project [gallery](https://github.com/seedhartha/reone/wiki/Gallery) and [roadmap](https://github.com/seedhartha/reone/wiki/Roadmap) in the Wiki.

## Contributing

reone is open to potential contributors. Contact me at <vkremianskii@gmail.com> if you're interested in taking a particular task from the [roadmap](https://github.com/seedhartha/reone/wiki/Roadmap).

Developer resources:

- [Project Architecture](https://github.com/seedhartha/reone/wiki/Project-Architecture)
- [Code Style](https://github.com/seedhartha/reone/wiki/Code-Style)
- [Tooling](https://github.com/seedhartha/reone/wiki/Tooling)
- [Research](https://github.com/seedhartha/reone/wiki/Research)
- [Knowledge Database](https://github.com/seedhartha/reone/wiki/Knowledge-Database)

## Installation

Those wishing to test reone, can either download a prebuilt executable from the [releases](https://github.com/seedhartha/reone/releases) page, or build it from source. Building it requires a modern C++ compiler and a set of free and open source libraries. See detailed instructions for your operating system [here](https://github.com/seedhartha/reone/wiki/Installation).

reone must only be used to run a legal copy of the game - both KotOR and TSL can be obtained from Steam or GOG.com. Keep in mind that mod compatibility is not being tested and thus cannot be guaranteed.

Install [Visual C++ Redistributable](https://aka.ms/vs/16/release/vc_redist.x64.exe) to run prebuilt Windows executables.

## Usage

Specify a game path to run the game, e.g. `reone --game C:\swkotor`

Controls:

- Use WASD and ZC keys to move around
- Left click to interact with an object
- Right click and move the mouse to rotate the camera
- Press Space to pause the game
- Press "~" to toggle the debug console

Developer mode controls:

- Press V to switch the camera type
- Use "+" and "-" keys to adjust the game speed
- Press F1-F3 to toggle graphical features
- Use "[" and "]" keys to adjust PBR HDR mode exposure

## Configuration

reone can be configured from either a command line or a configuration file. Configuration file, named "reone.cfg", must be located in the current directory. See a complete list of options [here](https://github.com/seedhartha/reone/wiki/Program-options).

## Similar Projects

reone is not the only project aiming to reimplement the Odyssey engine - there are at least four alternatives:

- [xoreos](https://github.com/xoreos/xoreos) - a reimplementation of BioWare's Aurora engine and its derivatives.
- [KotOR.js](https://github.com/KobaltBlu/KotOR.js) - a remake of the Odyssey engine written in JavaScript.
- [The Northern Lights](https://github.com/lachjames/NorthernLights) - a Unity-based reimplementation and a level editor.
- [KotOR-Unity](https://github.com/rwc4301/KotOR-Unity) - another Unity-based reimplementation.

In contrast, this is what reone offers:

- KotOR and TSL only, unlike xoreos, which aims to reimplement all of Aurora engine games - codebase is lighter and development is more focused.
- Written in C++ - industry standard language, faster than the JavaScript implementation.
- Custom game engine based on SDL 2 and OpenGL 3.3 - does not depend on Unity, which is a proprietary software with a learning curve of itself.
