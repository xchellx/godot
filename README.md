[![Godot Engine logo](/logo.png)](https://godotengine.org)

## Godot Engine

Homepage: https://godotengine.org

#### 2D and 3D cross-platform game engine

Godot Engine is a feature-packed, cross-platform game engine to create 2D and
3D games from a unified interface. It provides a comprehensive set of common
tools, so that users can focus on making games without having to reinvent the
wheel. Games can be exported in one click to a number of platforms, including
the major desktop platforms (Linux, Mac OSX, Windows) as well as mobile
(Android, iOS) and web-based (HTML5) platforms.

#### Free, open source and community-driven

Godot is completely free and open source under the very permissive MIT license.
No strings attached, no royalties, nothing. The users' games are theirs, down
to the last line of engine code. Godot's development is fully independent and
community-driven, empowering users to help shape their engine to match their
expectations. It is supported by the Software Freedom Conservancy
not-for-profit.

Before being open sourced in February 2014, Godot had been developed by Juan
Linietsky and Ariel Manzur (both still maintaining the project) for several
years as an in-house engine, used to publish several work-for-hire titles.

![Screenshot of a 3D scene in Godot Engine](https://raw.githubusercontent.com/godotengine/godot-design/master/screenshots/editor_tps_demo_1920x1080.jpg)

### Getting the engine

#### Binary downloads

Official binaries for the Godot editor and the export templates can be found
[on the homepage](https://godotengine.org/download).

#### Compiling from source

[See the official docs](https://docs.godotengine.org/en/latest/development/compiling/)
for compilation instructions for every supported platform.

### Community and contributing

Godot is not only an engine but an ever-growing community of users and engine
developers. The main community channels are listed [on the homepage](https://godotengine.org/community).

To get in touch with the developers, the best way is to join the
[#godotengine IRC channel](https://webchat.freenode.net/?channels=godotengine)
on Freenode.

To get started contributing to the project, see the [contributing guide](CONTRIBUTING.md).

### Documentation and demos

The official documentation is hosted on [ReadTheDocs](https://docs.godotengine.org).
It is maintained by the Godot community in its own [GitHub repository](https://github.com/godotengine/godot-docs).

The [class reference](https://docs.godotengine.org/en/latest/classes/)
is also accessible from within the engine.

The official demos are maintained in their own [GitHub repository](https://github.com/godotengine/godot-demo-projects)
as well.

There are also a number of other learning resources provided by the community,
such as text and video tutorials, demos, etc. Consult the [community channels](https://godotengine.org/community)
for more info.

[![Travis Build Status](https://travis-ci.org/godotengine/godot.svg?branch=master)](https://travis-ci.org/godotengine/godot)
[![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/bfiihqq6byxsjxxh/branch/master?svg=true)](https://ci.appveyor.com/project/akien-mga/godot)
[![Code Triagers Badge](https://www.codetriage.com/godotengine/godot/badges/users.svg)](https://www.codetriage.com/godotengine/godot)
[![Translate on Weblate](https://hosted.weblate.org/widgets/godot-engine/-/godot/svg-badge.svg)](https://hosted.weblate.org/engage/godot-engine/?utm_source=widget)

### Nintendo Wii Export Template
This specific fork has the ability to export for the Nintendo Wii. Support for this is minimal right now. The code changes are mirrored from [TGRCDev/godot_wii](https://git.tgrc.dev/TGRCDev/godot_wii/). Any exported game is meant to work with [Homebrew Channel](https://wiibrew.org/wiki/Homebrew_Channel). However, you can also boot the executable from [Dolphin Emulator](https://github.com/dolphin-emu/dolphin).

####What is not supported:
- Embedded pck
- Mono (C#)
- Different GX texture formats (only GX RGBA8 is supported for now)
- `PoolByteArray` GDScript class (it is not 32byte aligned yet)
- No Nintendo Wii specific API for GDScript (yet)
- Potentially more things which haven't been tested yet

#### Compiling the Nintendo Wii export template
- `export DEVKITPRO=DEVKITPROPATH`
- pass `dkp_path=DEVKITPROPATH` to SCONS when building.
- `DEVKITPROPATH` should be where devkitPro was installed to, for example: `"/opt/devkitpro"`.

For instructions on installing devkitPro see [Getting Started - devkitPro](https://devkitpro.org/wiki/Getting_Started) and [devkitPro pacman - devkitPro](https://devkitpro.org/wiki/devkitPro_pacman). The packages required are `devkitPPC`, `gamecube-tools`, `libogc`, `libfat-ogc`, `ppc-freetype`, `ppc-libpng`, and `ppc-zlib`. If you meet the dependency requirements for the wii platform, `scons platform=list` will show `wii` in the list.

Here is the command I use `scons platform=wii tools=no target=release bits=32 use_mingw=yes dkp_path=/opt/devkitpro -j6` for both `target=release` and `target=debug`.

*These instructions were written in assumption that you are using MSYS2.*
