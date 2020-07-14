#include "Entity.h"

#include "Categories.h"
#include "World.h"

#include "EntityNPC.h"
#include "EntityItem.h"
#include "EntityProjectile.h"

#include "Tile.h"

#include "XMLData.h"
#include "XMLDataTypes.h"

#include <math.h>

Entity::Entity(glm::vec2 pos, unsigned int layer, SaveDataTypes::MetaData data) : m_position(pos), m_layer(layer), m_metaData(data) {

} // This is more of an abstract class

Entity::Entity(SaveDataTypes::EntityData& saveData) {
	// Constructs an entity based off some very vague save data.
	m_id = saveData.id;
	m_velocity = saveData.velocity;
	m_position = saveData.position;
	m_layer = saveData.layer;
	m_metaData = saveData.md;
}

Entity* createEntity(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex) {
	unsigned int idTest = XMLModule::XMLData::getEntityNPCData(id).id;

	if(idTest != (unsigned int) - 1) {
		// An NPC
		return new EntityNPC(pos, layer, id, data, loadTex);
	}
	idTest = XMLModule::XMLData::getEntityItemData(id).id;
	if(idTest != (unsigned int) - 1) {
		// An Item
		return new EntityItem(pos, layer, id, data, loadTex);
	}
	idTest = XMLModule::XMLData::getEntityProjectileData(id).id;
	if(idTest != (unsigned int) - 1) {
		// An Projectile
		return new EntityProjectile(pos, layer, id, data, loadTex);
	} else {
		Logger::getInstance()->log("Failed to create entity with ID: " + std::to_string(id), true);
		return nullptr;
	}
}

Entity* createEntity(glm::vec2 pos, unsigned int layer, EntityIDs id, SaveDataTypes::MetaData data, bool loadTex) {
	unsigned int idV = (unsigned int)id;

	return createEntity(pos, layer, idV, data, loadTex);
}


Entity::~Entity() {

}

void Entity::update(World* world, float timeStep, unsigned int selfIndex) {
	if(m_updateScriptId != -1) {
		ScriptingModule::ScriptQueue::activateScript(m_updateScriptId, generateLuaValues());
	}

	move(1); /// Fix timestepping, make sure that each step, the entity collides :facepalm:
	interact(world);
	updateLightLevel(world);

	onUpdate(world, timeStep, selfIndex);
}

void Entity::tick(World* world) {
	if(m_tickScriptId != -1) {
		ScriptingModule::ScriptQueue::activateScript(m_tickScriptId, generateLuaValues());
	}
	onTick(world);
}

void Entity::draw(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset) {

	if(m_draw) {
		if(m_textureId == (GLuint) - 1) {
			loadTexture();
		}

		glm::vec4 destRect = glm::vec4(m_position.x + (xOffset * CHUNK_SIZE), m_position.y, m_size.x, m_size.y);

		int x = 0, y = 0;

		glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

		GLEngine::ColourRGBA8 colour(255, 255, 255, 255);

		float depth = getDepth();

		sb.draw(destRect, uvRect, m_textureId, depth, colour);


		onDraw(sb, time, layerDifference, xOffset);
	}
}

void Entity::drawNormal(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset) {

	//GLint lightUniform = program->getUniformLocation("lightColour");
	//glUniform3fv(lightUniform, 3, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);

	if(m_draw) {
		if(m_bumpMapId == (GLuint) - 1) {
			loadTexture();
		}

		glm::vec4 destRect = glm::vec4(m_position.x + (xOffset * CHUNK_SIZE), m_position.y, m_size.x, m_size.y);

		int x = 0, y = 0;

		glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

		GLEngine::ColourRGBA8 colour(255, 255, 255, 255);

		float depth = getDepth();

		sb.draw(destRect, uvRect, m_bumpMapId, depth, colour);
	}
}

void Entity::debugDraw(GLEngine::DebugRenderer& dr, float xOffset) {
	/*for(unsigned int i = 0; i < m_targets.size(); i++) {
	    GLEngine::ColourRGBA8 c(255, 255, 255, 255);
	    if(i == m_curTarget) c.r = 0;
	    dr.drawCircle(glm::vec2(m_targets[i].x + 0.5f, m_targets[i].y + 0.5f), c, 0.2f);
	}*/
}

void Entity::move(float timeStepVariable) {
	if(m_velocity.x > MAX_SPEED) m_velocity.x = MAX_SPEED; // MAX_SPEED is here just to make sure entities don't go too fast and clip through blocks
	if(m_velocity.x < -MAX_SPEED) m_velocity.x = -MAX_SPEED;
	if(m_velocity.y > MAX_SPEED) m_velocity.y = MAX_SPEED;
	if(m_velocity.y < -MAX_SPEED) m_velocity.y = -MAX_SPEED;

	m_position += m_velocity * timeStepVariable;

	if((int)m_position.x > WORLD_SIZE) {
		m_position.x -= WORLD_SIZE;
	} else if((int)m_position.x < 0) {
		m_position.x += WORLD_SIZE;
	}

	m_velocity.y -= 1.225f / 60.0f * timeStepVariable; // Earth gravity is far too harsh for games. We use about 1/8th
}

/// PRIVATE FUNCTIONS

bool Entity::checkTilePosition(World* world, std::vector<glm::vec2>& collideTilePositions, float x, float y) {
	// Get the position of this corner in grid-space
	glm::vec2 gridPos = glm::vec2(floor(x), floor(y)); // grid-space coords

	if(gridPos.y >= 0) {
		// If this is not an air tile, we should collide with it
		if(world->getTile(gridPos.x, gridPos.y, m_layer)->isSolid()) {
			collideTilePositions.push_back(glm::vec2((float)gridPos.x + 0.500f, (float)gridPos.y + 0.500f)); // CollideTilePositions are put in as gridspace coords
			return true;
		}
	}

	return false;
}

void Entity::collideWithTile(glm::vec2 tilePos, bool ground) {
	float x = m_position.x, y = m_position.y;

	glm::vec2 entRelativeCentrePosition; //Relative position of the centre of entity (to bottom left corner)
	entRelativeCentrePosition = glm::vec2(m_size.x / 2.0f, m_size.y / 2.0f);

	glm::vec2 entCentrePosition; //Position of the centre of entity in pixel coords
	// m_size is in grid coords so multiply it by TILE_SIZE
	entCentrePosition = glm::vec2(x + entRelativeCentrePosition.x, y + entRelativeCentrePosition.y);

	glm::vec2 tileRelativeCentrePosition; //Relative position of the centre of tile (to bottom left corner)
	tileRelativeCentrePosition = glm::vec2(0.5f);

	glm::vec2 tileCentrePosition; // Position of the centre of tile in pixel coords
	// tilePos is in grid coords, so mutiply it by TILE_SIZE
	// Don't add relativeCentrePosition because "tilePos" is already centered
	tileCentrePosition = glm::vec2(tilePos);

	glm::vec2 distanceVec; //Distance from centre of tile to centre of entity
	// subtract the points to get total distance, fairly simple
	// Can be used to determine which side the entity or tile is on, positive means entity is on right/top side of tile
	distanceVec = glm::vec2(entCentrePosition - tileCentrePosition);

	glm::vec2 depthVec; //How deep the entity is in the tile
	// difference between how far they should be and how far they currently are
	// Depthvec should always be positive
	depthVec = glm::vec2((entRelativeCentrePosition + tileRelativeCentrePosition) - abs(distanceVec));

	if(ground) { // Only y-direction
		if(abs(depthVec.x / m_size.x) > abs(depthVec.y / m_size.y)) { // Figure out if the entity is coming from the Y direction using proportions of depth:size
			if(distanceVec.y > 0.0f) { // Entity on top side
				m_velocity.y = 0.0f; // Stop it so that it doesn't keep moving inwards
				m_position.y += depthVec.y; // Put the entity back in the logical place
				m_onGround = true; // The entity is on the ground (deactivates gravity)
			} else { // Entity on bottom side
				m_velocity.y = 0.0f; // Stop it so that it doesn't keep moving inwards
				m_position.y -= depthVec.y; // Put the entity back in the logical place
			}
		}
	} else {
		if(abs(depthVec.x / m_size.x) < abs(depthVec.y / m_size.y)) { // Figure out if the entity is coming from the X direction
			if(distanceVec.x > 0.0f) { // Entity on right side
				m_velocity.x = 0.0f; // Stop it so that it doesn't keep moving inwards
				m_position.x += depthVec.x; // Put the entity back in the logical place
			} else { // Entity is on left side
				m_velocity.x = 0.0f; // Stop it so that it doesn't keep moving inwards
				m_position.x -= depthVec.x; // Put the entity back in the logical place
			}
		}
	}
}

void Entity::updateLightLevel(World* world) {
	m_exposedToSun = false;


	// Lighting
	// Find tiles on each corner of the entity
	// Find actual value at the point the entity's corner is at within the block (interpolate from corners)
	// Set corner values.

	glm::vec2 Pos_TL, Pos_TR, Pos_BL, Pos_BR;
	Pos_TL = glm::vec2(m_position.x, m_position.y + m_size.y);
	Pos_TR = glm::vec2(m_position.x + m_size.x, m_position.y + m_size.y);
	Pos_BR = glm::vec2(m_position.x + m_size.x, m_position.y);
	Pos_BL = glm::vec2(m_position.x, m_position.y);

	// Find tiles
	Tile *Tile_TR, *Tile_TL, *Tile_BR, *Tile_BL;
	Tile_TR = world->getTile(Pos_TR.x + 0.5f, Pos_TR.y + 0.5f, m_layer);
	Tile_TL = world->getTile(Pos_TL.x + 0.5f, Pos_TL.y + 0.5f, m_layer);
	Tile_BR = world->getTile(Pos_BR.x + 0.5f, Pos_BR.y + 0.5f, m_layer);
	Tile_BL = world->getTile(Pos_BL.x + 0.5f, Pos_BL.y + 0.5f, m_layer);

	// Find actual values at each of the tiles
	float Light_TR, Light_TL, Light_BR, Light_BL;

	Light_TR = Tile_TR->getLightAtPoint(Pos_TR - Tile_TR->getPosition());
	Light_TL = Tile_TL->getLightAtPoint(Pos_TL - Tile_TL->getPosition());
	Light_BR = Tile_BR->getLightAtPoint(Pos_BR - Tile_BR->getPosition());
	Light_BL = Tile_BL->getLightAtPoint(Pos_BL - Tile_BL->getPosition());

	m_cornerLight = glm::vec4(Light_TL, Light_TR, Light_BR, Light_BL);

	/*if(m_position.y >= 0) {
	    if(world->getTile(m_position.x, m_position.y + m_size.y / 2.0f, m_layer)->getSunLight() != 0.0f) m_exposedToSun = true;
	    if(world->getTile(m_position.x + m_size.x, m_position.y + m_size.y / 2.0f, m_layer)->getSunLight() != 0.0f) m_exposedToSun = true;
	    m_light /= 2.0f; /// TODO: Fix entity lighting
	}*/
}

void Entity::interact(World* world) {
	if(m_onGround && std::abs(m_velocity.x) > 0.1f) {
		glm::vec2 tileCoordsFloor = glm::vec2((int)(m_position.x + 0.5f), (int)(m_position.y - 0.5f));
		world->getTile(tileCoordsFloor.x, tileCoordsFloor.y, m_layer)->onInteract_WalkedOn();
	}
}

void Entity::moveUpLayer(World* world) {
	if(m_layer > 0) {
		m_layer--;
		std::vector<glm::vec2> tiles;

		for(int i = 0; i < m_size.x; i++) {
			for(int j = 0; j < m_size.y; j++) {
				if(checkTilePosition(world, tiles, m_position.x + i, m_position.y + j)) {
					m_layer++;
					return;
				}
			}
		}
	}
}

void Entity::moveDownLayer(World* world) {
	if(m_layer < WORLD_DEPTH - 1) {
		m_layer++;
		std::vector<glm::vec2> tiles;

		for(int i = 0; i < m_size.x; i++) {
			for(int j = 0; j < m_size.y; j++) {
				if(checkTilePosition(world, tiles, m_position.x + i, m_position.y + j)) {
					m_layer--;
					return;
				}
			}
		}
	}
}

void Entity::loadTexture() {
	GLEngine::GLTexture temp;
	temp = GLEngine::ResourceManager::getTexture(m_texturePath);
	m_textureId = temp.id;
	//m_animationFramesX = temp.width / (32 * m_size.x);
	//m_animationFramesY = temp.height / (32 * m_size.y);

	temp = GLEngine::ResourceManager::getTexture(m_bumpMapPath);
	m_bumpMapId = temp.id;
}

void Entity::generateUUID(World* world) {
	if(m_UUID != "NO_UUID") {
		return;
	}
	std::string timeString = std::to_string(world->getTime());
	std::string rand1 = std::to_string(std::rand());
	std::string rand2 = std::to_string(std::rand());
	std::string rand3 = std::to_string(std::rand());
	std::string xString = std::to_string(m_position.x);
	std::string yString = std::to_string(m_position.y);

	std::string UUID = timeString + rand1 + rand2 + rand3 + xString + yString;

	m_UUID = UUID;

	std::string key = "UUID";
	m_metaData.setElement(key, UUID);

}
