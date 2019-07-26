#include "Entity.h"

#include "Categories.h"

#include <DebugRenderer.h>
#include <math.h>

#include "Chunk.h"
#include "Player.h"

void Limb::draw(GLEngine::SpriteBatch& sb) {
    glm::vec2 position = m_parentEntity->getPosition() + (m_pos * (m_parentEntity->getSize()) * glm::vec2(TILE_SIZE));

    glm::vec4 destRect(position.x, position.y, m_size.x * TILE_SIZE, m_size.y * TILE_SIZE);
    glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

    GLEngine::ColourRGBA8 fullColour(255, 255, 255, 255);

    sb.draw(destRect, uvRect, m_textureId, 0.51f, fullColour, m_angle, glm::vec3(m_parentEntity->getLightLevel()), m_por);
}

unsigned int Entity::getChunkIndex() {
    if(!m_parentChunk) {
        return -10;
    }
    return m_parentChunk ? m_parentChunk->getIndex() : 0;
}

void Entity::setParentChunk(Chunk* chunk) {
    m_parentChunk = chunk;
}

Entity::~Entity()
{
    //dtor
}

/*void Entity::init(glm::vec2 position, Categories::Entity_Type type, unsigned int id, ScriptQueue* sq) {
    m_position = position;
    m_sq = sq;

    switch(type) {
        case Categories::Entity_Type::MOB: /// Temporary, change to abstract class
        {
            if(Category_Data::mobData[id].id == id) {
                m_texture = GLEngine::ResourceManager::getTexture(Category_Data::mobData[id].texturePath);
                m_size = Category_Data::mobData[id].size;
                m_faction = Category_Data::mobData[id].faction;
                m_jumpHeight = Category_Data::mobData[id].jumpHeight;
                m_speed = Category_Data::mobData[id].speed;
                m_ai = Category_Data::mobData[id].aitype;
                m_disabilities = Category_Data::mobData[id].disabilityType;
                m_attackType = Category_Data::mobData[id].attackType;
            } else {
                return;
            }
        }
        case Categories::Entity_Type::DIALOGUE:
            if(Category_Data::dialogueMobData[id].id == id) {

            } else {
                return;
            }
            if(Category_Data::mobData[id].id == id) {
                m_texture = GLEngine::ResourceManager::getTexture(Category_Data::mobData[id].texturePath);
                m_size = Category_Data::mobData[id].size;
                m_faction = Category_Data::mobData[id].faction;
                m_jumpHeight = Category_Data::mobData[id].jumpHeight;
                m_speed = Category_Data::mobData[id].speed;
                m_ai = Category_Data::mobData[id].aitype;
                m_disabilities = Category_Data::mobData[id].disabilityType;
                m_attackType = Category_Data::mobData[id].attackType;
            } else {
                return;
            }
    }
}*/

void Entity::update(float timeStep, Chunk* worldChunks[WORLD_SIZE]) {
    updateAI();
    updateLimbs();
    move(timeStep);
    updateSounds();
    updateMovement();
    updateLightLevel();
}
#include <iostream>
void Entity::tick(Player* p) {
    if(m_makesNoise) {
        if(std::rand() % 100 <= m_noiseFrequency/* && m_currentNoise == -1 Must create some way to check if the sound is finished sounding (TODO: )*/) {
            unsigned int dist;
            int crossDist = p->getChunkIndex() > getChunkIndex() ? (getChunkIndex() + WORLD_SIZE - p->getChunkIndex()) : (p->getChunkIndex() + WORLD_SIZE - getChunkIndex()); // This is the combined chunk distances to the end of the world, or the 'crossover'
            int pureDist = std::abs((int)(p->getChunkIndex() - getChunkIndex())); // This is the 'normal' way of calculating distance through the world, doesn't account for crossover

            int playerPlace = 0;

            if(crossDist < pureDist) playerPlace += crossDist * CHUNK_SIZE;
            if(crossDist >= pureDist) playerPlace += pureDist * CHUNK_SIZE;

            glm::vec2 d = glm::vec2((int)p->getPosition().x % CHUNK_SIZE + playerPlace, p->getPosition().y) - glm::vec2((int)m_position.x % CHUNK_SIZE, m_position.y);
            d.x = std::abs(d.x);
            d.y = std::abs(d.y);
            dist = (std::sqrt(d.x * d.x + d.y * d.y)); //  Good 'ol pythagoreas

            unsigned int vol = ((dist)*(-MIX_MAX_VOLUME)/(MAX_DIST_HEARD) + MIX_MAX_VOLUME); // Mapping 0-max_dist_heard -> mix_max_volume-0

            if(dist >= MAX_DIST_HEARD) {
                vol = 0;
            }

            m_currentNoise = std::rand() % m_ambientNoiseSound.size();
            m_audioManager->playSoundEffect((unsigned int)m_ambientNoiseSound[m_currentNoise], vol);
        }
        if(m_currentNoise != -1) {
            unsigned int dist;
            int crossDist = p->getChunkIndex() > getChunkIndex() ? (getChunkIndex() + WORLD_SIZE - p->getChunkIndex()) : (p->getChunkIndex() + WORLD_SIZE - getChunkIndex()); // This is the combined chunk distances to the end of the world, or the 'crossover'
            int pureDist = std::abs((int)(p->getChunkIndex() - getChunkIndex())); // This is the 'normal' way of calculating distance through the world, doesn't account for crossover

            int playerPlace = 0;

            if(crossDist < pureDist) playerPlace += crossDist * CHUNK_SIZE;
            if(crossDist >= pureDist) playerPlace += pureDist * CHUNK_SIZE;

            glm::vec2 d = glm::vec2((int)p->getPosition().x % CHUNK_SIZE + playerPlace, p->getPosition().y) - glm::vec2((int)m_position.x % CHUNK_SIZE, m_position.y);
            d.x = std::abs(d.x);
            d.y = std::abs(d.y);
            dist = (std::sqrt(d.x * d.x + d.y * d.y)); //  Good 'ol pythagoreas

            unsigned int vol = ((dist)*(-MIX_MAX_VOLUME)/(MAX_DIST_HEARD) + MIX_MAX_VOLUME); // Mapping 0-max_dist_heard -> mix_max_volume-0

            if(dist >= MAX_DIST_HEARD) {
                vol = 0;
            }

            m_audioManager->updateSoundEffect((unsigned int)m_ambientNoiseSound[m_currentNoise], vol);
        }
    }
}

void Entity::draw(GLEngine::SpriteBatch& sb, float time, float xOffset) {

    //GLint lightUniform = program->getUniformLocation("lightColour");
    //glUniform3fv(lightUniform, 3, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);

    glm::vec4 destRect = glm::vec4(m_position.x + (xOffset * CHUNK_SIZE * TILE_SIZE), m_position.y, m_size.x * TILE_SIZE, m_size.y * TILE_SIZE);

    float x, y;
    if(m_velocity.x > m_speed) {
        x = (int)time*abs((int)m_velocity.x)+1 % 3;
        y = 1;
        m_flippedTexture = false;
    } else if(m_velocity.x < -m_speed) {
        x = (int)time*abs((int)m_velocity.x)+1 % 3;
        y = 1;
        m_flippedTexture = true;
    } else {
        x = 0;
        y = 1;
    }
    if(m_velocity.y > 0.0f) {
        x = (int)time % 3;
        y = 0;
    }

    float finalX = (x / ((float)m_texture.width / (m_size.x * 32)));
    float finalY = (y / ((float)m_texture.height / (m_size.y * 32)));

    glm::vec4 uvRect;

    if(!m_flippedTexture) {
        uvRect = glm::vec4(finalX,
                           finalY,
                           1.0f / ((float)m_texture.width / (m_size.x * 32)),
                           1.0f / ((float)m_texture.height / (m_size.y * 32)));
    } else if(m_flippedTexture) {
        uvRect = glm::vec4(finalX + 1.0f / ((float)m_texture.width / (m_size.x * 32)),
                           finalY,
                           1.0f / -((float)m_texture.width / (m_size.x * 32)),
                           1.0f / ((float)m_texture.height / (m_size.y * 32)));
    }

    GLEngine::ColourRGBA8 colour(255, 255, 255, 255);

    sb.draw(destRect, uvRect, m_texture.id, 0.5f, colour, glm::vec3(m_light));

    for(unsigned int i = 0; i < m_limbs.size(); i++) {
        m_limbs[i]->draw(sb); // no sb.begin() or end()
    }
}

void Entity::move(float timeStepVariable) {
    if(m_velocity.x > MAX_SPEED) m_velocity.x = MAX_SPEED;
    if(m_velocity.x < -MAX_SPEED) m_velocity.x = -MAX_SPEED;
    if(m_velocity.y > MAX_SPEED) m_velocity.y = MAX_SPEED;
    if(m_velocity.y < -MAX_SPEED) m_velocity.y = -MAX_SPEED;

    m_velocity.y -= 1.225f / 60.0f; // Earth gravity is far too harsh for games. We use about 1/8th
    m_position += m_velocity;
}

#define getEntity(i) (&(*entities)[i])

void Entity::collide() {

    std::vector<Entity>* entities = m_parentChunk->getEntities();

    if(getChunkIndex() >= 0) {
        /// ENTITY COLLISION STARTS HERE
        for(unsigned int i = 0; i < entities->size(); i++) {
            if(getEntity(i)->getPosition() != m_position) {
                float xDist = (m_position.x / (float)TILE_SIZE + m_size.x / 2.0f) - (getEntity(i)->getPosition().x / (float)TILE_SIZE + getEntity(i)->getSize().x / 2.0f);
                float yDist = (m_position.y / (float)TILE_SIZE + m_size.y / 2.0f) - (getEntity(i)->getPosition().y / (float)TILE_SIZE + getEntity(i)->getSize().y / 2.0f);
                if(abs(xDist) < abs(m_size.x / 2.0f + getEntity(i)->getSize().x / 2.0f)) {
                    if(abs(yDist) < abs(m_size.y / 2.0f + getEntity(i)->getSize().y / 2.0f)) {

                        float depth = xDist - (m_size.x / 2.0f + getEntity(i)->getSize().x / 2.0f);
                        float force = (depth / 2.0f * TILE_SIZE) * (depth / 2.0f * TILE_SIZE) / ((m_size.x / 2.0f + getEntity(i)->getSize().x / 2.0f) * 512.0f);


                        m_position.x -= force;
                        getEntity(i)->setPosition(glm::vec2(getEntity(i)->getPosition().x + force, getEntity(i)->getPosition().y));
                    }
                }
            }
        }
        /// ENTITY COLLISION ENDS HERE

        /// TILE COLLISION STARTS HERE
        {
            /// Many thanks to Ben Arnold. He taught me almost everything I know about programming through his Youtube channel, "Makinggameswithben"
            /// This is just a small piece of code that handles and reacts to dynamic rectangle and tile collisions
            std::vector<glm::vec2> collideTilePositions;
            std::vector<glm::vec2> groundTilePositions;

            float x = m_position.x, y = m_position.y, width = m_size.x * TILE_SIZE, height = m_size.y * TILE_SIZE;

            glm::vec2 posBL(x, y);
            glm::vec2 posBR(x + width, y);
            glm::vec2 posTL(x, y + height);
            glm::vec2 posTR(x + width, y + height);

            const float testVar = 1.0f/16.0f;

            // Check for ground/ceiling
            checkTilePosition(m_parentChunk->tiles,
                              m_parentChunk->extraTiles,
                              groundTilePositions,
                              posBR.x - testVar,
                              posBR.y);

            checkTilePosition(m_parentChunk->tiles,
                              m_parentChunk->extraTiles,
                              groundTilePositions,
                              posBL.x + testVar,
                              posBL.y);

            checkTilePosition(m_parentChunk->tiles,
                              m_parentChunk->extraTiles,
                              groundTilePositions,
                              posTR.x - testVar,
                              posTR.y);

            checkTilePosition(m_parentChunk->tiles,
                              m_parentChunk->extraTiles,
                              groundTilePositions,
                              posTL.x + testVar,
                              posTL.y);


            // Check the corners
            checkTilePosition(m_parentChunk->tiles,
                              m_parentChunk->extraTiles,
                              collideTilePositions,
                              posBR.x,
                              posBR.y + testVar);

            checkTilePosition(m_parentChunk->tiles,
                              m_parentChunk->extraTiles,
                              collideTilePositions,
                              posBL.x,
                              posBL.y + testVar);

            checkTilePosition(m_parentChunk->tiles,
                              m_parentChunk->extraTiles,
                              collideTilePositions,
                              posTL.x,
                              posTL.y - testVar);

            checkTilePosition(m_parentChunk->tiles,
                              m_parentChunk->extraTiles,
                              collideTilePositions,
                              posTR.x,
                              posTR.y - testVar);

            /// Collision prediction time!

            float increment = 0.1f;
            int signX = (m_velocity.x > 0.0f) ? 1 : -1;
            int signY = (m_velocity.y > 0.0f) ? 1 : -1;

            std::vector<glm::vec2> predictiveTiles;

            for(int i = 0; i < std::abs(m_velocity.x) / (increment * width) + increment; i++) {
                for(int j = 0; j < std::abs(m_velocity.y) / (increment * height) + increment; j++) {
                    glm::vec2 p_posBL(x+(i*increment*width)*signX, y+(j*increment*height)*signY);
                    glm::vec2 p_posBR(x+(i*increment*width)*signX + width, y+(j*increment*height)*signY);
                    glm::vec2 p_posTL(x+(i*increment*width)*signX, y+(j*increment*height)*signY + height);
                    glm::vec2 p_posTR(x+(i*increment*width)*signX + width, y+(j*increment*height)*signY + height);

                    checkTilePosition(m_parentChunk->tiles,
                                      m_parentChunk->extraTiles,
                                      predictiveTiles,
                                      p_posBR.x,
                                      p_posBR.y);

                    checkTilePosition(m_parentChunk->tiles,
                                      m_parentChunk->extraTiles,
                                      predictiveTiles,
                                      p_posBL.x,
                                      p_posBL.y);

                    checkTilePosition(m_parentChunk->tiles,
                                      m_parentChunk->extraTiles,
                                      predictiveTiles,
                                      p_posTL.x,
                                      p_posTL.y);

                    checkTilePosition(m_parentChunk->tiles,
                                      m_parentChunk->extraTiles,
                                      predictiveTiles,
                                      p_posTR.x,
                                      p_posTR.y);
                    }
            }

            for (unsigned int i = 0; i < collideTilePositions.size(); i++) {
                collideWithTile(collideTilePositions[i], false);
            }

            if(predictiveTiles.size() > 0) collideWithTile(predictiveTiles[0], false);

            for (unsigned int i = 0; i < groundTilePositions.size(); i++) {
                collideWithTile(groundTilePositions[i], true);
            }
        }
        /// TILE COLLISION ENDS HERE
    }
}

/// PRIVATE FUNCTIONS
int Entity::setParentChunk(Chunk* worldChunks[WORLD_SIZE]) {

    /*short int indexBegin = (int)(std::floor((m_position.x) / (float)CHUNK_SIZE) + WORLD_SIZE) % WORLD_SIZE;
    short int indexEnd = (int)(std::floor((m_position.x + m_size.x) / (float)CHUNK_SIZE) + WORLD_SIZE) % WORLD_SIZE;
    short int index = 0;

    short int r = -1;

    if(indexBegin != indexEnd) {
        if(m_velocity.x > 0.0f) { // Entity is heading towards right chunk
            index = indexEnd;
            r = 1;
        } else { // Entity is heading towards left chunk
            index = indexBegin;
            r = 0;
        }
    } else {
        index = indexBegin;
    }

    if(m_velocity.x > 0.0f && m_position.x > WORLD_SIZE * CHUNK_SIZE) {
        m_position.x = std::abs(m_position.x - WORLD_SIZE * CHUNK_SIZE);
    } else if(m_velocity.x < 0.0f && m_position.x + m_size.x < 0.0f) {
        m_position.x = (m_position.x + WORLD_SIZE * CHUNK_SIZE);
    }

    m_parentChunk = worldChunks[index];

    return r;*/

    int index = std::floor((m_position.x + m_size.x / 2.0f) / CHUNK_SIZE);

    /*int posBase = ((int)(m_position.x) + WORLD_SIZE*CHUNK_SIZE) % (WORLD_SIZE*CHUNK_SIZE);
    float extra = m_position.x - (float)(posBase);

    m_position.x = (float)(posBase) + extra;*/

    int ret = 0;

    if(index != m_parentChunk->getIndex()) {
        if(index < 0) {
            m_position.x += (WORLD_SIZE * CHUNK_SIZE);
            index = WORLD_SIZE - 1;
            ret = -1;
        } else if(index >= WORLD_SIZE) {
            m_position.x -= (WORLD_SIZE * CHUNK_SIZE);
            index = 0;
            ret = 1;
        }
        if(index >= 0 && index < WORLD_SIZE) {
            if(index > m_parentChunk->getIndex()) {
                ret = 1;
            } else {
                ret = -1;
            }
            m_parentChunk = worldChunks[index];
        }
    }

    return ret;
}

bool Entity::checkTilePosition(Tile*** tiles, Tile*** extraTileArray, std::vector<glm::vec2>& collideTilePositions, float x, float y) {
    // Get the position of this corner in grid-space
    glm::vec2 gridPos = glm::vec2(floor(x / TILE_SIZE),
                                      floor(y / TILE_SIZE)); // grid-space coords


    // If we are outside the world, just return
    /*if (gridPos.x < CHUNK_SIZE * m_parentChunkIndex || gridPos.x >= CHUNK_SIZE + CHUNK_SIZE * m_parentChunkIndex ||
        gridPos.y < 0 || gridPos.y >= WORLD_HEIGHT) {
        return false;
    }*/

    // If this is not an air tile, we should collide with it
    if((int)gridPos.y >= 0 && (int)gridPos.y < WORLD_HEIGHT) {
        if ((int)gridPos.x - (getChunkIndex() * CHUNK_SIZE) >= 0 && (int)gridPos.x - (getChunkIndex() * CHUNK_SIZE) < CHUNK_SIZE) {
            // returning gridpos.x as NAN
            if (tiles[(int)gridPos.y][(int)gridPos.x - (getChunkIndex() * CHUNK_SIZE)]->isSolid()) { //  - getChunkIndex()
                collideTilePositions.push_back(glm::vec2((float)gridPos.x + 0.500f, (float)gridPos.y + 0.500f)); // CollideTilePositions are put in as gridspace coords
                return true;
            }
        } else if((int)gridPos.x - ((signed int)getChunkIndex() * CHUNK_SIZE) <= -1) {
            if (extraTileArray[(int)gridPos.y][0]->isSolid()) { //  - getChunkIndex()
                collideTilePositions.push_back(glm::vec2((float)gridPos.x + 0.500f, (float)gridPos.y + 0.500f)); // CollideTilePositions are put in as gridspace coords
                return true;
            }
        } else if((int)gridPos.x - (getChunkIndex() * CHUNK_SIZE) >= CHUNK_SIZE) {
            if (extraTileArray[(int)gridPos.y][1]->isSolid()) { //  - getChunkIndex()
                collideTilePositions.push_back(glm::vec2((float)gridPos.x + 0.500f, (float)gridPos.y + 0.500f)); // CollideTilePositions are put in as gridspace coords
                return true;
            }
        }
    }

    return false;
}

void Entity::collideWithTile(glm::vec2 tilePos, bool ground) {
    float x = m_position.x, y = m_position.y;

    glm::vec2 entRelativeCentrePosition; //Relative position of the centre of entity (to bottom left corner)
    entRelativeCentrePosition = glm::vec2(m_size.x / 2.0f * (float)TILE_SIZE, m_size.y / 2.0f * (float)TILE_SIZE);

    glm::vec2 entCentrePosition; //Position of the centre of entity in pixel coords
    // m_size is in grid coords so multiply it by TILE_SIZE
    entCentrePosition = glm::vec2(x + entRelativeCentrePosition.x, y + entRelativeCentrePosition.y);

    glm::vec2 tileRelativeCentrePosition; //Relative position of the centre of tile (to bottom left corner)
    tileRelativeCentrePosition = glm::vec2((float)TILE_SIZE * 0.5f);

    glm::vec2 tileCentrePosition; // Position of the centre of tile in pixel coords
    // tilePos is in grid coords, so mutiply it by TILE_SIZE
    // Don't add relativeCentrePosition because "tilePos" is already centered
    tileCentrePosition = glm::vec2(tilePos * glm::vec2((float)TILE_SIZE));

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

void Entity::updateLightLevel() {
    if(m_parentChunk) {
        int entityChunkX, entityChunkY; // The entity's coords in the chunk

        entityChunkX = (int)((m_position.x) / TILE_SIZE) % CHUNK_SIZE;
        entityChunkY = (int)((m_position.y) / TILE_SIZE) + m_size.y / 2.0f;

        m_exposedToSun = false;

        if(entityChunkX >= 0 && entityChunkX < CHUNK_SIZE) {
            if(entityChunkY >= 0 && entityChunkY < WORLD_HEIGHT) {
                m_light = m_parentChunk->tiles[entityChunkY][entityChunkX]->getLight();
                if(m_parentChunk->tiles[entityChunkY][entityChunkX]->getSunLight() != 0.0f) m_exposedToSun = true;
            }
        }

        if(entityChunkX+m_size.x >= 0 && entityChunkX+m_size.x < CHUNK_SIZE) {
            if(entityChunkY >= 0 && entityChunkY < WORLD_HEIGHT) {
                m_light += m_parentChunk->tiles[entityChunkY][(int)(entityChunkX+m_size.x)]->getLight();
                if(m_parentChunk->tiles[entityChunkY][(int)(entityChunkX+m_size.x)]->getSunLight() != 0.0f) m_exposedToSun = true;
                m_light /= 2.0f;
            }
        }
    } else {
        m_light = 0.0f;
    }
}

void Entity::updateMovement() {
    if(m_controls[0]) { // UP
        if(m_onGround) {
            m_velocity.y = m_jumpHeight;
            m_onGround = false;
        }
    }
    if(m_controls[1]) { // DOWN
        /// TODO: implement crouching
    }
    if(m_controls[2]) { // LEFT
        if(m_velocity.x > -m_maxSpeed && m_onGround)
            m_velocity.x -= m_speed;
    } else if(m_controls[3]) { // RIGHT
        if(m_velocity.x < m_maxSpeed && m_onGround)
            m_velocity.x += m_speed;
    } else {
        m_velocity.x /= 5.0f;
    }

    if(m_velocity.x > MAX_SPEED) {
        m_velocity.x = MAX_SPEED;
    } else if(m_velocity.x < -MAX_SPEED) {
        m_velocity.x = -MAX_SPEED;
    }
}

void Entity::updateSounds() {
    if(m_onGround && std::abs(m_velocity.x) > 0.01f) {
        m_soundTimer += std::abs(m_velocity.x * 3) + 1;
    } else if(std::abs(m_velocity.x) <= 0.01f || !m_onGround) {
        m_soundTimer = 0;
    }
    if(m_onGround && m_soundTimer > 10) { // 10 is arbitrary, should probably add a variable to each type of entity (for longer strides, etc.)
        glm::vec2 tileCoordsFloor = glm::vec2((int)(m_position.x / TILE_SIZE + 0.5f), (int)(m_position.y / TILE_SIZE - 0.5f));

        unsigned int chunkIndex = tileCoordsFloor.x / CHUNK_SIZE;

        tileCoordsFloor.x -= chunkIndex * CHUNK_SIZE;

        if(tileCoordsFloor.x >= CHUNK_SIZE) {
            m_audioManager->playSoundEffect(m_parentChunk->extraTiles[(int)tileCoordsFloor.y][1]->getWalkedOnSoundEffectID(), MIX_MAX_VOLUME);
        } else if(tileCoordsFloor.x < 0) {
            m_audioManager->playSoundEffect(m_parentChunk->extraTiles[(int)tileCoordsFloor.y][0]->getWalkedOnSoundEffectID(), MIX_MAX_VOLUME);
        } else {
            m_audioManager->playSoundEffect(m_parentChunk->tiles[(int)tileCoordsFloor.y][(int)tileCoordsFloor.x]->getWalkedOnSoundEffectID(), MIX_MAX_VOLUME);
        }
        m_soundTimer = 0;
    }
}
