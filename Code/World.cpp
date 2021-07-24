#include "World.h"

#include <cstdlib>
#include "Tile.h"
#include "FluidDomain.h"
#include "EntityPlayer.h"

#include "XMLData.h"

#include "EventQueue.h"

#include "Singletons.h"

World::World(unsigned int xSize, unsigned int ySize, unsigned int zSize) {
	initTiles(xSize, ySize, zSize);
	initFluids();

	Singletons::setWorld(this);

	Singletons::getEntityManager()->init(this);
}

World::~World() {
	//free(m_tiles);

	for(unsigned int x = 0; x < m_tiles.size(); x++) {
		for(unsigned int y = 0; y < m_tiles[x].size(); y++) {
			for(unsigned int z = 0; z < m_tiles[x][y].size(); z++) {
				delete m_tiles[x][y][z];
			}
		}
	}
}

void World::initTiles(unsigned int xSize, unsigned int ySize, unsigned int zSize) {
	// Clear tiles just in case we already did this.
	for(unsigned int x = 0; x < m_tiles.size(); x++) {
		for(unsigned int y = 0; y < m_tiles[x].size(); y++) {
			for(unsigned int z = 0; z < m_tiles[x][y].size(); z++) {
				delete m_tiles[x][y][z];
			}
			m_tiles[y].clear();
		}
		m_tiles[x].clear();
	}
	m_tiles.clear();

	// Make/reserve tiles
	m_tiles.reserve(xSize);
	for(unsigned int x = 0; x < xSize; x++) {
		std::vector<std::vector<Tile*>> tempX;
		for(unsigned int y = 0; y < ySize; y++) {
			std::vector<Tile*> tempY;
			for(unsigned int z = 0; z < zSize; z++) {
				tempY.push_back(new Tile());
			}
			tempX.push_back(tempY);
		}
		m_tiles.push_back(tempX);
	}

	m_biomesMap.resize(xSize / CHUNK_SIZE);
}

void World::initFluids() {
	// Clear fluids, just in case we're doing this again.
	for(unsigned int i = 0; i < m_fluidDomains.size(); i++) {
		delete m_fluidDomains[i];
	}
	m_fluidDomains.clear();

	unsigned int fluidCount =
		XMLModule::XMLData::getFluidCount(); // Number of fluids that can be placed, therefore number of domains we need.

	for(unsigned int i = 0; i < fluidCount; i++) {
		m_fluidDomains.push_back(new FluidModule::FluidDomain(m_tiles, i));
	}
}

void World::setTile(Tile* tile) {
	setTile_noEvent(tile);

	unsigned int x, y, layer, id;
	x	  = tile->getPosition().x;
	y	  = tile->getPosition().y;
	layer = tile->getLayer();
	id	  = tile->getID();

	specialUpdateTile(m_tiles[x][y][layer]);

	std::vector<ScriptingModule::Argument> args = {ScriptingModule::Argument("blockID", std::to_string(id)),
												   ScriptingModule::Argument("blockX", std::to_string(x)),
												   ScriptingModule::Argument("blockY", std::to_string(y)),
												   ScriptingModule::Argument("blockLayer", std::to_string(layer))};
	EventModule::EventQueue::triggerEvent("setTile", args);
	/*Tile* b = getTile(x, y, layer+1);
	if(b) b->setNeedsSunCheck();
	Tile* f = getTile(x, y, layer-1);
	if(f) f->setNeedsSunCheck();*/
}

void World::setTile_noEvent(Tile* tile) {
	unsigned int x, y, layer;
	x	  = tile->getPosition().x;
	y	  = tile->getPosition().y;
	layer = tile->getLayer();

	if(tile->getEmittedLight() > 0.0f) {
		addLight(tile);
	}

	if(m_tiles[x][y][layer]->getEmittedLight() > 0.0f) {
		removeLight(m_tiles[x][y][layer]);
		m_tiles[x][y][layer]->resetNeighboursLight();
	}

	tile->setAmbientLight(m_tiles[x][y][layer]->getAmbientLight());

	m_tiles[x][y][layer]->destroy(); // Make sure everything gets cleaned up nicely.

	m_tiles[x][y][layer] = tile;

	for(int i = y - 1; i >= 0; i--) {
		Tile* t = m_tiles[x][i][layer];
		t->setNeedsSunCheck();
		Tile* l = getTile(x - 1, i, layer);
		if(l)
			l->setNeedsSunCheck();
		Tile* r = getTile(x + 1, i, layer);
		if(r)
			r->setNeedsSunCheck();
		if(!t->isTransparent()) {
			break;
		}
	}
	Tile* l = getTile(x - 1, y, layer);
	if(l)
		l->setNeedsSunCheck();
	Tile* r = getTile(x + 1, y, layer);
	if(r)
		r->setNeedsSunCheck();

	for(unsigned int i = 0; i < m_fluidDomains.size(); i++) {
		m_fluidDomains[i]->updateField(tile);
	}
}

Tile* World::getTile(int x, int y, int layer) {
	if(y < WORLD_HEIGHT && y >= 0) {
		if(layer >= 0 && layer < WORLD_DEPTH) {
			return m_tiles[(x + getSize()) % getSize()][y][layer];
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

FluidModule::FluidDomain* World::getFluid(unsigned int index) {
	return m_fluidDomains[index];
}

void World::sortLights() {
	bool changed = true;
	while(changed) {
		changed = false;
		for(unsigned int i = 0; i < m_lights.size() - 1; i++) {
			if(m_lights[i]->getPosition().x > m_lights[i + 1]->getPosition().x) {
				Tile* temp		= m_lights[i];
				m_lights[i]		= m_lights[i + 1];
				m_lights[i + 1] = temp;
				changed			= true;
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
			Tile* temp		= m_lights[i];
			m_lights[i]		= t;
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
		if(m_lights[i] == t)
			index = i;
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
			leftDist  = -(centerXViewport - m_lights[i]->getPosition().x);
			rightDist = (getSize() - centerXViewport) + m_lights[i]->getPosition().x;
		} else {
			leftDist  = -((getSize() - m_lights[i]->getPosition().x) + centerXViewport);
			rightDist = m_lights[i]->getPosition().x - centerXViewport;
		}

		float shortestDist = abs(leftDist) < rightDist ? leftDist : rightDist;
		if(abs(shortestDist) <= (range + destRect.z) / 2.0f) {
			append = true;
		}

		if(append) {
			glm::vec3 result =
				glm::vec3(centerXViewport + shortestDist, m_lights[i]->getPosition().y, m_lights[i]->getEmittedLight());
			// 3.
			if(added >= MAX_LIGHTS_RENDERED) {
				glm::vec2 center(destRect.x + destRect.z / 2.0f, destRect.y + destRect.w / 2.0f);
				float	  maxDist =
					glm::distance(glm::vec2(result.x, result.y), center); // Distance from the centre of the viewport
				int maxDistIndex = -1;
				for(int i = 0; i < MAX_LIGHTS_RENDERED; i++) {
					float IDistance = glm::distance(glm::vec2(returnVal[i].x, returnVal[i].y), center);
					if(IDistance > maxDist) {
						maxDist		 = IDistance;
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
		glm::vec2 pos =
			Singletons::getGameCamera()->convertWorldToScreen(glm::vec2(returnVal[i].x + 0.5f, returnVal[i].y + 0.5f));

		lights[i * 3]	  = pos.x;
		lights[i * 3 + 1] = pos.y;
		lights[i * 3 + 2] =
			returnVal[i].z * Singletons::getGameCamera()->getScale() * Singletons::getGameCamera()->getScale();
	}
}

void World::setLightsUniform(glm::vec4 destRect, GLEngine::GLSLProgram* textureProgram) {
	float lights[MAX_LIGHTS_RENDERED * 3];
	getRenderedLights(destRect, lights);
	GLint textureUniform = textureProgram->getUniformLocation("lights");
	glUniform3fv(textureUniform, MAX_LIGHTS_RENDERED, lights); /// TODO: Set define directive for 30 lights max.
}

void World::drawTiles(GLEngine::SpriteBatch&   sb,
					  GLEngine::SpriteFont&	   sf,
					  GLEngine::DebugRenderer& dr,
					  glm::vec4				   destRect,
					  GLEngine::GLSLProgram*   textureProgram) {
	/**
	    Draws an area of tiles at position destRect.xy, with width and height of destRect.z and destRect.w respectively.
	    Negative coordinates are mapped to accomodate for 'crossover'
	    eg. destRect = (-10, 10, 20, 10) will draw tiles from x=(WORLD_SIZE - 10) to x=(-10 + 20) and y=(10) to y=(10 + 10)
	*/

	EntityPlayer* player = Singletons::getEntityManager()->getPlayer();

	int	  playerLayer = player->getLayer();
	int	  diff[WORLD_DEPTH];
	float blur[WORLD_DEPTH];
	for(int i = 0; i < WORLD_DEPTH; i++) {
		diff[i] = (playerLayer - i);
	}

	for(int x = destRect.x; x < destRect.z + destRect.x; x++) {
		int columnIndex = (int)((x /*+destRect.x*/) + (getSize())) % getSize();
		int offset		= (/*destRect.x + */ x) - columnIndex;

		for(int y = destRect.y; y < destRect.w + destRect.y; y++) {
			for(unsigned int layer = 0; layer < WORLD_DEPTH; layer++) {
				if(y >= 0) {
					if(y < WORLD_HEIGHT) {
						m_tiles[columnIndex][y][layer]->draw(sb, sf, offset, diff[layer]);
						m_tiles[columnIndex][y][layer]->drawGUI(sb, sf, offset);
						if(!m_tiles[columnIndex][y][layer]->isTransparent()) {
							break;
						}
					}
				}
			}
		}
	}
}

void World::drawTilesNormal(GLEngine::SpriteBatch& sb, glm::vec4 destRect, GLEngine::GLSLProgram* textureProgram) {
	EntityPlayer* player = Singletons::getEntityManager()->getPlayer();

	int	  playerLayer = player->getLayer();
	int	  diff[WORLD_DEPTH];
	float blur[WORLD_DEPTH];
	for(int i = 0; i < WORLD_DEPTH; i++) {
		diff[i] = (playerLayer - i);
	}

	for(int x = destRect.x; x < destRect.z + destRect.x; x++) {
		int columnIndex = (int)((x /*+destRect.x*/) + (getSize())) % getSize();
		int offset		= (/*destRect.x + */ x) - columnIndex;

		for(int y = destRect.y; y < destRect.w + destRect.y; y++) {
			for(unsigned int layer = 0; layer < WORLD_DEPTH; layer++) {
				if(y >= 0) {
					if(y < WORLD_HEIGHT) {
						m_tiles[columnIndex][y][layer]->drawNormal(sb, offset, diff[layer]);
						if(!m_tiles[columnIndex][y][layer]->isTransparent()) {
							break;
						}
					}
				}
			}
		}
	}
}

void World::drawTilesGUI(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, glm::vec4 destRect) {
	for(int x = destRect.x; x < destRect.z + destRect.x; x++) {
		int columnIndex = (int)((x) + (getSize())) % getSize();
		int offset		= (x)-columnIndex;

		for(int y = destRect.y; y < destRect.w + destRect.y; y++) {
			for(unsigned int layer = 0; layer < WORLD_DEPTH; layer++) {
				if(y >= 0) {
					if(y < WORLD_HEIGHT) {
						m_tiles[columnIndex][y][layer]->drawGUI(sb, sf, offset);
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

	for(int y = destRect.y; y < destRect.w + destRect.y; y++) {
		for(int x = destRect.x; x < destRect.z + destRect.x; x++) {
			for(unsigned int layer = 0; layer < WORLD_DEPTH; layer++) {
				if(y >= 0 && y < WORLD_HEIGHT) {
					(m_tiles[(x + getSize()) % getSize()][y][layer])->update(m_time, true, m_sunlight);
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

	m_sunlight = std::cos(m_time / (DAY_LENGTH / 6.28318f)) / 2.0f + 0.5f;
	;

	for(int y = destRect.y; y < destRect.w + destRect.y; y++) {
		if(y < -destRect.w)
			break;
		for(int x = destRect.x; x < destRect.z + destRect.x; x++) {
			for(unsigned int layer = 0; layer < WORLD_DEPTH; layer++) {
				if(y >= 0 && y < WORLD_HEIGHT) {
					(m_tiles[(x + getSize()) % getSize()][y][layer])->tick(m_time, m_sunlight);
				} else {
					continue;
				}
			}
		}
	}
	return;
}

void World::drawFluids(GLEngine::SpriteBatch& sb, glm::vec4& destRect) {
	glDisable(GL_DEPTH_TEST); // Make sure that our fluids aren't blocked by... invisible fluids.
	
	for(unsigned int i = 0; i < m_fluidDomains.size(); i++) {
		m_fluidDomains[i]->draw(sb, destRect);
	}
}

void World::updateFluidTextures(glm::vec4& destRect) {
	for(unsigned int i = 0; i < m_fluidDomains.size(); i++) {
		m_fluidDomains[i]->updateTexture(destRect);
	}
}

void World::updateFluids(float timeStep, glm::vec4& destRect) {
	for(unsigned int i = 0; i < m_fluidDomains.size(); i++) {
		m_fluidDomains[i]->update();
	}
}

void World::drawDebug(GLEngine::DebugRenderer& dr, float xOffset) {
	//for(unsigned int i = 0; i < m_entities.size(); i++) {
	//	m_entities[i]->debugDraw(dr, 0.0f); /// TODO: Finish these entity functions up and improve!
	//}
	//dr.end();
}

void World::drawSunlight(GLEngine::SpriteBatch& sb, glm::vec4 destRect) {
	for(float y = destRect.y; y < (destRect.y + destRect.w); y++) {
		if((int)y < 1) {
			y = 0;
			continue;
		}
		if((int)y > WORLD_HEIGHT - 2) {
			y = WORLD_HEIGHT - 1;
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
			Tile* t = m_tiles[((int)x + getSize()) % getSize()][(int)y][0];

			int light = (t->getSunlight() * 255.0f);

			int corners = 0; // Describes whether each corner is lit. (G)

			// Determine TL (bit 0)
			glm::vec4 cornersVec = t->getSunlightCorners();
			corners |= (cornersVec.x > 0.0f);

			// Determine TR (bit 1)
			corners |= (cornersVec.y > 0.0f) << 1;

			// Determine BL (bit 2)
			corners |= (cornersVec.w > 0.0f) << 2;

			// Determine BR (bit 3)
			corners |= (cornersVec.z > 0.0f) << 3;

			sb.draw(glm::vec4((int)x, (int)y, 1.0f, 1.0f),
					glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
					0,
					0.0f,
					GLEngine::ColourRGBA8(light, corners, 255, 255));
		}
	}
}

float World::getDistance(glm::vec2 point0, glm::vec2 point1) {
	float xDist;

	if(point0.x < point1.x) {
		float xDist0 = point1.x - point0.x;					// Regular dist not accounting for crossover.
		float xDist1 = (point0.x) + (getSize() - point1.x); // Distance accounting for crossover.

		xDist = xDist0 < xDist1 ? xDist0 : xDist1; // Choose the shorter of the two.
	} else if(point0.x > point1.x) {
		float xDist0 = point0.x - point1.x;					// Regular dist not accounting for crossover.
		float xDist1 = (point1.x) + (getSize() - point0.x); // Distance accounting for crossover.

		xDist = xDist0 < xDist1 ? xDist0 : xDist1; // Choose the shorter of the two.
	}

	float yDist = std::abs(point0.y - point1.y);

	return std::sqrt(xDist * xDist + yDist * yDist);
}

/// PRIVATE FUNCTIONS BELOW

void World::specialUpdateTile(Tile* origin) {
	origin->specialUpdate(m_time);

	glm::vec2 oPos = origin->getPosition();

	Tile* l = getTile(oPos.x - 1, oPos.y, origin->getLayer());
	Tile* r = getTile(oPos.x + 1, oPos.y, origin->getLayer());
	Tile* u = getTile(oPos.x, oPos.y + 1, origin->getLayer());
	Tile* d = getTile(oPos.x, oPos.y - 1, origin->getLayer());
	Tile* f = getTile(oPos.x, oPos.y, origin->getLayer() - 1);
	Tile* b = getTile(oPos.x, oPos.y, origin->getLayer() + 1);

	// left and right will always return a tile
	l->specialUpdate(m_time);
	r->specialUpdate(m_time);

	if(u)
		u->specialUpdate(m_time);
	if(d)
		d->specialUpdate(m_time);

	if(f)
		f->specialUpdate(m_time);
	if(b)
		b->specialUpdate(m_time);
}
