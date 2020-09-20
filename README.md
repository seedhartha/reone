# reone

[![Build Status](https://travis-ci.com/seedhartha/reone.svg?branch=master)](https://travis-ci.com/seedhartha/reone)

reone is a free and open source game engine, capable of running Star Wars: Knights of the Old Republic and its sequel, The Sith Lords. We aim to provide an engine that is flexible, performant and portable.

Currently, the most fundamental engine subsystems are already implemented. Actual gameplay, however, is very limited. See project [gallery](https://github.com/seedhartha/reone/wiki/Gallery) and [roadmap](https://github.com/seedhartha/reone/wiki/Roadmap) in the Wiki.

## Similar projects

reone is not the only KotOR recreation project out there. Below is my opinion, as the project leader, on alternative engines:

* [xoreos](https://github.com/xoreos/xoreos) – a much more ambitious and a more established project, which I had contributed to and use alot as a reference. It is, however, somewhat bloated, unoptimized and is not very active anymore.
* [KotOR.js](https://github.com/KobaltBlu/KotOR.js) – while a fascinating undertaking, I do not believe JavaScript to be the right tool for the job. Still, very useful to have as a reference.
* [KotOR-Unity](https://github.com/rwc4301/KotOR-Unity) – yet to be investigated, but my personal preference is for C++ and a custom rendering engine.

## Installation

Those wishing to test reone, can either download a prebuilt executable from the [releases](https://github.com/seedhartha/reone/releases) page, or build it from source. Building it requires a modern C++ compiler and a set of free and open source libraries. See detailed instructions for your operating system [here](https://github.com/seedhartha/reone/wiki/Installation).

Install [Visual C++ Redistributable](https://aka.ms/vs/16/release/vc_redist.x64.exe) to run prebuilt Windows executables.

You must legally own a game to run it using reone – we do not endorse piracy.

## Usage

Specify a game path to run the game, e.g. `reone --game C:\swkotor`

Controls:
* Use WASD keys to move around
* Click on doors to open them

## Configuration

reone can be configured from either a command line or a configuration file. Configuration file, named "reone.cfg", must be located in the current directory. See complete list of options [here](https://github.com/seedhartha/reone/wiki/Program-options).

## Contribution

reone is in the early stages of development at the moment, meaning that the codebase is not entirely stable. Nevertheless, the project is open to potential contributors. Take a look at the [roadmap](https://github.com/seedhartha/reone/wiki/Roadmap) and [development](https://github.com/seedhartha/reone/wiki/Development) pages in the Wiki if you want to contribute.
