# OutpostHD Readme
OutpostHD is a reimplementation of OUTPOST published by Sierra On-Line in 1994. It is not a clone. It's a complete redesign of OUTPOST taking the core concepts and reimplementing them in a much more fun and more responsive game.

For up to the minute information, please visit The Outpost Universe's [online forum](http://forum.outpost2.net). You may also view [OutpostHD's Forum Thread](http://forum.outpost2.net/index.php/topic,5718.0.html) for additional details or to participate in the discussion.

## Requirements
*For Windows*

* Windows 7 (not tested on Vista or XP but probably works)
* 2GB RAM
* 32MB Available Hard Drive Space
* Graphics card that can support OpenGL 3.0 & GLSL v1.0 (basically any modern GPU)

## Installation
Installation is simple: Extract the contents of the ZIP Archive into any folder. Double-click on OPHD.exe to launch the game.

As this is not an installer package it is not distributed with any necessary dependencies. Please see the following sections for the requirements needed for your particular OS of choice.

### Windows Dependencies
OutpostHD is built using Microsoft's Visual C++ 2017. If you haven't already, you may need to download and install the redistributable package from Microsoft. Follow this link and download the 32-Bit version of the Visual C++ runtime:

https://outpost2.net/files/ophd/VC_redist.x86.exe

### MacOS X Dependencies
The only current build of OutpostHD is Windows but as I understand MacOS X development and program distribution, MacOS distributions come as a 'bundle' with all necessary dependencies packaged in. That stated, there aren't any official osX builds (yet) but we're working on it!

### Linux
OutpostHD does build and run under Linux. Some of the contributing maintainers have been able to build and run OPHD on Arch Linux and Ubuntu Linux. No official Linux build is available yet but we're working on it!

## Configuration
At the moment there are very few configurable options for OutpostHD. All of it must be done via the XML configuration file (config.xml) located in the 'data' subdirectory.

Please use SANE values as there is no sanity checking except for minimum resolution (1000x700). You can set the video resolution to whatever you want but if your monitor or graphics card doesn't support it don't expect things to work.

Audio options are limited to mixrate quality (currently only 11025, 22050 and 44100 are supported, please stay with one of these three options), channels (1 for mono, 2 for stereo), sfxvolume and musicvolume (between 0 and 128) and bufferlength. Aside for volume levels and mixrate, you should leave the rest of the options alone.

When OutpostHD first starts it will start in full screen mode in the native resolution of your desktop. After the first time you run the game, a configuration file will be saved ('data/config.xml'). From there you can change your video resolution and whether or not you want to skip the splash screen.

As a note, in windowed mode, the game window is resizable. Minimum window size is 1000x700 but can theoretically be expanded to any modern resolution (this has been tested up to 1920x1080).

## Troubleshooting
OutpostHD is in an early state of development so there's not much to say other than make sure you have the latest graphics drivers. Head on over to [the forums](http://forum.outpost2.net) for help getting OutpostHD running if you run into trouble.

## Key Bindings

F2: Save Game

F3: Load Game

F10: Show debug info

PAGE UP: Go up one level

PAGE DOWN: Go down one level

HOME: Go to surface

END: Go to max depth level

ENTER: End turn

ESCAPE: Cancel edit mode.

W / A / S / D: Move one tile UP / LEFT / DOWN / RIGHT

ARROWS: Same as WASD

0: Go to Surface

1: Go to Underground 1

2: Go to Underground 2

3: Go to Underground 3

4: Go to Underground 4

## FAQ
**Q: When I try to launch OutpostHD, I'm told that I'm missing "msvcp140.dll" and "vcruntime140.dll". How do I fix this?**

A: You are missing some dependencies that OutpostHD requires in order to run. See the section '[Windows Dependencies](#3)' above.

**Q: How do I get started?**

A: When you first start the game, the only structure available to you is the SEED Lander which can be found in the Structure Panel at the bottom left of the screen. When you select the SEED Lander from the panel, the mouse pointer will change to the diamond shaped 'place structure' pointer. Left click anywhere you think is a suitable landing location.

Keep in mind that the SEED Lander can't be placed within 3 tiles of any of the edges of the map and must have all tiles surrounding it be free of obstructions like Mines and Impassable Terrain.

After you place the SEED Lander, the Advance Turn button will become available to click on (you must place a SEED Lander before you can do anything else). After you left click on the Advance Turn button, other options will become available to you and the first structures of your colony will be built.

**Q: Can I automatically skip the splash/logo screens?**

A: Yes. After the first time the game runs, an automatically generated configuration file is saved in the 'data' folder ('data/config.xml'). Open this file in a text editor, look for the line that says 'skip-splash' and change 'value' from 'false' to 'true'.

**Q: The keybindings for movement (WASD) don't work well on my keyboard layout, can this be changed?**

A: Not yet; keybindings are hard coded. This will, however, change in the future to configurable keybindings.

**Q: The game generally works but there are some graphics problems like missing or corrupt textures. How can I fix this?**

A: This is a result of some budget GPU drivers attempting and failing to accommodate for texture resolutions that are not a power of two (e.g., 16x16, 32x32, 128x128, etc.). There really isn't a way to fix this as generally these issues show up on budget laptops and mobile devices with integrated Intel GPU's.

This is something that will improve over time -- the game is still under heavy development and a lot changes under the hood. While we've made considerable effort to align all visuals to power of two textures, it's likely we're going to miss some here and there. We're working on it though it would be helpful if you could point it out for us on the discussion boards.

**Q: Robots I'm building in the factory never become available. Why?**

A: In order to use the robots that your factory builds, you'll need both a Robot Command Center and a Warehouse with enough space to store the robot.

When robots are produced by a Factory, they are first moved to a Warehouse. On the next turn, they are assigned to a Robot Command Center. Without the Warehouse the Robot will sit, finished, on the Factory assembly line with nowhere to go.

After they're moved to a Warehouse, you'll need to have a Robot Command Center with available slots. When you first start the game, you're given 3 slots for robots (this is for the initial three robots though after you place your miner you can build another robot type to fill the now vacant slot). Each Robot Command Center can handle 10 robots.

**Q: There are mines that are distant from my colony that I can't place robots on. Why?**

A: All robots and structures must be placed within the communications range of the Command Center or a Communications Tower. These are denoted on your minimap as a light blue circle.

You can extend your communications range by building Communications Towers on the edges of your comm range.

**Q: I've built Comm. Towers far away from my colony but I can't place structures, only robots. Why?**

A: Structures must be built within the comm range of a Command Center. This is the large comm circle on your minimap.

**Q: I have a question that's not answered here. How can I get help?**

A: You can visit The Outpost Universe's Forums (http://forum.outpost2.net) and visit the Main Projects forum.

Alternatively, you can hop onto IRC and join OPU's official IRC channel and ask for help there. The lead developer, leeor_net, is often available during the day (US Eastern Time, UTC-5:00). (IRC Server: irc.quakenet.org, channel #Outpost2).

## License
OutpostHD is distributed under a BSD 3-clause license. See LICENSE.txt for details.

## Maintainers
OutpostHD is developed and maintained by the following contributors:

- Leeor Dicker (aka leeor_net): *Design & Programming*
- Goof: *Programming*
- lhark: *Programming*
- White Claw: *Graphics*
- Hooman: *Design & Programming Assistance*
- Casper van Heck (aka vomov): *Research Tree & Design*

## Special Thanks
I'd like to give a special thanks and shout out to the following contributers who have helped to shape the direction of OutpostHD and provide extremely valuable insight in alphabetical order:

- dave_erald
- FallTime
- havkyp
- JetMech1999
- lordpalandus
- macksting
- singthemuse
- Sirbomber
- Vagabond

Thanks for all of your support! Without your feedback I could never have accomplished what I have!
