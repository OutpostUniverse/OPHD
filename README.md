# OutpostHD Readme
OutpostHD is a reimplementation of OUTPOST published by Sierra On-Line in 1994. It is not a clone. It's a complete redesign of OUTPOST taking the core concepts and reimplementing them in a much more fun and more responsive game.

For up to the minute information, please visit [The Outpost Universe's Forums](http://forum.outpost2.net). You may also view [OutpostHD's Forum Thread](http://forum.outpost2.net/index.php/topic,5718.0.html) for additional details or to participate in the discussion.

## Playing

OutpostHD is still a work in progress with lots of active development, so things may still be a bit rough and incomplete (not entirely unlike the original). To give it a try, head over to the [Releases page](https://github.com/OutpostUniverse/OPHD/releases) and download a recent copy.

You may also be interested in reading the [Player Guide](PlayerGuide.md).

## Installation

### Windows
Installation is simple: Extract the contents of the ZIP Archive into any folder. Double-click on OPHD.exe to launch the game.

As this is not an installer package it is not distributed with any necessary dependencies. Please see the following sections for the requirements needed for your particular OS of choice.

### macOS
Simply download the disk image from the release section, mount it and drag OutpostHD into your Applications folder. If you have an existing installation, overwrite it.

macOS will warn you on launch that the application was downloaded from the Internet and can't be verified. You can go into `System Preferences->Security & Privacy` to override this behavior and allow OutpostHD to run.

### Linux
Use `git` to checkout OutpostHD's repository and then run the `make` command. Depending on your flavor of Linux you may need to install `SDL2`, `SDL2-Image`, `SDL2-Ttf`, `SDL2-Mixer` and `GLEW` libraries. OutpostHD doesn't yet have any officially supported application distributions so building from source is how you'll have to use it.

### FreeBSD
See Linux installation above. The process should be identical.

## Dependencies
### Windows
OutpostHD is built in C++ using Microsoft's Visual Studio. If you haven't already, you may need to download and install the 64-bit [Visual C++ Redistributable](https://docs.microsoft.com/en-US/cpp/windows/latest-supported-vc-redist?view=msvc-170) package from Microsoft.

## Configuration
At the moment there are very few configurable options for OutpostHD. All of it must be done via the XML configuration file (config.xml) located in your User directory.

The User directory will vary from platform to platform. On Windows, for example, you'll find it under "C:\Users\Your Username\AppData\Roaming\LairWorks\OutpostHD". On macOS and Linux, this will generally be in your user Home directory.

Note that the file will only be generated after running the game for the first time.

## Troubleshooting
OutpostHD is still under heavy development so there will be kinks to work out. If you're having trouble, head on over to [The Outpost Universe Forums](http://forum.outpost2.net) or the [Outpost Universe Discord Server](https://discord.gg/kDz5Q3t) for help getting OutpostHD running. The developers will usually respond quickly to questions.

## License
OutpostHD is distributed under a BSD 3-clause license. See LICENSE.md for details.

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
- Laura Hild
- lhark
- lordpalandus
- macksting
- matt-handy
- singthemuse
- Sirbomber
- Vagabond
- vomov

Thanks for your contributions! I tried to include everybody who's provided feedback or contributed to the code and other resources. This is a community effort and OPHD would never have come this far without your help! If I missed you, let me know! You deserve the recognition!
