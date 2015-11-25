# Change Log
This is the changelog for OutpostHD.

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
