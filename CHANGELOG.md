# Change Log
This is the changelog for OutpostHD.

## [0.8.5] - 2021-09-26

Introduces integrity decay and the Maintenance Facility.

### Added
- Maintenance Facility
- Structure Integrity & Decay
- Criminal Activity

### Changed
- Structures will now slowly lose integrity (decay) over time
- Decayed roads will negatively affect route traversal times
- Maintenance Facility required to repair decaying buildings
- Improved descriptions of criminal activity

### Fixed
- Factories will no longer push new products into warehouses that aren't operational
- Fixed misreported warehouse usage in full-screen warehouse UI
- Fixed display of tilemap overlays being cleared when turns are run
- Fixed a mistake in route cost calculation that caused extremely low movement of resources
- Fixed an issue that made it impossible to place robots before the Command Center was built
- Fixed an issue that caused roads to be set to an incorrect animation state during loading


## [0.8.3] - 2021-06-6

Introduces the NotificationArea UI element to inform users of non-critical issues or notifications that don't interrupt gameplay and the user can look at and respond to at their leisure.

### Added
- Added a NotificationArea to display notifications
- Added a NotificationWindow to show detailed information about new notifications
- Added RadioButton and RadioButtonGroup
- Added Police Coverage overlay

### Changed
- Certain conditions will now throw exceptions -- these are logic bugs that should be reported and not ignored
- Population changes (births and deaths) will now push notifications to the player
- Robot aging and failure will now push notifications to the player
- Robot self-destruct will now push a notification instead of display a message box
- Structure state changes (construction complete, destroyed, disabled, etc.) will now push notifications to the player

### Fixed
- Log output redirect crash
- Disabled or destroyed Smelters will now be ignored in truck routing
- Assigned trucks will now be saved/loaded properly from save games
- Recycling structure will now be properly highlighted red if insufficient resources are available to build it


## [0.8.2] - 2021-03-24

Introduces communcations range overlays, additional GUI quality of life improvements and a number of bug fixes that correct some subtle behavior issues.

### Added
- Tile highlight overlays for communication ranges
- Tile highlight overlays for truck routes
- Robot Inspector Window -- allows for changing robot orders and issuing robot self destruct

### Changed
- FileIO Window now has a 'delete' button so save games can be deleted from within the game
- Change order of food counting and population update so that landing cargo/colonist landers at the same time doesn't result in an immediate game over
- Planet attribute and definitions moved out of the code and into external XML definition in the data files
- Air shaft now behaves as a four-directional tube
- Recycling Center now shows more information in the Structure Inspector window

### Fixed
- Fixed some computations in the population simulation that caused subtle behavior issues
- Fixed typos in some string literals
- Robots and structures can now be placed at the edges of the diamond map view
- Tile Inspector window can now be opened when right-clicking a tile on the edge
- Fix savegame issue where game breaks when loading a savegame saved on turn 0 before the seed lander is placed


## [0.8.1] - 2021-02-17

Introduces biowaste produced by residential units and some quality of life improvements in the GUI.

### Added
- Recycling Facility
- Biowaste accumulation
- GUI ToolTip's

### Changed
- StructureInspector window now dynamically resizes to accommodate contents
- Middle mouse button and `shift + right mouse button` will now bring up Structure Inspector window regardless of structure type
- Depleted food stores in the Command Center will be refilled each turn from the production of food producing facilities
- Morale will take a take a hit whenever biowaste in residences overflows
- Improve behavior of UI elements in full-screen Mine Operations UI
- Command Center no longer requires any population to operate

### Fixed
- Fix text overlap in Mine Operations Window
- Fix crash when opening the Structure Inspector Window on any structure that employs Scientists
- Fix crash when right-clicking a Robot Command Center
- Fix a mistake that tallied food totals before population consumption which resulted in incorrect value displayed in the HUD
- Fix Mine full-screen UI showing "no routes" when loading a game
- Fix for phantom mine routes being drawn on the Mini Map if you quit to the main menu from an active game and start a new game
- Fix for immediate colony death when landing colonists
- Fix crash when double-clicking after returning to Main Menu from Game State


## [0.8.0] - 2021-01-01

Includes improved mined resource management and a proper implementation of truck routing for mines and mine to smelter routes.

### Added
- Added 64bit build configurations. OutpostHD will now be available as 32-bit and 64-bit builds.
- Truck manufacturing has been implemented.
- Mine Operations Window allows assignment of trucks from Warehouse storage.
- Roads have been added. These are taken into account when calculating transport routes.
- Planets now define a mean distance from the host star.

### Changed
- SaveGame version changes -- saves from older versions of the game are no longer supported.
- A LOT of internal code cleanup and improvements.
- Road Materials have been removed from the game.
- Resources are now moved via trucks from Mine Facilities to Smelters.
- Refined resources are now stored in Storage Tanks instead of in an accumulator value.
- Command Center now stores up to 250 units of Refined Resources.
- Command Center now stores up to 250 units of Food.
- Loss of resources or CHAP is no longer an instant game over as the Command Center now acts as a sort of buffer.
- Solar Panel energy production will vary based on mean distance from the host star.

### Fixed
- Fix a crash when going back to the Main Menu and starting a new game.
- Fix for weird values shown in Residential Capacity reporting.


## [0.7.11] - 2019-03-01

Maintenance release. Addresses usability issues and a number of bugs that have been discovered.

### Added
- Help button to the Main Menu. This launches the OutpostHD Wiki in your default browser.
- Resource breakdown panel when hovering over resource HUD panel.
- Pin 'button' to keep population panel visible even if you're not moused over it.

### Changed
- The AI Voice notifications for errors have been replaced with message boxes providing much greater detail about what the issue actually is.
- Robodozer's that 'die' during their task will no longer leave the tile it was on as 'bulldozed' and instead will revert it to its original terrain type.
- Cargo and Colonist landers are now restricted to landing within 5 tiles of the command center.
- Mine Production Window now shows the remaining time during mine shaft extension.
- Structure Inspector Window now shows remaining turns until construction is complete.
- Structure Inspector Window now shows the current age of a Structure.
- Placing a Robodigger on a Mine will ask the user if they want to sacrifice the mine instead of just automatically placing the robot.
- Robot info HUD will now only draw after the SEED Lander has been placed and the Command Center is available on the map.
- Fullscreen mode has been removed so that OutpostHD plays nicely with the native window manager due to the use of native message boxes.
- Window state is now saved on closing (maximized or not).
- Window size is now saved on closing.
- Reason for a structure going Idle is now detailed in the Structure Inspector window.
- Reason for a structure being Disabled is now detailed in the Structure Inspector window.
- Changing depth level while in Robot insert mode will no longer clear insertion mode.

### Fixed
- Right-clicking on a Warehouse that's under construction will now bring up the Structure Inspector Window instead of the Warehouse Inspector.
- Right-clicking on a Mine Facility that's under construction will now bring up the Structure Inspector Window instead of the Mine Operations Window.
- Fixed a mistake in the Population Panel that displayed housing incorrectly before the the Command Center was built.
- Fixed an issue in the GraphWalker that could leave some tiles marked as connected when they shouldn't be has been resolved. Fix provided by Sirbomber.
- Fixed an issue in MainMenuState that could lead to the UI buttons being clicked inadvertently when skipping the splash screens.
- Fix failure to load when saving a game with a newly built Robot Command Center that doesn't have any robots assigned to it yet.
- Fix an issue that left structures manually set to idle permanently disabled if they lost their connection to the CC even if the connection was restored.


## [0.7.10] - 2019-01-29

Maintenance release.

### Changed
- RobotCommand::addRobot() will no longer throw an exception if a robot appears to match an already existing robot. It will instead write this to the log and request that the user report the issue to the developer.

### Fixed
- Fixed an oversight which neglected to remove a dead robot from a Robot Command Center. This would lead to an abrupt program termination with the message "RobotCommand::addRobot(): Adding a robot that is already under the command of this Robot Command Facility."
- Fixed a logic mistake that failed to draw the mouse tile highlight when the connectedness overlay was turned on.
- Fixed an issue which caused energy usage being displayed incorrectly when a savegame is loaded.
- Fixed a mistake which allowed the player to place Colonist and Cargo landers on Turn 0 if the SEED Lander had been placed.


## [0.7.9] - 2019-01-20

User interface overhaul and maintenance.

### Added
- CheckBox Control.
- Full-screen Factory/Production UI.
- Full-screen Warehouse UI.
- Added an unencumbered classical recording of Mars, the Bringer of War.

### Changed
- MineOperationsWindow now uses CheckBox controls instead of toggle buttons.
- Double-clicking on Factories will pull up the full-screen UI.
- Double-clicking on Warehouses will pull up the full-screen UI.
- Lots of internal cleanup that should improve memory usage.
- Tweaked the SplashScreen state animation.
- SplashScreen and MainMenu states now use the Mars sound track in the background.
- Setting minimum resolution to 1000x700. I know, kind of a weird one but this is intended mostly for windowed modes to run properly on low-resolution displays (1366x768).
- Structures that are disabled now indicate why they were disabled.
- Buldozing a mine facility on top of a completely exhausted mine will also remove all of the mine shafts underneath.
- Bulldozing a Warehouse will ask the user to confirm if there isn't enough storage space to move products to other Warehouses.

### Fixed
- Fixed an issue that prevented the planet description from displaying.
- Fixed an oversight that caused ListBox to sort all of its items regardless of whether it was in sorted mode or not.
- Addressed a usability issue with the underlying NAS2D library that sometimes resulted in incorrect sizing and positioning of UI elements when resizing the game window.
- Fixed 'Quit' button in Planet Selection view being positioned incorrectly on window resize.
- Fixed a mistake in robot aging logic that resulted in a dangling pointer dereference. Thanks to OPU user jem7650 for bringing this to my attention!
- Fixed an animation state issue that would force the AirShaft sprites that are underground to use the surface animation.
- Fixed a crash issue that occurred when an exhausted mine facility is bulldozed.


## [0.7.6] - 2018-07-06

Numerous bug fixes, rounding out some features introduced in v0.7.5 and introduces the new Mine Operations Window.

### Added
- Added a Main Menu state which allows for starting a new game, continuing an old game or quitting (options menu slated for a future release).
- Added the Solar Panel Array structure. This is a cheap power structure most useful in the early game that generates a small amount of power.
- Added Mine Operations Window to allow for halting and resuming production of mines and specific ore threads within it.

### Changed
- Savegame protocol updated to 0.30 (from 0.25) due to breaking changes. Savegame's from older versions will no longer load.
- Tweaked fade transitions between states.
- Minor performance improvements regarding item sorting in the IconGrid UI panels.
- TileMap selector has been changed from an image outline highlight to highlighting the entire tile.
- Tweaked energy requirements for most structures to make energy requirements more realistic and something to genuinely contend with in gameplay.
- Errors when loading savegames will now display an OS dialog box letting the user know about the issue.
- Robots beyond the first three robots are now stored within a Robot Command Center.
- Factories producing robots will go Idle if no Robot Command Capacity is available.
- Mine shafts can now be extended using the new Mine Operations Window.
- Mine ore processing can be controlled much more finely using the new Mine Operations Window.
- Mines no longer operate indefinitely. They now have a finite amount of resources based on yield.
- Robominer's are now single use robots. Once they finish digging the initial mine shaft they use themselves as the base to build the mine facility.
- Exhausted mines will now be indicated on the minimap using a light grayish-blue color.
- Placing a robodigger on a mine will now destroy the mine.
- Robodozers can now be placed on exhausted mines that have reached their maximum digging depth.
- Resource counts will throb red in the Resource HUD when resources are critically low.

### Fixed
- Fixed an issue in the Factory Production window which would fail to set the current production type when selecting a Factory leading to production queue being cleared when hitting the Okay button.
- Fixed an oversight where warehouses didn't save/load any products that were stored in them.
- Fixed an issue in loading code that would lead to a crash if loading a savegame where the SEED lander hasn't been placed yet after being in a game with a command center somewhere on the map.
- Fixed an oversight that didn't reset all of the UI control states when switching to a new map when already in a game.
- Fixed an issue regarding loading games in which structure states were not properly set.
- Fixed an issue when first starting a colony that would fail to set population requirements for initial structures.
- Fixed a mistake that allowed the user to bulldoze the landing site for a Cargo Lander.
- Fixed an issue that caused Button's to not show their 'pressed' stated.
- Fixed an issue that would cause a crash if the Structure Inspector Window was open and the structure it was detailing is bulldozed.
- Fixed an issue that would cause a crash if the Factory Production Window was open if the factory being modified is bulldozed.
- Fixed a mistake in the WarehouseInspector which would display incorrect storage values.
- Fixed a hidden mistake which failed to call derived activation logic when a structure became active.
- Fixed an issue that caused incorrect reporting of storage use in ProductPool (affected warehouses).
- Fixed an issue that would leave UI windows open when interacting with the system interfaces (main menus, fileIO, etc.).


## [0.7.5] - 2018-05-11

This version of OutpostHD has moved to Git.

### Added
- Robot Command Center is now fully functional.
- Added a Nursery structure. Required to keep infant mortality rate at a reasonable level.
- Added the Cargo Lander.
- New visuals for some of the surface structures.
- F11 keybinding to switch between fullscreen and windowed mode.
- Game window is resizable (minimum size 800x600).
- New higher resolution tile sets.
- Added a drop shadow to mine beacons on the minimap to help them stand out better.
- Added a new method to IconGrid to set the current selection using an IconGridItem's 'meta' value.
- Added Loading/Saving plaque's during file operations for save games.
- Added 'processing turn' plaque whenever a turn is processed.
- Factory production now stores products in Warehouses.
- Commercial structures now draw luxury products from warehouses each turn. +1 morale for each commercial structure that has access to luxury products.
- Added Warehouse Inspector window. Primitive but does the job.

### Changed
- Robots can now only be placed within the area of influence of the Command Center and Comm. Towers.
- FileIO window includes a new ListBox that shows a scroll bar when the list of files is too long to display.
- FileIO window now responds to double clicks.
- Structures can now only be placed within 15 tiles of the command center.
- Robots can now only be placed within 15 tiles of the command center.
- The population model has been completely rewritten. It now accounts for morale, residences, universities and hospitals.
- Factory Production window now allows the user to set a factory to Idle. This is not undone until the user explicitely states it.
- Factory Production window now has a "Clear Selection" button. This provides a much more obvious way to clear the selection vs. clicking outside a product icon in the product grid.
- Factory Production window now has an "Apply" button which works the same as the "Okay" button but doesn't hide the window after clicked.
- Updated keybindings -- see README.md for details.
- Population panel now shows residential capacity.
- Morale is now affected by overcrowding in residences.
- Internal modifications to ProductType valuation -- this is a breaking change for older save games.
- Updated resource icons.
- MiniMap internal drawing code has been simplified using NAS2D's new clip rectangle functions. This should improve performance for games with a lot of things going on in the minimap.
- FileIO window now responds to Enter/Escape keys.
- Factories will go idle if there is insufficient storage for products they are producing.

### Fixed
- FileOP button (Save/Load) in FileIO window will now start off disabled when first displayed.
- Fixed a mistake in UndergroundFactory that would cause it to be saved as a SurfaceFactory.
- Fixed an issue with tile picking that caused inaccuracies when map edge lengths were even.
- Fixed an issue which left unavailable robot selections available when loading a saved game from a game that had said robots available.
- Fixed a mistake that would fail to bring the Structure Inspector window to the front of the window stack.
- Fixed an issue in the population food consumption/starvation code that could result in an infinite loop causing the game to hang.


## [0.7.1] - 2017-07-30

This version includes all revisions from r191 to r276.

### Added
- Added a Game Over window.
- Added Colonist Lander.
- Added an Announcement window for major events in the colony. Not polished but does the job.
- Added a System menu to visually access the Save/Load interface and return to the menu.
- Added a Save/Load interface.

### Changed
- Agridomes will now dump their entire storage of food when they go disabled. This effectively simulates a failure of the food storage units which leads to spoilage.
- Game will go into a Game Over state when population reaches 0.
- Population will now only become active when dropped to the surface with a lander. 50 colonists per lander.
- The game will alert the player that the colony ship has deorbited and crashed after 24 turns (basically two years).
- Player must now drop their colonists before the ship deorbits or they will lose their colonists.
- File dialog text field will now only allow 50 characters total.
- Buildings will go disabled if insufficient population is available to them.
- Structure Panel will now disallow selecting structures if there are insufficient resources. These structures will be highlighted in red.

### Fixed
- Fixed an issue that would keep one colonist alive regardless of how much food was available.
- Fixed a mistake that would cause the morale indicator icon to disappear when the player reaches the maximum morale level.
- Fixed a crash when switching between game states -- was an oversight in the UI handling code.



## [0.7.0] - 2016-09-12

This version includes all revisions from r136 to r190.

### Added
- Site Map now shows Command Center's influence radius.
- Added the following structures:
	- Hot Laboratory
	- Park / Reservoir
	- Recreation Facility
	- Red Light District
	- Robot Command
	- Solar Power Receiver Array
	- Surface Police
	- University
	- UG Commercial
	- UG Factory
	- UG Laboratory
	- UG Police
	- Warehouse
- Added Robot Counts to the display.
- Added an almost perfect haunting track to the planet select state. Really sets the mood.
- Added a background and some visual flare to the planet select state. Again, really sets the mood.
- Added a PopulationPanel to the UI in the HUD.
- Added UI skinning. Pretty basic skin but adds a an air of refinement to the overall look and feel.
- Added a method to switch between levels and move around the map using the mouse.
- Added a check when placing robots, structures or tubes so that these objects aren't placed unless the tile being pointed at is actually visible.

### Changed
- Windows (tile inspector, structure inspector, digger direction and factory production) are now drag-able.
- Windows will now arrange themselves according to their position in a 'stack'.
- Updated Digger placement logic to account for additional use cases that caused weird behavior.
- Updated Miner bot placement logic so that insertion mode isn't cleared if more miner's are available.
- Save/Load now handles Morale.
- Save/Load now handles Population.
- Reworked UI
	- Structures/Robots/Tubes menus are now displayed all at once.
	- The buttons made to toggle between the menus are removed.
	- The count of robots available/total for each type of robot has been added on screen above the structure menu in the lower left corner.
	- The other buttons were moved/rearranged to avoid space loss.
- Changed the Splash Screen jingle. Little less creepy, little more appropriate, still hardly perfect.
- Command Center now provides bare bones food storage.
- Digger direction dialog is repositioned when placing a digger bot on an underground level. This prevents the user from accidentally selection a direction they may not have intended.

### Fixed
- Fixed a mistake in GameState that would forget to set a pointer to the players resource pool when the surface factory was placed.
- Fixed a mistake that causes a game crash when turning on the Debug information when the view is at the edges of the map.
- Fixed an issue in the Agricultural Dome that would cause food production and storage to continue past storage capacity.
- Fixed an issue in the savegame load code that neglected to update the current storage availability until a turn was completed.
- Fixed a mistake which caused the underground build menu to show surface structures if you attempted to go to a depth level below the max planetary depth.
- Fixed an oversight that caused the structure build menu to populate with structures when underground before placing a SEED Lander.



## [0.6.6] - 2016-07-27

This version includes all revisions in r135.

### Added
- Added a Smelter structure.
- Added a Surface Factory structure.

### Changed
- Modified all structure graphics to pad them to power of two sizes.
- Updated code license to a BSD 3-Clause license.

### Fixed
- Fixed a mistake in the game save code that would skip saving tile states on the lowest possible underground level.
- Fixed a mistake in load logic that would force airshafts on subsurface levels to the surface animation state.



## [0.6.5] - 2016-07-19

This version includes all revisions from r127 to r134.

### Added
- Added a Fusion Reactor structure.
- Added a Splash Screen and a user configurable option to skip the splash screen after the first time the game is run.
- Added a TextArea UI control. It's crude but does the job.
- Added a Quit button to Planet Select State.

### Changed
- Planet Select state now shows basic flavor text for each planet.
- Choosing a planet now affects mine count and max digging depth in addition to tileset and landing site changes.
- The game now automatically sets its resolution to the native desktop resolution and full screen. This can be changed after the first run in the configuration file.

### Fixed
- Fixed a mistake in StructureManager which forgot to drop all of the structure lists whenever StructureManager::dropAllStructures() was called.
- Fixed a mistake in Structure's forced_state_change() that would leave a structure that had just reached its build age in a construction animation state.
- Fixed a mistake in Control that would not call onSizeChanged() when any of Control's size functions were called.



## [0.6.0] - 2016-07-14

This version includes all revisions from r90 to r126.

### Added
- Current state of the game can now be saved.
- Current state of the game can now be loaded.
- Added mouse hover coordinates to debug.
- Added Structure information to debug.
- Added a current level indicator.
- Added voice indicators for tile placement.
- Added AI Voice Gender Selection.
- Added a Structure Inspector UI. Not a whole lot of information in it yet but the basics are there.
- Added some UI Blips and beeps to the Planet Select screen for basic feedback.

### Changed
- Pulled old UI data files.
- Updated code for Mine interface and implementation.
- OutpostHD will now create a 'savegame' directory under 'data' if one doesn't exist.
- Minimap now displays location of active robots.
- User can now change the tube selection when placing tubes using the wheel mouse (allows for quick tube placement).
- Using the mouse's scroll wheel when in tube insertion mode will now switch which tube piece is being placed.
- Improved mine beacon glow effect.
- Mine robots now build a mine shaft underneath the surface facility.
- Fonts have been replaced with easier to read and more predictable bitmap based fonts.
- Tile Inspector is much more fleshed out.

### Fixed
- Fixed a mistake in the way robots were handled in the UI when deployed. Robot selection UI will now properly update itself when any robot in the field completes its task instead of the original robots created when the SEED Lander is deployed.
- Fixed a dangling pointer bug when deserializing robots.
- Fixed a dangling pointer bug when deserializing structures.
- Fixed a mistake that would cause the FactoryProduction UI to not be properly updated when viewing a factory.
- Fixed a mistake that would cause some mouse clicks to be ignored until the TileInspector window has been pulled up at least once.



## [0.5.2] - 2016-06-27

This version includes all revisions from r46 to r89.

### Added
- Added Storage Tanks structure.
- Added a Residence Structure.
- Added ResourcePool interfaces to Structure for resource management of structures that offer resource storage and production.
- Added a sanity check to GraphWalker class to avoid or note potential null pointer accesses.
- Added current resource storage vs. total storage available to UI status bar.
- New colonies now come with a base storage capacity of 250. This can be expanded with storage tanks.
- Automatic trucking of resources implemented.
- StructureManager now maintains lists of typed structures.

### Changed
- Massive internal code cleanup and refactoring too numerous to list here -- see commit history for details.
- Moved sprite coloration calls from StructureManager to Structure.
- Decoupled update and think logic in Structure such that the StructureManager handles how and when structures are updated.
- IconGrid now sorts items it contains.
- Pulled old map data files that are no longer used.
- Pulled TilePositionInfo structure from the code.
- Moved tile position information into the Tile class itself. Perhaps not the most elegant design but it offers a much easier to understand interface, a lot less potential for bugs and makes it a lot easier to make determinations in the structure graph.
- incrementAge() function made a private, internal only member of Structure -- prevents future logic mistakes.
- Updated smelter's resource processing to avoid race condition where only common metals would be updated.
- Tweaked resource processing of the smelter to make for more balanced resource income. Will likely need future adjustments
- Command Centers are now flagged as self-sustained.
- Modified StructureManager such that we can use dependency injection to provide the player's resource pool to structures without having to make it a global variable.
- Updated StructureManager interfaces for addStructure() and removeStructure() -- these functions no longer return a boolean value (never used, overcomplicates the logic, etc.)
- Renamed elements in StructureID to prevent future naming conflicts with StructureType enumeration.
- Updated all code to reflect the change in StructureID naming.
- Updated Structure to take advantage of the StructureType enumerator instead of storing and setting boolean flags for things like isFactory, providesCHAP, etc.
- Debug information can be turned on and off with the F10 key (defaults to off).
- Completely rebuilt how StructureManager identifies and manages structures between different lists.
- Updated energy UI status bar to show available energy vs. total energy capacity.
- Structures now cost resources to build.

### Fixed
- Fixed a mistake in GameState in which clicks within the digger direction dialog were ignored when the dialog was hidden.
- Fixed a mistake in the logic of several Structures that would make inappropriate calls to incrementAge().
- Fixed a logic mistake in MineFacility that would set the building to Idle as soon as it became active.
- Fixed a mistake in ResourcePool where reporting and setting food and energy values were swapped.
- Fixed another mistake in ResourcePool's operator-=() and operator+=() that was swapping food and energy resources.
- Fixed a crash bug waiting to happen in StructureTranslator.



## [0.5.1] - 2015-12-28

This version includes all revisions from r39 to r45.

### Changed
- Placing tubes will now check for connectedness.
- Redefined energy requirements for several structures including the Agridome, Command Center and CHAP.
- Mini Map now draws the highlighter box above mines and with a shadow to help it stand out from light colored site maps.
- Numerous internal code changes and cleanup making it much more understandable and maintainable.

### Fixed
- Fixed a mistake where tube insertion mode would only be updated when the Tube Intersection was selected when displaying underground tubes.
- Fixed a mistake when the SEED Lander is deployed where the Build Structures button wasn't disabled.



## [0.5.0] - 2015-11-25

This version includes all revisions from r21 to r38.

### Added
- Added a FactoryProduction dialog.
- Added a StructurePriority field to Structure for sorting purposes.
- Added StructureManager::printSortedList() for a quick and easy way to see 1) what's being managed by the StructureManager and 2) to verify that the list is sorted correctly.
- Structure now has an 'isFactory' field.
- Added a changeDepth() function to GameState which is called whenever number keys are pressed (code readability).
- Added convenience functions to Control.
- Added ID field to Structure. Currently generates an ID based on the address of the Structure object in memory.
- Added additional UI Constants for code cleanup.
- Added additional UI icons.

### Changed
- Changed the way StructureManager stores its managed Structures so that Structures can be sorted and updated by priority.
- Updated several structures to adjust from the default priority level.
- Fleshed out the Factory interface class a bit more. It's now generally functional.
- SeedFactory and SeedSmelter now derive from Factory instead of Structure.
- SeedSmelter is now processing raw resources.
- Tinkered with Agridome and CHAP with their resource consumption. Will require better balancing to be playable.
- StructureManager now implements a factory update loop.
- Changed Resources structure to store resources as double's instead of int's. Helps to correct for rounding issues and makes more sense in terms of a simulation.
- Played with the mine yields a bit... still not where I want them to be but better.
- Tweaked vales in SeedSmelter. No provides better balanced output.
- Buildings will draw as translucent (72.5% alpha) when 'Idle'.
- Updated UI Icons to contain resources.
- Updated Control with a convenience function to set size with a single parameter.
- Modified UI fields to condense the UI a bit.
- Many other changes too numerous to list. See commit notes for additional information.

### Fixed
- Removed some unnecessary data structures in Menu control (defined its own rect when Control already has one).
- Pulled unused/legacy typedef from GameState.
- SeedPower now shows itself as destroyed after it reaches its max age.
- SeedLander will now add resources to the resource pool when bulldozed.
- Fixed a mistake that would change the depth to level 4 no matter what button was pressed.



## [0.4.2] - 2015-10-13

This version includes all revisions from r14 to r20.

### Added
- Added Tile::structure() to get a pointer to a derived Structure instead of a base. Eliminates some ugly casts.
- Added Tile::robot() to get a pointer to a derived Robot instead of a base. Eliminates some ugly casts.
- Added Tile::empty() purely for readability in some parts of the code.
- Added a naive approach to redirecting console output to a log file. Not crazy about it but it does the job.
- Added basic resource input and output checking.
- Began Factory interface.
- Added CHAP Requirement check.
- Added screen fades for state transitions.
- Added PlanetSelectState -- users can now choose between three different planet types.

### Changed
- Moved some of the function definitions from Robot.h to Robot.cpp.
- Modified RobotPool to get specific types of robots instead of a pointer to a base. Eliminates some ugly casts.
- Changed the way a few handlers checked for robot availability. Was kind of sloppy testing for a nullptr when when RobotPool::robotAvailable() function should have been used instead.
- Modified all code that calls Tile::index(int) to take advantage of the TerrainType enumerator. Easer to understand code.
- Removed command center pointer in StructureManger. Don't need it anymore.
- Broke StructureManager's update code into several smaller functions.
- Updated CHAP Facility animation.
- Removed some unused code from GameState that updated all 'things'.
- Updated Release mode -- no longer shows the console.
- Moved around some of the code in SeedLander to match current structure code formatting guidelines.
- Game now starts player off with some additional resources.
- Moved the GameState.h, GameState.cpp and GameStateUi.cpp files to a States directory.
- Moved Tube.h to ../Things/Structures/.
- TileMap's c'tor now takes additional parameters to load different tilesets.


### Fixed
- Fixed an issue where the tile position information when inserting a Digger robot could change when the user clicked to select a direction on non-surface levels.
- SeedFactory now calls its Activate function after it finishes building.
- Updated NAS2D Libraries (fixing an issue that caused event handler issues).



## [0.4.1] - 2015-10-09

This version includes all from r14.

### Added
- Added a RequiresCHAP field to structures. Updated structures currently defined that don't require a CHAP to operate.
- Added some comments to Structure::enabled(bool) and Structure::idle(bool) to clarify what and why they are separate and the difference between IDLE and DISABLED states.
- Added a utility operator () to TilePositionInfo that allows quick setting of values.
- Added onActivate() to GameState so that when the window loses focus any flags set by event handlers (like mouseDown) can be cleared if the window loses focus before a corresponding event fires (like mouseUp).
- Added a new map for 1) variety and 2) as proof of concept so we can start dumping OP1's graphics.
- Added removeStructure() to StructureManager.
- Added building bulldozing. Bulldozed buildings will add their resource recovery values to the resource pool. Command Center's can never be bulldozed.
- Added a README to the repository for distribution with releases.
- Added a CHANGELOG to the repository for distribution with releases.

### Changed
- Mine facilities now take several turns to complete construction and go into an operational state. This is something that can probably be adjusted based on difficulty level.
- Optimized TubesPalette to show which tube type is currently being placed. A right click will clear the mode and reset the toggle states on the TubesPalette buttons.
- Turn button is now disabled until the SEED Lander is placed.
- Removed check from checkConnectedness() that is no longer necessary.
- Removed DiggerTile structure -- less useful clone of TilePositionInfo and ultimately there was a better way to handle it.
- Diggers will now only dig downward if placed on an existing AirShaft.
- When a building is bulldozed or a digger bot is placed on an airshaft, connectedness checks are run to immediately update connectedness availability.
- StructureManager now has a function to quickly set all tiles that structures are on into a disconnected state.
- Various changes in DiggerDirection UI class to implement correct digger behavior and simplify the storage of tile position information in the GameState class.
- Moved some stuff around in GameState.h to group like things together and get things in a more sensible state.
- MiniMap now shows mines using a diamond shaped graphic to more closely resemble OP1's visuals.
- TileMap now properly centers itself in the vertical viewspace regardless of resolution
- UI Panel is now 185 pixels tall instead of 180.

### Fixed
- When a digger is placed its mode will now properly be cleared.
- Fixed a logic mistake in the GraphWalker that prevented the walker from properly seeing the deepest level of structures.
- Logic error in Robot corrected -- turnsToCompleteTask() assumed that the input value would always be greater than 0 which would lead to the taskComplete callback never getting fired.
- Found an unused variable in the StructureManager -- cleared it out.
