# OutpostHD Player Guide

## Key Bindings

F1: Reports view

F2: Save Game

F3: Load Game

F10: Show debug info

PAGE UP: Go up one level

PAGE DOWN: Go down one level

HOME: Go to surface

END: Go to max depth level

ENTER: End turn

ESCAPE: Cancel edit mode.

W / A / S / D: Move one tile NORTH / WEST / SOUTH / EAST

ARROWS: Same as WASD

0: Go to Surface

1: Go to Underground 1

2: Go to Underground 2

3: Go to Underground 3

4: Go to Underground 4

## FAQ

**Q: When I try to launch OutpostHD, I'm told that I'm missing "msvcp140.dll" and "vcruntime140.dll". How do I fix this?**

A: You are missing some dependencies that OutpostHD requires in order to run. You'll need to visit the Microsoft website in order to download and install the [Visual C++ Redistributable](https://docs.microsoft.com/en-US/cpp/windows/latest-supported-vc-redist?view=msvc-170) package.

**Q: How do I get started?**

A: When you first start the game, the only structure available to you is the SEED Lander which can be found in the Structure Panel at the bottom left of the screen. When you select the SEED Lander from the panel, the mouse pointer will change to the diamond shaped 'place structure' pointer. Left click anywhere you think is a suitable landing location.

Keep in mind that the SEED Lander can't be placed within 3 tiles of any of the edges of the map and must have all tiles surrounding it be free of obstructions like Mines and Impassable Terrain.

After you place the SEED Lander, the Advance Turn button will become available to click on (you must place a SEED Lander before you can do anything else). After you left click on the Advance Turn button, other options will become available to you and the first structures of your colony will be built.

**Q: Can I automatically skip the splash/logo screens?**

A: Yes. After the first time the game runs, an automatically generated configuration file is saved in your user directory ('config.xml'). Open this file in a text editor, look for the line that says 'skip-splash' and change 'value' from 'false' to 'true'.

**Q: The keybindings for movement (WASD) don't work well on my keyboard layout, can this be changed?**

A: Not yet; keybindings are hard coded. This will, however, change in the future to configurable keybindings.

**Q: The game generally works but there are some graphics problems like missing or corrupt textures. How can I fix this?**

A: This is a result of some budget GPU drivers attempting and failing to accommodate for texture resolutions that are not a power of two (e.g., 16x16, 32x32, 128x128, etc.). There really isn't a way to fix this as generally these issues show up on budget laptops and mobile devices with integrated Intel GPU's.

This is something that will improve over time -- the game is still under heavy development and a lot changes under the hood. While we've made considerable effort to align all visuals to power of two textures, it's likely we're going to miss some here and there. If you do find textures/images that are blurry or don't look right please point it out on the forums or the Discord server. It'll really help me out when I miss something!

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

A: You can visit [The Outpost Universe Forums](http://forum.outpost2.net) and visit the Main Projects forum or join the [Outpost Universe Discord Server](https://discord.gg/kDz5Q3t).
