# Data

Pretty much any object in the game has its attributes defined in this folder's files. Naturally, there is a file for each type of object (Items, Particles, Blocks, and Entities). Each file name is self-explanatory, with the exception of the entities; entities are split into three categories: Projectiles, ItemEntities, and NPCs. Those are explained in a little more detail later.

**Please note that this is all in XML format. Each object has its own tag. Creating an object without the proper syntax/tags will raise an error!**

## Items

Each item is defined starting with the tag `<item id="`*`Your number here`*`" name="`*`Your value here`*`">`. The ID attribute is how the game/scripts will actually internally refer to the item, and the name is the name that will show up when the item's name is required.

The following attributes can be set:

- `<texture>`

 This defines the path that the texture will be found in. Working directory is `Assets/Textures/Items/`

- `<weight>`

 This defines the item's weight in an inventory.

- `<useScript>`

 This defines the path of the script that will be run when this item is used/consumed. Working directory is `Assets/Scripts/`


## Particles

Each particle is defined starting with tag `<particle id="`*`Your number here`*`">`. The ID attribute is similar to the item's ID attribute, as this is how scripts and the game will refer to the particle type.

The following attributes can be set:

- `<texture>`

 This defines the path that the texture is found at. Working directory is `Assets/Textures/Particles/`

- `<bumpMap>`

 This defines the path that the normal map's texture is found at. Working directory is `Assets/Textures/BumpMaps/`

- `<script>`

 This defines the path of the script that should be run each update. Working directory is `Assets/Scripts/`


## Blocks

Each block (or tile) is defined starting with the tag `<tile id="`*`Your number here`*`" name="`*`Your name here`*`>`. The ID attribute is similar to the item's and the particle's, and the name is purely for readability (as of the time of writing this).

The following attributes can be set:

- `<texture>`

 This defines the path that the texture is found at. Working directory is `Assets/Textures/Blocks/`

- `<bumpMap>`

 This defines the path that the normal map's texture is found at. Working directory is `Assets/Textures/BumpMaps/`

- `<emittedLight>` & `<emittedHeat>`

 These define the amount of light/heat this block actively provides to its surroundings.

- `<sizeX>` & `<sizeY>`

 These define the size of a block, where 1.0 is equal to the width/height of a regular block.

- `<isSolid>`

 This defines if entities can travel through this block

- `<isDrawn>`

 This defines if the block is drawn/rendered.

- `<isNatural>`

 This defines if the block is natural. If this is true, it is unaffected during era changes.

- `<isTransparent>`

 This defines if sunlight can pierce through this block.

- `<updateScript>`, `<tickScript>`, `<destructionScript>`, `<interactScript_XXXXX>`

 These define the paths of each type of script this tile can trigger. Working directory is `Assets/Scripts/`. Current available interactions are: `walkedOn` (when any entity is moving across the block's top) and `used` (When the player right clicks the block).


## Entities

Entities are split into three categories:

- Projectiles,

 These are projectiles, whether that's bullets, magic balls, or flying corpses. They simply move, update, and (optionally) collide with entities/blocks.

- ItemEntities, and

 These are the entities which represent items in the world. These are super simple; they just update, draw, and collide.

- NPCs

 These are "classic" entities. These are the monsters, quest-givers, helpless citizens, etc. These are quite a bit more complicated.

Although they're split into categories, all entities have base attributes:

- `<texture>`

 This defines the path that the texture is found at. Working directory is `Assets/Textures/Entities/`*`[Items or NPCs or Projectiles]`*`/`

- `<bumpMap>`

 This defines the path that the normal map's texture is found at. Working directory is `Assets/Textures/BumpMaps/`

- `<sizeX>` & `<sizeY>`

 These define the size of the entity, where 1.0 is equal to the width/height of a regular block.

- `<updateScript>`, `<tickScript>`

 These define the paths of each script this entity triggers. Working directory is `Assets/Scripts/`

#### Projectiles

 Projectiles are defined with the tag `<projectile id="`*`Your number here`*`">`. As with the rest of the objects, the ID is how scripts and the game refer to this type of **Entity**. **Note that the IDs are shared between all types of entities. This means you can't have any identical IDs between NPCs, ItemEntities, or Projectiles.**

 They have the following attributes:

 - `<collides>`

  This defines if the projectile collides with blocks. If true, it collides.

 - `<speed>`

  This defines how fast the projectile moves along its path.

 - `<damage>`

  This defines how much damage the projectile deals when colliding with an entity.


#### ItemEntities

 ItemEntities are defined with the tag `<itemEntity id="`*`Your number here`*`" name="`*`Your value here`*`>`. The name is simply for readability. As with the rest of the objects, the ID is how scripts and the game refer to this type of **Entity**. **Note that the IDs are shared between all types of entities. This means you can't have any identical IDs between NPCs, ItemEntities, or Projectiles.**

 They have the following attributes:

 - `<item>`

  This defines the ID of the item that this yields when picked up/collected.


#### NPCs

 NPCs are defined with the tag `<npc id="`*`Your number here`*`" name="`*`Your value here`*`>`. The name is simply for readability. As with the rest of the objects, the ID is how scripts and the game refer to this type of **Entity**. **Note that the IDs are shared between all types of entities. This means you can't have any identical IDs between NPCs, ItemEntities, or Projectiles.**

 They have the following attributes:

 - `<isDamagedByFalls>`

  This defines if the entity takes damage from falling.

 - `<isInvincible>`

  This defines if the entity is immortal.

 - `<speed>`

  This defines the running/flying speed of the entity.

 - `<jumpHeight>`

  This defines the entity's jump height (in blocks).

 - `<maxHealth>`

  This defines the initial and maximum amounts of health an entity can have.

 - `<faction>`

  This defines the faction of this entity. **Values can only be: evil, bad, neutral, good, or benign**

 - `<attackScript>`

  This defines the script triggered when this entity attacks. This script defines the attack patterns for entities. The working directory is `Assets/Scripts/`

