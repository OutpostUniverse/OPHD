# OutpostHD Readme
OutpostHD is a reimplementation of OUTPOST published by Sierra On-Line in 1994. It is not a clone. It's a complete redesign of OUTPOST taking the core concepts and reimplementing them in a much more fun and more responsive game.

For up to the minute information, please visit [The Outpost Universe's Forums](http://forum.outpost2.net). You may also view [OutpostHD's Forum Thread](http://forum.outpost2.net/index.php/topic,5718.0.html) for additional details or to participate in the discussion.

## Playing

OutpostHD is still a work in progress with lots of active development, so things may still be a bit rough and incomplete (not entirely unlike the original). To give it a try, head over to the [Releases page](https://github.com/OutpostUniverse/OPHD/releases) and download a recent copy.

You may also be interested in reading the [Player Guide](PlayerGuide.md).

## Installation
Installation is simple: Extract the contents of the ZIP Archive into any folder. Double-click on OPHD.exe to launch the game.

As this is not an installer package it is not distributed with any necessary dependencies. Please see the following sections for the requirements needed for your particular OS of choice.

### Windows Dependencies
OutpostHD is built in C++ using Microsoft's Visual Studio. If you haven't already, you may need to download and install the 64-bit [Visual C++ Redistributable](https://docs.microsoft.com/en-US/cpp/windows/latest-supported-vc-redist?view=msvc-170) package from Microsoft.

### MacOS X Dependencies
MacOS X applications are generally distributed as App Bundles which include all necessary dependencies. OutpostHD doesn't yet have a MacOS maintainer that can put together official app Bundles together.

In the mean time, for users of MacOS 10.14 (Mojave) and above, you can install all of OPHD's dependencies using Homebrew (https://brew.sh -- follow instructions for installation) and then run the following command in terminal:

    brew install sdl2 sdl2_image sdl2_mixer sdl2_ttf libpng libjpeg libtiff webp libmodplug libvorbis libogg freetype glew googletest

It will take a few minutes but all dependencies will be installed. From here you can clone the OPHD git repository and run `make` in terminal to build OutpostHD. When built, you can launch OutpostHD by entering `./ophd.exe` from the terminal.

You can make OPHD double-clickable to run by opening the File Info window and renaming it from 'ophd.exe' to simply 'ophd'. This is an artifact of the makefile configuration that we're looking into solving.

### Linux
OutpostHD does build and run under Linux. Some of the contributing maintainers have been able to build and run OPHD on Arch Linux and Ubuntu Linux. No official Linux build is available yet but we're working on it!

## Configuration
At the moment there are very few configurable options for OutpostHD. All of it must be done via the XML configuration file (config.xml) located in your User directory.

The User directory will vary from platform to platform. On Windows, for example, you'll find it under "C:\Users\Your Username\AppData\Roaming\LairWorks\OutpostHD". This will be different on Linux and macOS.

Note that the file will only be generated after running the game for the first time.

## Troubleshooting
OutpostHD is still under heavy development so there are likely kinks to work out. If you're having trouble, head on over to [The Outpost Universe Forums](http://forum.outpost2.net) or the [Outpost Universe Discord Server](https://discord.gg/kDz5Q3t) for help getting OutpostHD running.

## License
OutpostHD is distributed under a BSD 3-clause license. See LICENSE.txt for details.

## Maintainers
OutpostHD is developed and maintained by the following contributors:

- Leeor Dicker (aka leeor_net): *Design & Programming*
- Hooman: *Programming & System Engineering*
- White Claw: *Graphics*

## Special Thanks
I'd like to give a special thanks and shout out to the following contributors who have helped to shape the direction of OutpostHD and provide extremely valuable insight in alphabetical order:

- belgianguy
- dave_erald
- FallTime
- Goof
- havkyp
- Inglonias
- JetMech1999
- lhark
- lordpalandus
- macksting
- singthemuse
- Sirbomber
- Vagabond
- vomov

Thanks for your contributions! I tried to include everybody who's provided feedback or contributed to the code and other resources. This is a community effort and OPHD would never have come this far without your help! If I missed you, let me know! You deserve the recognition!
