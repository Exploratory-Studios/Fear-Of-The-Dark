# Data

This folder contains pretty much all the functional data for blocks, items, entities, and particles. Each respective file has the following attributes, in XML format:

## Blocks (Tiles)

Blocks (otherwise known as tiles) are stored in the Blocks.xml file. Their attributes are as follows:

- `texture`
 
 This attribute is a `string`, and tells the game what the block's texture should be. (The directory is Assets/Textures/Blocks/**Your value here**)

- `bumpMap`

 This attribute is a `string`, and tells the game what the block's bump map texture should be. (The directory is Assets/Textures/BumpMaps/**Your value here**)

- `emittedLight`

 This attribute is a `float`, defaults to `0.0`, and tells the game what level of light this block emits.

- `emittedHeat`

 This attribute is a `float`, defaults to `0.0`, and tells the game how much heat this block emits.

- `sizeX`, `sizeY`

 These attributes are `float`s, they default to `1.0` and `1.0`, and they tell the game how big the block is, in terms of block units.

- `isSolid`

 This attribute is a `bool`, defaults to `1`, and it tells the game if entities can travel through this block.

- `isDrawn`

 This attribute is a `bool`, defaults to `1`, and tells the game if this block should be rendered

- `isNatural`

 This attribute is a `bool`, defaults to `0`, and tells the game if this block is natural (should be turned to stone when eras pass).

- `isTransparent`

 This attribute is a `bool`, defaults to `0`, and tells the game if sunlight can travel through this block.

- `updateScript`

 This attribute is a `string`, defaults to `NO_SCRIPT`, and tells the game what script should be run every time this block updates. (The directory is Assets/Scripts/**Your value here**)

- `tickScript`

 This attribute is a `string`, defaults to `NO_SCRIPT`, and tells the game what script should be run every time this block ticks. (The directory is Assets/Scripts/**Your value here**)

- `destructionScript`

 This attribute is a `string`, defaults to `NO_SCRIPT`, and tells the game what script should be run when this block is destroyed. (The directory is Assets/Scripts/**Your value here**)

#### Interaction Scripts

These scripts are prefaced with `interactScript_`.

- `...walkedOn`

 This attribute is a `string`, defaults to `NO_SCRIPT`, and tells the game what script should be run every time this block is stepped on. (The directory is Assets/Scripts/**Your value here**)

- `...used`

 This attribute is a `string`, defaults to `NO_SCRIPT`, and tells the game what script should be run every time this block is right-clicked. (The directory is Assets/Scripts/**Your value here**)


As blocks are defined in XML, they must be formatted as follows:

```
<tile id="**Put An ID Integer Here**" name="**A Name For Easy Identification**">
	<texture>**Your texture here**</texture>
	<isSolid>**1 or 0 here**</isSolid>
	...
</tile>

```
