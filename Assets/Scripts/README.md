# DaviScript is no more!

The scripting system in Fear Of The Dark is now Lua! This means that you can write any valid Lua expression, be it math or, uh, something else!

Alongside all the regular Lua functions, Fear Of The Dark's system also includes some *custom functions*.

The following is a list of custom functions with arguments, types, and other info:

## Variables

There are a few variables at your disposal in certain types of scripts.

#### Update, Tick, Destruction, and Interaction Scripts (Blocks)

These scripts are called on certain events (like every frame for update scripts, every tick for tick scripts, etc.). They automatically contain and set the variables:

- `selfX`

 The X position of this block.

- `selfY`

 The Y position of this block.

- `selfID`

 The ID of this block.

## Utility Functions

These functions are used not to set/get information, but they still affect the gameplay experience.

- `playSound(soundID, volume)`

 Plays a sound with the specified ID at the specified volume

- `createParticle(ID, x, y, xVelocity, yVelocity, width)`

 Creates a new particle with the specified `ID` (specifies texture, bumpmap, etc.) at the position `(x, y)` with the velocity vector `(xVelocity, yVelocity)`, and size of `width`.

## Setters

These functions set or change values in-game.

#### Block Setters

- `setBlock(ID, x, y, layer, metadata)`

 At the position `(x, y, layer)`, a block with the ID of `ID` (duh) will be made and given the `metadata`. The `metadata` should be in the form of a string.

- `removeBlock(x, y, layer)`

 The block at the position `(x, y, layer)` will be replaced with an air block. This is a synonym for `setBlock(0, x, y, layer, "")`.

- `showBlock(x, y, layer)`

 Makes a block at position `(x, y, layer)` not-invisible.

- `hideBlock(x, y, layer)`

 Makes a block at position `(x, y, layer)` invisible.

#### Entity Setters

- `addEntity(ID, x, y, layer)`

 Creates an entity with ID of `ID` and places it at position `(x, y, layer)`.

- `removeEntity(UUID)`

 Removes an entity with the specified `UUID` from the game, leaving no trace of it. **Note: UUIDs should only be obtained from Getter functions and not modified.**

- `showEntity(UUID)`

 Makes an entity with the specified `UUID` not-invisible. **Note: UUIDs should only be obtained from Getter functions and not modified.**

- `hideEntity(UUID)`

 Makes an entity with the specified `UUID` invisible. **Note: UUIDs should only be obtained from Getter functions and not modified.**

- `setTime(time)`

 Sets the world's time to the specified `time` value.

- `teleport(UUID, x, y)`

 Teleports an entity with the specified `UUID` to the position `(x, y)`. **Note: UUIDs should only be obtained from Getter functions and not modified.**

- `giveItem(UUID, ID, quantity)`

 Gives an entity with the specified `UUID` the specified `quantity` of items with the specified `ID`. **Note: UUIDs should only be obtained from Getter functions and not modified.**

#### Player Setters

- `setPlayerCanInteract(value)`

 Sets if the player can interact with the environment and NPCs (a `true` `value` means they can, `false` means they can't).

- `setPlayerGodMode(value)`

 Gives the player *god priveleges*. Used for debugging and screwing around.

- `setPlayerStat_sanity(sanity)`

 Sets the player's sanity to a given `sanity` (from 0.0-1.0)

#### Camera Setters

- `camera_setLocked(locked)`

 Either sets the camera to not move (`true`) or unlocks it to follow the player (`false`).

- `camera_setPosition(x, y)`

 Centers the camera on the given position.

- `camera_move(xDistance, yDistance)`

 Moves the camera a given distance in each axis away from its current position.

- `camera_smoothMove(x, y, speed)`

 Moves the camera a given distance in each axis away from its current position, but smoothly at a given `speed`.

- `pause()`

 Stops the continual updating and ticking of the game. Useful for cutscenes, as it still allows the camera to move and scripts to run.

- `play()`

 Lets the game play normally (Unpauses).

#### Other Setters

- `setFlag(ID, value)`

 Sets the flag with the specified `ID` to the specified `value` (`true` or `false`).

- `setEra(eraName)`

 Changes the era to the one specified. See below for possible values.

- `startTrade(ID)`

 Opens the trade dialogue, using the given tradetable `ID`.

- `startDialogue(ID)`

 Starts a conversation, using the given question `ID`.


## Getters

These functions are used to get values from the game into your Lua script to be used in other functions or calculations.

- `getEntitiesNear(x, y, distance)`

 Returns an array of all entities' UUIDs who fall within a circle around the position `(x, y)` with radius `distance`.

- `getEntitiesArea(x1, y1, x2, y2)`

 Returns an array of all entities' UUIDs who fall within a rectangle made between position `(x1, y1)` and `(x2, y2)`.

- `getPlayer()`

 Returns the player's UUID.

- `getSelectedEntity()`

 Returns the UUID of the entity that the player has currently selected.

- `getEntityPosition(UUID)`

 Returns an array formed with the x (1st element) and the y (2nd element) of the entity with the given `UUID`.

- `getFrame()`
 
 Returns the current frame the game is at. Useful for timing without `delay()` and making sure things only happen on certain frame intervals.
 Ex: to get a sound to play every 4 frames, you could use `if getFrame() % 4 == 0 then playSound(sound) end` (provided this script is run every frame)

## Miscellaneous

These functions are useful, but very specific.

- `delay(frames)`

 Delays the script for the specified number of `frames` (60 frames are in one second).


## Debugging

These functions often don't actually affect anything in-game, but they can be useful for debugging your scripts.

- `log(message)`

 Logs the given `message` in the GameLog file and in a running console (if one is available).


## Interactions

Objects can be interacted with, and each type of interaction calls a given script (specified in the Block/Entity/Item data XML file). The following is a list of interactions:

- `WalkedOn`

 This interaction is only for blocks, and triggers when any entity walks on any block.

- `Used`

 This interaction is for blocks and items, and triggers when a) a block is right clicked, or b) when an item is used.
