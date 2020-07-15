#include "EntityManager.h"

#include "EventQueue.h"

#include "World.h"
#include "Tile.h"

#include "Entity.h"
#include "EntityPlayer.h"

EntityManager::EntityManager() {
	// Create the entity manager, log it so we know if multiple are (somehow) made
	Logger::getInstance()->log("EntityManager constructed");
}

EntityManager::~EntityManager() {
	dispose();
}

void EntityManager::init(World* world) {
	// Init (obviously), and log it if we already inited.
	if(m_inited) {
		Logger::getInstance()->log("WARNING: EntityManager attempted to initialize after already doing so!", true);
		return;
	}

	m_world = world;
}

void EntityManager::dispose() {
	// Delete all entities. RAII baybeeeeee!
	for(Entity* e : m_entities) {
		delete e;
	}

	m_entitiesByUUID.clear(); // We don't need to delete these; they are just copies of the pointers in m_entities (already deleted)

	Logger::getInstance()->log("EntityManager deconstructed");
}

void EntityManager::setPlayer(EntityPlayer& p) {
	/// Copies p, creates/updates a pointer to the world-stored player.
	if(!m_player) {
		m_player = new EntityPlayer(glm::vec2(0.0f), 0.0f, SaveDataTypes::MetaData(), false);
		*m_player = p;
		addEntity(m_player);
	} else {
		removeEntity(m_player->getUUID());
		m_player = &p;
		addEntity(&p);
	}
}

void EntityManager::sortEntities() {
	bool changed = true;
	while(changed) {
		changed = false;
		for(unsigned int i = 0; i < m_entities.size() - 1; i++) {
			if(m_entities[i]->getPosition().x > m_entities[i + 1]->getPosition().x) {
				Entity* temp = m_entities[i];
				m_entities[i] = m_entities[i + 1];
				m_entities[i + 1] = temp;
				changed = true;
			}
		}
	}
}

void EntityManager::addEntity(Entity* e) {
	/** ADDS ENTITY TO VECTOR, MAKES SURE IT'S SORTED */

	/*
	    Push back the entity to the end.
	    Check entity with index 1 less. If their x pos is more than e, swap 'em!
	    Repeat until the entity with a lower index's x pos is less than e's
	*/

	if(e->getUUID() == "NO_UUID") {
		e->generateUUID(m_world);
	}

	m_entities.push_back(e);
	for(int i = m_entities.size() - 1; i > 0; i--) {
		if(m_entities[i]->getPosition().x > e->getPosition().x) {
			Entity* temp = m_entities[i];
			m_entities[i] = e;
			m_entities[i + 1] = temp;
		} else {
			return;
		}
	}

	e->generateUUID(m_world);
	m_entitiesByUUID.insert(std::pair<std::string, Entity*>(e->getUUID(), e));

	std::vector<ScriptingModule::Argument> args = { ScriptingModule::Argument("entityUUID", e->getUUID()),
	                                                ScriptingModule::Argument("entityID", std::to_string(e->getID())),
	                                                ScriptingModule::Argument("entityX", std::to_string(e->getPosition().x)),
	                                                ScriptingModule::Argument("entityY", std::to_string(e->getPosition().y)),
	                                                ScriptingModule::Argument("entityLayer", std::to_string(e->getLayer()))
	                                              };
	EventModule::EventQueue::triggerEvent("addEntity", args);
}

void EntityManager::removeEntity(unsigned int index) {
	/** REMOVES AND DELETES ENTITY, COMPLETELY HANDLING MEMORY MANAGEMENT
	*/

	std::vector<ScriptingModule::Argument> args = { ScriptingModule::Argument("entityUUID", m_entities[index]->getUUID()),
	                                                ScriptingModule::Argument("entityID", std::to_string(m_entities[index]->getID())),
	                                                ScriptingModule::Argument("entityX", std::to_string(m_entities[index]->getPosition().x)),
	                                                ScriptingModule::Argument("entityY", std::to_string(m_entities[index]->getPosition().y)),
	                                                ScriptingModule::Argument("entityLayer", std::to_string(m_entities[index]->getLayer()))
	                                              };
	EventModule::EventQueue::triggerEvent("removeEntity", args);

	auto a = m_entitiesByUUID.find(m_entities[index]->getUUID());
	m_entitiesByUUID.erase(a);

	delete m_entities[index];

	for(unsigned int i = index; i < m_entities.size() - 1; i++) {
		m_entities[i] = m_entities[i + 1];
	}

	m_entities.pop_back();
}

void EntityManager::removeEntity(std::string UUID) {
	/** REMOVES AND DELETES ENTITY, COMPLETELY HANDLING MEMORY MANAGEMENT
	*/

	unsigned int index;

	for(int i = 0; i < m_entities.size(); i++) {
		if(m_entities[i] == m_entitiesByUUID[UUID]) {
			index = i;
			break;
		}
	}

	std::vector<ScriptingModule::Argument> args = { ScriptingModule::Argument("entityUUID", m_entities[index]->getUUID()),
	                                                ScriptingModule::Argument("entityID", std::to_string(m_entities[index]->getID())),
	                                                ScriptingModule::Argument("entityX", std::to_string(m_entities[index]->getPosition().x)),
	                                                ScriptingModule::Argument("entityY", std::to_string(m_entities[index]->getPosition().y)),
	                                                ScriptingModule::Argument("entityLayer", std::to_string(m_entities[index]->getLayer()))
	                                              };
	EventModule::EventQueue::triggerEvent("removeEntity", args);

	m_entitiesByUUID.erase(UUID);

	delete m_entities[index];

	for(unsigned int i = index; i < m_entities.size() - 1; i++) {
		m_entities[i] = m_entities[i + 1];
	}

	m_entities.pop_back();
}

void EntityManager::drawEntities(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, GLEngine::DebugRenderer& dr, glm::vec4 destRect) {
	/**
	    Draws an area of tiles at position destRect.xy, with width and height of destRect.z and destRect.w respectively.
	    Negative coordinates are mapped to accomodate for 'crossover'
	    eg. destRect = (-10, 10, 20, 10) will draw tiles from x=(WORLD_SIZE - 10) to x=(-10 + 20) and y=(10) to y=(10 + 10)
	*/

	int playerLayer = m_player->getLayer();
	int diff[WORLD_DEPTH];
	for(int i = 0; i < WORLD_DEPTH; i++) {
		diff[i] = playerLayer - i;
	}

	for(unsigned int i = 0; i < m_entities.size(); i++) {
		m_entities[i]->draw(sb, m_world->getTime(), std::abs(diff[m_entities[i]->getLayer()]) + 1.0, 0.0f); /// TODO: Finish these entity functions up and improve!
	}
}

void EntityManager::drawEntitiesNormal(GLEngine::SpriteBatch& sb, glm::vec4 destRect) {
	/**
	    Draws an area of tiles at position destRect.xy, with width and height of destRect.z and destRect.w respectively.
	    Negative coordinates are mapped to accomodate for 'crossover'
	    eg. destRect = (-10, 10, 20, 10) will draw tiles from x=(WORLD_SIZE - 10) to x=(-10 + 20) and y=(10) to y=(10 + 10)
	*/

	int playerLayer = m_player->getLayer();
	int diff[WORLD_DEPTH];
	for(int i = 0; i < WORLD_DEPTH; i++) {
		diff[i] = playerLayer - i;
	}

	for(unsigned int i = 0; i < m_entities.size(); i++) {
		m_entities[i]->drawNormal(sb, m_world->getTime(), std::abs(diff[m_entities[i]->getLayer()]) + 1.0, 0.0f); /// TODO: Finish these entity functions up and improve!
	}
}


void EntityManager::updateEntities(float timeStep) {
	/**
	    Simply updates an area of tiles at position destRect.xy, with width and height of destRect.z and destRect.w respectively.
	    Negative coordinates are mapped to accomodate for 'crossover'
	    eg. destRect = (-10, 10, 20, 10) will update tiles from x=(WORLD_SIZE - 10) to x=(-10 + 20) and y=(10) to y=(10 + 10)
	*/

	for(unsigned int i = 0; i < m_entities.size(); i++) {
		m_entities[i]->update(m_world, timeStep, i);
		m_entities[i]->collideWithTiles(m_world);
		for(unsigned int j = i + 1; i != (j % m_entities.size()) && m_entities[i]->collideWithOther(m_entities[j % m_entities.size()]) == true; j++);
		for(unsigned int j = i - 1; i != ((j + m_entities.size()) % m_entities.size()) && m_entities[i]->collideWithOther(m_entities[(j + m_entities.size()) % m_entities.size()]) == true; j--);
	}
}

void EntityManager::tickEntities() {
	/**
	    Simply ticks an area of tiles at position destRect.xy, with width and height of destRect.z and destRect.w respectively.
	    Negative coordinates are mapped to accomodate for 'crossover'
	    eg. destRect = (-10, 10, 20, 10) will tick tiles from x=(WORLD_SIZE - 10) to x=(-10 + 20) and y=(10) to y=(10 + 10)
	*/

	//spawnEntities();

	for(unsigned int i = 0; i < m_entities.size(); i++) {
		m_entities[i]->tick(m_world);
	}
}

void EntityManager::spawnEntities() {
	/**
	    Should, for an arbritrary amount,
	    1. Select an x value near the player
	    2. Select a random y and z value that allows for 3*2 space (at least) and a floor
	    3. Select an entity that would fit all the requirements (light level, weather, etc.)
	    4. Spawn it!
	*/

	/// 1.
	std::vector<glm::vec3> positions;

	for(int x = 0; x < 1; x++) {
		positions.push_back(glm::vec3(m_player->getPosition().x + (std::rand() % ENTITY_SPAWN_RANGE), 0.0f, -1.0f));
	}

	/// 2.
	for(int i = 0; i < positions.size(); i++) {
		bool foundPosition = false;
		for(int y = 0; !foundPosition && y < WORLD_HEIGHT - 2; y++) {
			bool checkNext = false;
			for(int z = 0; !checkNext && z < WORLD_DEPTH; z++) {
				unsigned int x = ((int)positions[i].x + WORLD_SIZE) % WORLD_SIZE;
				for(unsigned int y1 = 0; !checkNext && y1 < 2; y1++) {
					for(int x1 = -1; !checkNext && x1 < 2; x1++) {
						if(!m_world->getTile((x + x1 + WORLD_SIZE) % WORLD_SIZE, y + y1, z)) {
							checkNext = true;
						} else if(m_world->getTile((x + x1 + WORLD_SIZE) % WORLD_SIZE, y + y1, z)->isSolid()) {
							checkNext = true;
						}
					}
				}
				for(int x1 = -1; !checkNext && x1 < 2; x1++) {
					if(!m_world->getTile((x + x1 + WORLD_SIZE) % WORLD_SIZE, y - 1, z)) {
						checkNext = true;
					} else if(!m_world->getTile((x + x1 + WORLD_SIZE) % WORLD_SIZE, y - 1, z)->isSolid()) {
						checkNext = true;
					}
				}
				if(!checkNext) {
					positions[i].y = y;
					positions[i].z = z;
					foundPosition = true;
				}
			}
		}
		if(!foundPosition) {
			if(positions.size() > 1) {
				for(int k = i; k < positions.size() - 1; k++) {
					positions[k] = positions[k + 1];
				}
			}
			positions.pop_back();
		}

	}

	for(int i = 0; i < positions.size(); i++) {
		addEntity(createEntity(glm::vec2(positions[i].x, positions[i].y), (int)positions[i].z, EntityIDs::NPC_NEUTRAL_COMPANIONCUBE, SaveDataTypes::MetaData(), true));
	}
}
