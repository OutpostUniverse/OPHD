# Change Log
This is the changelog for OutpostHD.

## [0.4.1] - 2015-10-09
### Added
* Added a RequiresCHAP field to structures. Updated structures currently defined that don't require a CHAP to operate.
* Added some comments to Structure::enabled(bool) and Structure::idle(bool) to clarify what and why they are separate and the difference between IDLE and DISABLED states.
* Added a utility operator () to TilePositionInfo that allows quick setting of values.
* Added onActivate() to GameState so that when the window loses focus any flags set by event handlers (like mouseDown) can be cleared if the window loses focus before a corresponding event fires (like mouseUp).
* Added a new map for 1) variety and 2) as proof of concept so we can start dumping OP1's graphics.
* Added removeStructure() to StructureManager.
* Added building bulldozing. Bulldozed buildings will add their resource recovery values to the resource pool. Command Center's can never be bulldozed.
* Added a README to the repository for distribution with releases.
* Added a CHANGELOG to the repository for distribution with releases.

### Changed
* Mine facilities now take several turns to complete construction and go into an operational state. This is something that can probably be adjusted based on difficulty level.
* Optimized TubesPalette to show which tube type is currently being placed. A right click will clear the mode and reset the toggle states on the TubesPalette buttons.
* Turn button is now disabled until the SEED Lander is placed.
* Removed check from checkConnectedness() that is no longer necessary.
* Removed DiggerTile structure -- less useful clone of TilePositionInfo and ultimately there was a better way to handle it.
* Diggers will now only dig downward if placed on an existing AirShaft.
* When a building is bulldozed or a digger bot is placed on an airshaft, connectedness checks are run to immediately update connectedness availability.
* StructureManager now has a function to quickly set all tiles that structures are on into a disconnected state.
* Various changes in DiggerDirection UI class to implement correct digger behavior and simplify the storage of tile position information in the GameState class.
* Moved some stuff around in GameState.h to group like things together and get things in a more sensible state.
* MiniMap now shows mines using a diamond shaped graphic to more closely resemble OP1's visuals.
* TileMap now properly centers itself in the vertical viewspace regardless of resolution
* UI Panel is now 185 pixels tall instead of 180.

### Fixed
* When a digger is placed its mode will now properly be cleared.
* Fixed a logic mistake in the GraphWalker that prevented the walker from properly seeing the deepest level of structures.
* Logic error in Robot corrected -- turnsToCompleteTask() assumed that the input value would always be greater than 0 which would lead to the taskComplete callback never getting fired.
* Found an unused variable in the StructureManager -- cleared it out.
