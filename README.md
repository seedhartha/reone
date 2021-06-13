# reone

[![Build Status](https://github.com/seedhartha/reone/actions/workflows/cmake.yml/badge.svg)](https://github.com/seedhartha/reone/actions/workflows/cmake.yml)

reone is a free and open source game engine, capable of running Star Wars: Knights of the Old Republic and its sequel, The Sith Lords. We aim to provide an engine that is flexible and modern, and can be used to:

- Play KotOR and TSL from beginning to end
- Create mods that drastically change the gameplay or improve the visuals

The project is at a mid-stage of development: most engine subsystems have already been implemented, while game mechanics are actively being worked on. See project [gallery](https://github.com/seedhartha/reone/wiki/Gallery) and [roadmap](https://github.com/seedhartha/reone/wiki/Roadmap) in the Wiki.

## Legal Stance

reone adheres to clean-room design principles. We do not condone the use of decompilation - instead, we rely on publicly available information (e.g., file format specification provided by BioWare) and our own observations of the game and its resources.

We do not distribute copyrighted materials - instead, we require our users and developers to obtain a legal copy of the game.

Lastly, reone is explicitly a non-commercial project: we do not seek to gain profit, neither through donations nor other means.

## Contributing

reone is open to potential contributors. Contact us on [this](https://discord.gg/6sqSyfn8Jp) Discord server if you're interested in taking a particular task from the [roadmap](https://github.com/seedhartha/reone/wiki/Roadmap).

Developer resources:

- [Project Architecture](https://github.com/seedhartha/reone/wiki/Project-Architecture)
- [Code Style](https://github.com/seedhartha/reone/wiki/Code-Style)
- [Tooling](https://github.com/seedhartha/reone/wiki/Tooling)
- [Modding](https://github.com/seedhartha/reone/wiki/Modding)
- [Knowledge Database](https://github.com/seedhartha/reone/wiki/Knowledge-Database)

## Installation

Those wishing to test reone, can either download a prebuilt executable from the [releases](https://github.com/seedhartha/reone/releases) page, or build it from source. Building it requires a modern C++ compiler and a set of free and open source libraries. See detailed instructions for your operating system [here](https://github.com/seedhartha/reone/wiki/Installation).

reone must only be used to run a legal copy of the game - both KotOR and TSL can be obtained from Steam or GOG.com. Keep in mind that mod compatibility is not being tested and thus cannot be guaranteed.

Install [Visual C++ Redistributable](https://aka.ms/vs/16/release/vc_redist.x64.exe) to run prebuilt Windows executables.

## Usage

Launch reone using the launcher application or from the command line:

`reone --game C:\swkotor`

## Configuration

reone can be configured via the launcher, the configuration file or the command line.

Configuration file, named "reone.cfg", must be located in the current directory. See complete list of program options [here](https://github.com/seedhartha/reone/wiki/Program-options).

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
