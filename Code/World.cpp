#include "World.h"

#include <cstdlib>
#include "Tile.h"
#include "EntityPlayer.h"

#include "XMLData.h"

#include "EventQueue.h"

#include "Factory.h"

World::World() {
	XMLModule::XMLData::init(); /// TODO: Move elsewhere.

	m_tiles = (Tile****)malloc(WORLD_HEIGHT * WORLD_SIZE * WORLD_DEPTH * sizeof(Tile));
	for(int y = 0; y < WORLD_HEIGHT; y++) {
		m_tiles[y] = (Tile***)malloc(WORLD_SIZE * WORLD_DEPTH * sizeof(Tile));
		for(int x = 0; x < WORLD_SIZE; x++) {
			m_tiles[y][x] = (Tile**)malloc(WORLD_DEPTH * sizeof(Tile));
			for(int z = 0; z < WORLD_DEPTH; z++) {
				m_tiles[y][x][z] = new Tile();
			}
		}
	}

	Factory::getEntityManager()->init(this);
}

World::~World() {
	free(m_tiles);
}

void World::setTile(Tile* tile) {
	setTile_noEvent(tile);

	unsigned int x, y, layer, id;
	x = tile->getPosition().x;
	y = tile->getPosition().y;
	layer = tile->getLayer();
	id = tile->getID();

	specialUpdateTile(m_tiles[y][x][layer]);

	std::vector<ScriptingModule::Argument> args = { ScriptingModule::Argument("blockID", std::to_string(id)),
	                                                ScriptingModule::Argument("blockX", std::to_string(x)),
	                                                ScriptingModule::Argument("blockY", std::to_string(y)),
	                                                ScriptingModule::Argument("blockLayer", std::to_string(layer))
	                                              };
	EventModule::EventQueue::triggerEvent("setTile", args);
	/*Tile* b = getTile(x, y, layer+1);
	if(b) b->setNeedsSunCheck();
	Tile* f = getTile(x, y, layer-1);
	if(f) f->setNeedsSunCheck();*/
}

void World::setTile_noEvent(Tile* tile) {
	unsigned int x, y, layer;
	x = tile->getPosition().x;
	y = tile->getPosition().y;
	layer = tile->getLayer();

	if(tile->getEmittedLight() > 0.0f) {
		addLight(tile);
	}

	if(m_tiles[y][x][layer]->getEmittedLight() > 0.0f) {
		removeLight(m_tiles[y][x][layer]);
		m_tiles[y][x][layer]->resetNeighboursLight(this);
	}

	tile->setAmbientLight(m_tiles[y][x][layer]->getAmbientLight());

	m_tiles[y][x][layer]->destroy(this); // Make sure everything gets cleaned up nicely.

	m_tiles[y][x][layer] = tile;

	for(int i = y - 1; i >= 0; i--) {
		Tile* t = m_tiles[i][x][layer];
		t->setNeedsSunCheck();
		Tile* l = getTile(x - 1, i, layer);
		if(l) l->setNeedsSunCheck();
		Tile* r = getTile(x + 1, i, layer);
		if(r) r->setNeedsSunCheck();
		if(!t->isTransparent()) {
			break;
		}
	}
	Tile* l = getTile(x - 1, y, layer);
	if(l) l->setNeedsSunCheck();
	Tile* r = getTile(x + 1, y, layer);
	if(r) r->setNeedsSunCheck();
}

Tile* World::getTile(int x, int y, int layer) {
	if(y < WORLD_HEIGHT && y >= 0) {
		if(layer >= 0 && layer < WORLD_DEPTH) {
			return m_tiles[y][(x + WORLD_SIZE) % WORLD_SIZE][layer];
		}
	}
	return nullptr;
}

XMLModule::BiomeData World::getBiome(int x) {
	// Get chunk position
	int chunkX = (unsigned int)(x / CHUNK_SIZE);

	// Get biome's id from position:
	unsigned int biomeID = m_biomesMap[chunkX];

	// Get biome data from XML Data
	return XMLModule::XMLData::getBiomeData(biomeID);

}

void World::sortLights() {
	bool changed = true;
	while(changed) {
		changed = false;
		for(unsigned int i = 0; i < m_lights.size() - 1; i++) {
			if(m_lights[i]->getPosition().x > m_lights[i + 1]->getPosition().x) {
				Tile* temp = m_lights[i];
				m_lights[i] = m_lights[i + 1];
				m_lights[i + 1] = temp;
				changed = true;
			}
		}
	}
}

void World::addLight(Tile* t) {
	/** ADDS TILE TO VECTOR, MAKES SURE IT'S SORTED */

	/*
	    Push back the tile to the end.
	    Check tile with index 1 less. If their x pos is more than t, swap 'em!
	    Repeat until the tile with a lower index's x pos is less than t's
	*/

	m_lights.push_back(t);
	for(int i = m_lights.size() - 1; i > 0; i--) {
		if(m_lights[i]->getPosition().x > t->getPosition().x) {
			Tile* temp = m_lights[i];
			m_lights[i] = t;
			m_lights[i + 1] = temp;
		} else {
			return;
		}
	}
}

void World::removeLight(unsigned int index) {
	/** REMOVES TILE FROM VECTOR. NOTHING MORE.
	*/

	for(unsigned int i = index; i < m_lights.size() - 1; i++) {
		m_lights[i] = m_lights[i + 1];
	}

	m_lights.pop_back();
}

void World::removeLight(Tile* t) {
	/** REMOVES TILE FROM VECTOR. NOTHING MORE.
	*/

	int index = -1;

	for(unsigned int i = 0; i < m_lights.size(); i++) {
		if(m_lights[i] == t) index = i;
	}

	if(index != -1) {
		for(unsigned int i = index; i < m_lights.size() - 1; i++) {
			m_lights[i] = m_lights[i + 1];
		}

		m_lights.pop_back();
	} else {
		Logger::getInstance()->log("LIGHT NOT FOUND!", true);
	}
}

void World::getRenderedLights(glm::vec4 destRect, float lights[MAX_LIGHTS_RENDERED * 3]) {
	/*
	    Gets all the lights that should be rendered this frame, using a viewport rectangle (destRect). Returns glm::vec3 array of length MAX_LIGHTS_RENDERED
	    Algorithm:
	    1. Check light's range (sqrt(abs(emittedLight)) * 4.0f)
	    2. If light's position +/- range is a position within the viewport, create a vec3 (x&y are pos, z is emittedLight). Append to return array
	    3. If it cannot be appended (number added >= MAX_LIGHTS_RENDERED), change it out for one whose distance from the scene is further.
	    4. Repeat for every light in m_lights.
	*/

	glm::vec3 returnVal[MAX_LIGHTS_RENDERED];

	for(unsigned int i = 0; i < MAX_LIGHTS_RENDERED; i++) {
		returnVal[i] = glm::vec3(0.0);
	}

	unsigned int added = 0; // number of lights added. Equal to next index in returnVal to set.

	for(int i = 0; i < m_lights.size(); i++) {
		// 1.
		float range = std::sqrt(m_lights[i]->getEmittedLight()) * 4.0f;

		// 2.
		bool append = false;

		// Get distance from centre of viewport to centre of light
		float leftDist; // going left from viewport
		float rightDist;

		float centerXViewport = destRect.x + (destRect.z / 2.0f);

		if(centerXViewport > m_lights[i]->getPosition().x) {
			leftDist = -(centerXViewport - m_lights[i]->getPosition().x);
			rightDist = (WORLD_SIZE - centerXViewport) + m_lights[i]->getPosition().x;
		} else {
			leftDist = -((WORLD_SIZE - m_lights[i]->getPosition().x) + centerXViewport);
			rightDist = m_lights[i]->getPosition().x - centerXViewport;
		}

		float shortestDist = abs(leftDist) < rightDist ? leftDist : rightDist;
		if(abs(shortestDist) <= (range + destRect.z) / 2.0f) {
			append = true;
		}


		if(append) {
			glm::vec3 result = glm::vec3(centerXViewport + shortestDist, m_lights[i]->getPosition().y, m_lights[i]->getEmittedLight());
			// 3.
			if(added >= MAX_LIGHTS_RENDERED) {
				glm::vec2 center(destRect.x + destRect.z / 2.0f, destRect.y + destRect.w / 2.0f);
				float maxDist = glm::distance(glm::vec2(result.x, result.y), center); // Distance from the centre of the viewport
				int maxDistIndex = -1;
				for(int i = 0; i < MAX_LIGHTS_RENDERED; i++) {
					float IDistance = glm::distance(glm::vec2(returnVal[i].x, returnVal[i].y), center);
					if(IDistance > maxDist) {
						maxDist = IDistance;
						maxDistIndex = i;
					}
				}
				if(maxDistIndex != -1) {
					returnVal[maxDistIndex] = result;
				}
			} else {
				returnVal[added] = result;
				added++;
			}
		}
	}

	for(int i = 0; i < MAX_LIGHTS_RENDERED; i++) {
		glm::vec2 pos = Factory::getGameCamera()->convertWorldToScreen(glm::vec2(returnVal[i].x + 0.5f, returnVal[i].y + 0.5f));

		lights[i * 3] = pos.x;
		lights[i * 3 + 1] = pos.y;
		lights[i * 3 + 2] = returnVal[i].z * Factory::getGameCamera()->getScale() * Factory::getGameCamera()->getScale();
	}
}

void World::setLightsUniform(glm::vec4 destRect, GLEngine::GLSLProgram* textureProgram) {
	float lights[MAX_LIGHTS_RENDERED * 3];
	getRenderedLights(destRect, lights);
	GLint textureUniform = textureProgram->getUniformLocation("lights");
	glUniform3fv(textureUniform, MAX_LIGHTS_RENDERED, lights); /// TODO: Set define directive for 30 lights max.
}

void World::drawTiles(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, GLEngine::DebugRenderer& dr, glm::vec4 destRect, GLEngine::GLSLProgram* textureProgram) {
	/**
	    Draws an area of tiles at position destRect.xy, with width and height of destRect.z and destRect.w respectively.
	    Negative coordinates are mapped to accomodate for 'crossover'
	    eg. destRect = (-10, 10, 20, 10) will draw tiles from x=(WORLD_SIZE - 10) to x=(-10 + 20) and y=(10) to y=(10 + 10)
	*/

	EntityPlayer* player = Factory::getEntityManager()->getPlayer();

	int playerLayer = player->getLayer();
	int diff[WORLD_DEPTH];
	float blur[WORLD_DEPTH];
	for(int i = 0; i < WORLD_DEPTH; i++) {
		diff[i] = (playerLayer - i);
	}

	for(int x = destRect.x; x < destRect.z + destRect.x; x++) {
		int columnIndex = (int)((x/*+destRect.x*/) + (WORLD_SIZE)) % WORLD_SIZE;
		int offset = (/*destRect.x + */x) - columnIndex;

		for(int y = destRect.y; y < destRect.w + destRect.y; y++) {
			for(unsigned int layer = 0; layer < WORLD_DEPTH; layer++) {
				if(y >= 0) {
					if(y < WORLD_HEIGHT) {
						m_tiles[y][columnIndex][layer]->draw(sb, sf, offset, diff[layer]);
						if(!m_tiles[y][columnIndex][layer]->isTransparent()) {
							break;
						}
					}
				}
			}
		}
	}
}

void World::drawTilesNormal(GLEngine::SpriteBatch& sb, glm::vec4 destRect, GLEngine::GLSLProgram* textureProgram) {
	EntityPlayer* player = Factory::getEntityManager()->getPlayer();

	int playerLayer = player->getLayer();
	int diff[WORLD_DEPTH];
	float blur[WORLD_DEPTH];
	for(int i = 0; i < WORLD_DEPTH; i++) {
		diff[i] = (playerLayer - i);
	}

	for(int x = destRect.x; x < destRect.z + destRect.x; x++) {
		int columnIndex = (int)((x/*+destRect.x*/) + (WORLD_SIZE)) % WORLD_SIZE;
		int offset = (/*destRect.x + */x) - columnIndex;

		for(int y = destRect.y; y < destRect.w + destRect.y; y++) {
			for(unsigned int layer = 0; layer < WORLD_DEPTH; layer++) {
				if(y >= 0) {
					if(y < WORLD_HEIGHT) {
						m_tiles[y][columnIndex][layer]->drawNormal(sb, offset, diff[layer]);
						if(!m_tiles[y][columnIndex][layer]->isTransparent()) {
							break;
						}
					}
				}
			}
		}
	}
}

void World::updateTiles(glm::vec4 destRect) {
	/**
	    Simply updates an area of tiles at position destRect.xy, with width and height of destRect.z and destRect.w respectively.
	    Negative coordinates are mapped to accomodate for 'crossover'
	    eg. destRect = (-10, 10, 20, 10) will update tiles from x=(WORLD_SIZE - 10) to x=(-10 + 20) and y=(10) to y=(10 + 10)
	*/

	/*for(int i = 0; i < m_deadTiles.size(); i++) {
	    m_deadTiles[i]->destroy(this);
	    delete m_deadTiles[i];
	}
	m_deadTiles.clear();*/ // This breaks stuff apparently

	const float s = m_sunlight;

	for(int y = destRect.y; y < destRect.w + destRect.y; y++) {
		for(int x = destRect.x; x < destRect.z + destRect.x; x++) {
			for(unsigned int layer = 0; layer < WORLD_DEPTH; layer++) {
				if(y >= 0 && y < WORLD_HEIGHT) {
					(m_tiles[y][(x + WORLD_SIZE) % WORLD_SIZE][layer])->update(this, m_time, true, s);
				}
			}
		}
	}
}

void World::tickTiles(glm::vec4 destRect) {
	/**
	    Simply ticks an area of tiles at position destRect.xy, with width and height of destRect.z and destRect.w respectively.
	    Negative coordinates are mapped to accomodate for 'crossover'
	    eg. destRect = (-10, 10, 20, 10) will tick tiles from x=(WORLD_SIZE - 10) to x=(-10 + 20) and y=(10) to y=(10 + 10)
	*/

	const float sunlight = std::cos(m_time / (DAY_LENGTH / 6.28318f)) / 2.0f + 0.5f;
	m_sunlight = sunlight;

	for(int y = destRect.y; y < destRect.w + destRect.y; y++) {
		for(int x = destRect.x; x < destRect.z + destRect.x; x++) {
			for(unsigned int layer = 0; layer < WORLD_DEPTH; layer++) {
				if(y >= 0 && y < WORLD_HEIGHT) {
					(m_tiles[y][(x + WORLD_SIZE) % WORLD_SIZE][layer])->tick(this, m_time, sunlight);
				}
			}
		}
	}
}

void World::drawDebug(GLEngine::DebugRenderer& dr, float xOffset) {
	//for(unsigned int i = 0; i < m_entities.size(); i++) {
	//	m_entities[i]->debugDraw(dr, 0.0f); /// TODO: Finish these entity functions up and improve!
	//}
	//dr.end();
}

void World::drawSunlight(GLEngine::SpriteBatch& sb, glm::vec4 destRect) {
	sb.begin();

	for(float y = destRect.y; y < (destRect.y + destRect.w); y++) {
		if((int)y < 1 || (int)y > WORLD_HEIGHT - 2) {
			continue;
		}
		for(float x = destRect.x; x < (destRect.x + destRect.z); x++) {
			// Loop though all tiles in the destRect.

			/*
			 *
			 * Each corner can be described in a bit. 4 bits for four corners. Room for expansion.
			 *
			 * */

			// Determine actual light value (R)
			Tile* t = m_tiles[(int)y][((int)x + WORLD_SIZE) % WORLD_SIZE][0];

			int light = (t->getSunlight() * 255.0f);

			int corners = 0; // Describes whether each corner is lit. (G)

			// Determine TL (bit 0)
			corners |= (t->getSunlightCorners().x > 0.0f);

			// Determine TR (bit 1)
			corners |= (t->getSunlightCorners().y > 0.0f) << 1;

			// Determine BL (bit 2)
			corners |= (t->getSunlightCorners().w > 0.0f) << 2;

			// Determine BR (bit 3)
			corners |= (t->getSunlightCorners().z > 0.0f) << 3;

			sb.draw(glm::vec4((int)x, (int)y, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0, 0.0f, GLEngine::ColourRGBA8(light, corners, 255, 255));
		}
	}

	sb.end();
	sb.renderBatch();
}

float World::getDistance(glm::vec2 point0, glm::vec2 point1) {
	float xDist;

	if(point0.x < point1.x) {
		float xDist0 = point1.x - point0.x; // Regular dist not accounting for crossover.
		float xDist1 = (point0.x) + (WORLD_SIZE - point1.x); // Distance accounting for crossover.

		xDist = xDist0 < xDist1 ? xDist0 : xDist1; // Choose the shorter of the two.
	} else if(point0.x > point1.x) {
		float xDist0 = point0.x - point1.x; // Regular dist not accounting for crossover.
		float xDist1 = (point1.x) + (WORLD_SIZE - point0.x); // Distance accounting for crossover.

		xDist = xDist0 < xDist1 ? xDist0 : xDist1; // Choose the shorter of the two.
	}

	float yDist = std::abs(point0.y - point1.y);

	return std::sqrt(xDist * xDist + yDist * yDist);
}


/// PRIVATE FUNCTIONS BELOW

void World::specialUpdateTile(Tile* origin) {
	origin->specialUpdate(this, m_time);

	glm::vec2 oPos = origin->getPosition();

	Tile* l = getTile(oPos.x - 1, oPos.y, origin->getLayer());
	Tile* r = getTile(oPos.x + 1, oPos.y, origin->getLayer());
	Tile* u = getTile(oPos.x, oPos.y + 1, origin->getLayer());
	Tile* d = getTile(oPos.x, oPos.y - 1, origin->getLayer());
	Tile* f = getTile(oPos.x, oPos.y, origin->getLayer() - 1);
	Tile* b = getTile(oPos.x, oPos.y, origin->getLayer() + 1);

	// left and right will always return a tile
	l->specialUpdate(this, m_time);
	r->specialUpdate(this, m_time);


	if(u)
		u->specialUpdate(this, m_time);
	if(d)
		d->specialUpdate(this, m_time);


	if(f)
		f->specialUpdate(this, m_time);
	if(b)
		b->specialUpdate(this, m_time);

}
