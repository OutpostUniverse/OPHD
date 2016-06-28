# OutpostHD Readme
OutpostHD is a remake of the original Outpost published by Siera On-Line in 1994. It is not a clone. It is complete redesign of the game taking the core concepts and build the game that Outpost should have been.

For up to the minute information, please visit The Outpost Universe's [online forum](http://forum.outpost2.net). You may also view [OutpostHD's Forum Thread](http://forum.outpost2.net/index.php/topic,5718.0.html) for additional details or to participate in the discussion.

##Requirements
*For Windows*

* Windows 7 (not tested on Vista or XP)
* 2GB RAM
* 5MB Available Hard Drive Space
* Graphics card that can support OpenGL 3.0 & GLSL v1.0 (basically any modern non-Intel GPU)

##Installation
Installation is simple: Extract the contents of the ZIP Archive into any folder. Double-click on OPHD.exe to launch the game.

As this is not an installer package it is not distributed with any necessary dependencies. Please see the following sections for the requirements needed for your particular OS of choice.

###Windows Dependencies
OutpostHD is built using Microsoft's Visual C++ 2015. If you haven't already, you may need to download and install the redistributable package from Microsoft. Follow this link and download the 32-Bit version of the Visual C++ runtime:

https://www.microsoft.com/en-us/download/details.aspx?id=48145

###MacOS X Dependencies
The only current build of OutpostHD is Windows but as I understand MacOS X development and program distribution, MacOS distributions come as a 'bundle' with all necessary dependencies packaged in.

###Linux
The only current build of OutpostHD is Windows but should, with a few minor modifications to some of its underlying libraries, build a run on Linux. As I have no experience with Linux and providing prebuilt binaries for said system, any and all libraries that OutpostHD links against will need to be installed. In the past I had to do this manually so you may need to as well.

See your distribution's readme files for information on how to download and install the necessary dependencies.

##Configuration
At the moment there are very few configurable options for OutpostHD. All of it must be done via the XML configuration file (config.xml) located in the 'data' subdirectory.

Please use SANE values as there is no sanity checking. You can set the video resolution to whatever you want but if your monitor or graphics card doesn't support it don't expect things to work.

##Troubleshooting
OutpostHD is in an early state of development so there's not much to say other than make sure you have the latest graphics drivers. Head on over to [the forums](http://forum.outpost2.net) for help getting OutpostHD running if you run into trouble.

##FAQ
**Q: When I try to launch OutpostHD, I'm told that I'm missing "msvcp140.dll" and "vcruntime140.dll". How do I fix this?**

A: You are missing some dependencies that OutpostHD requires in order to run. See the section '[Windows Dependencies](#3)' above.

**Q: How do I get started?**

A: Click on the Build Structure button (this is the first button on the bottom left side of your screen that looks like a wrench and screw driver). When you click on this button, the Build Window will be displayed showing you what structures are available. On your first turn, the only available option is the SEED Lander. When you left click on this, the mouse pointer will change to the diamond shaped 'place structure' pointer. Left click anywhere you think is a suitable landing location.

Keep in mind that the SEED Lander can't be placed within 3 tiles of any of the edges of the map and must have all tiles surrounding it be free of obstructions like Mines and Impassable Terrain.

After you place the SEED Lander, the Turns icon will become available to click on (you must place a SEED Lander before you can do anything else). After you left click on the Advance Turn button, other options will become available to you and the first structures of your colony will be built.

**Q: The resolution is low or how can I run in full screen mode? **

A: By default OutpostHD sets itself to 800x600 windowed mode. Eventually you'll be able to make changes in the game via an options menu but for now if you want to change the resolution you can open "data/config.xml" and modify the screenwidth and screenheight values. There is no sanity checking so set this to a sane value or you could get weird behavior including crashing.

To set fullscreen mode, simply change 'fullscreen="false"' to 'fullscreen="true"'.

**Q: I have a question that's not answered here. How can I get help?**

A: You can visit The Outpost Universe's Forums (http://forum.outpost2.net) and visit the Main Projects forum.

Alternatively, you can hop onto IRC and join OPU's official IRC channel and ask for help there. The lead developer, leeor_net, is often available during the day (US Eastern Time, UTC-5:00). (IRC Server: irc.quakenet.org, channel #Outpost2).

##Maintainers
OutpostHD is developed and maintained by the following contributors:

- Leeor Dicker (aka leeor_net): *Design & Programming*
- Hooman: *Design & Programming Assistance*
- Casper van Heck (aka vomov): *Research Tree & Design*