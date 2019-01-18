#include "Entity.h"

#include "Categories.h"

#include <DebugRenderer.h>
#include <math.h>

#include "Chunk.h"

unsigned int Entity::getChunkIndex() {
    return m_parentChunk->getIndex();
}

void Entity::setParentChunk(Chunk* chunk) {
    m_parentChunk = chunk;
}

Entity::Entity()
{
    //ctor
}

Entity::Entity(glm::vec2 position, unsigned int id, AudioManager* audioManager) {
    m_audioManager = audioManager;
    init(position, Categories::Entity_Type::MOB, id);
}

Entity::~Entity()
{
    //dtor
}

void Entity::init(glm::vec2 position, Categories::Entity_Type type, unsigned int id) {
    m_position = position;
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
}

void Entity::update(float timeStep, Chunk* worldChunks[WORLD_SIZE]) {
    updateAI();
    updateMovement();
    //setParentChunk(worldChunks);
    m_parentChunk = worldChunks[0];
    updateLightLevel();
    move(timeStep);
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

    sb.draw(destRect, uvRect, m_texture.id, 0.0f, colour, glm::vec3(m_light));
}

void Entity::move(float timeStepVariable) {
    if(m_velocity.x > MAX_SPEED) m_velocity.x = MAX_SPEED;
    if(m_velocity.x < -MAX_SPEED) m_velocity.x = -MAX_SPEED;
    if(m_velocity.y > MAX_SPEED) m_velocity.y = MAX_SPEED;
    if(m_velocity.y < -MAX_SPEED) m_velocity.y = -MAX_SPEED;

    if(!m_onGround) {
        m_velocity.y -= 0.098f;
    } else if(m_velocity.y < 0.0f) {
        m_velocity.y = 0.0f;
    }
    m_position += m_velocity;
    m_onGround = false;
}

void Entity::collide() {

    std::vector<Entity*> entities = m_parentChunk->getEntities();

    if(getChunkIndex() >= 0) {
        /// ENTITY COLLISION STARTS HERE
        for(auto e : entities) {
            if(e && e != this) {
                float xDist = (m_position.x / (float)TILE_SIZE + m_size.x / 2.0f) - (e->getPosition().x / (float)TILE_SIZE + e->getSize().x / 2.0f);
                float yDist = (m_position.y / (float)TILE_SIZE + m_size.y / 2.0f) - (e->getPosition().y / (float)TILE_SIZE + e->getSize().y / 2.0f);
                if(abs(xDist) < abs(m_size.x / 2.0f + e->getSize().x / 2.0f)) {
                    if(abs(yDist) < abs(m_size.y / 2.0f + e->getSize().y / 2.0f)) {

                        float depth = xDist - (m_size.x / 2.0f + e->getSize().x / 2.0f);
                        float force = (depth / 2.0f * TILE_SIZE) * (depth / 2.0f * TILE_SIZE) / ((m_size.x / 2.0f + e->getSize().x / 2.0f) * 512.0f);


                        m_position.x -= force;
                        e->setPosition(glm::vec2(e->getPosition().x + force, e->getPosition().y));
                        //std::cout << "CONTACT " << depth << "\n\n\n";
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

            x += m_velocity.x / m_size.x;
            y += m_velocity.y / m_size.y;

            glm::vec2 posBL(x, y);
            glm::vec2 posBR(x + width, y);
            glm::vec2 posTL(x, y + height);
            glm::vec2 posTR(x + width, y + height);

            const float testVar = 16.0f;

            // Check for ground/ceiling
            checkTilePosition(m_parentChunk->tiles,
                              m_parentChunk->extraTiles,
                              groundTilePositions,
                              posBR.x - TILE_SIZE / testVar,
                              posBR.y);

            checkTilePosition(m_parentChunk->tiles,
                              m_parentChunk->extraTiles,
                              groundTilePositions,
                              posBL.x + TILE_SIZE / testVar,
                              posBL.y);

            checkTilePosition(m_parentChunk->tiles,
                              m_parentChunk->extraTiles,
                              groundTilePositions,
                              posTR.x - TILE_SIZE / testVar,
                              posTR.y);

            checkTilePosition(m_parentChunk->tiles,
                              m_parentChunk->extraTiles,
                              groundTilePositions,
                              posTL.x + TILE_SIZE / testVar,
                              posTL.y);


            // Check the corners
            checkTilePosition(m_parentChunk->tiles,
                              m_parentChunk->extraTiles,
                              collideTilePositions,
                              posBR.x,
                              posBR.y + TILE_SIZE / testVar);

            checkTilePosition(m_parentChunk->tiles,
                              m_parentChunk->extraTiles,
                              collideTilePositions,
                              posBL.x,
                              posBL.y + TILE_SIZE / testVar);

            checkTilePosition(m_parentChunk->tiles,
                              m_parentChunk->extraTiles,
                              collideTilePositions,
                              posTL.x,
                              posTL.y - TILE_SIZE / testVar);

            checkTilePosition(m_parentChunk->tiles,
                              m_parentChunk->extraTiles,
                              collideTilePositions,
                              posTR.x,
                              posTR.y - TILE_SIZE / testVar);

            for (unsigned int i = 0; i < collideTilePositions.size(); i++) {
                collideWithTile(collideTilePositions[i], false);
            }

            for (unsigned int i = 0; i < groundTilePositions.size(); i++) {
                collideWithTile(groundTilePositions[i], true);
            }
        }
        /// TILE COLLISION ENDS HERE
    }
}

/// PRIVATE FUNCTIONS
int Entity::setParentChunk(Chunk* worldChunks[WORLD_SIZE]) {

    short int indexBegin = std::floor(m_position.x / TILE_SIZE / CHUNK_SIZE);
    short int indexEnd = std::floor((m_position.x + m_size.x * TILE_SIZE) / TILE_SIZE / CHUNK_SIZE);
    short int index = getChunkIndex(); // End product

    short int r = -1;

    if(indexBegin != indexEnd) {
        float x = (int)m_position.x / TILE_SIZE % (CHUNK_SIZE);
        if(abs(x - CHUNK_SIZE) > m_size.x / 2.0f * TILE_SIZE) {
            index = indexBegin;
            r = 0;
        } else if(abs(x - TILE_SIZE * CHUNK_SIZE) < m_size.x / 2.0f * TILE_SIZE) {
            index = indexEnd;
            r = 1;
        }
    } else {
        index = indexBegin;
    }

    if(index < 0) {
        m_position.x += WORLD_SIZE * CHUNK_SIZE * TILE_SIZE;
        index = WORLD_SIZE-1;
        r = 0;
    }
    if(index >= WORLD_SIZE) {
        m_position.x -= WORLD_SIZE * CHUNK_SIZE * TILE_SIZE;
        index = 0;
        r = 1;
    }

    if(index >= 0 && index < WORLD_SIZE) {
        m_parentChunk = worldChunks[index];
    }

    return r;

    /*if(index != m_parentChunkIndex) {
        if(index < 0) {
            m_position.x = (int)(m_position.x + WORLD_SIZE * CHUNK_SIZE * TILE_SIZE) % (WORLD_SIZE * CHUNK_SIZE * TILE_SIZE);
            index = std::floor((m_position.x + m_size.x * TILE_SIZE) / TILE_SIZE / CHUNK_SIZE);
            if((int)(m_position.x + m_size.x * TILE_SIZE) == 0) index = 0; // Just to avoid 0 / whatever. It just creates issues :(
        } else if(index >= WORLD_SIZE) {
            m_position.x = (int)(m_position.x + WORLD_SIZE * CHUNK_SIZE * TILE_SIZE) % (WORLD_SIZE * CHUNK_SIZE * TILE_SIZE);
            index = std::floor((m_position.x) / TILE_SIZE / CHUNK_SIZE);
        }
        if(index >= 0 && index < WORLD_SIZE) {
            m_parentChunk = worldChunks[index];
            m_parentChunkIndex = index;
        }
    }*/
}

bool Entity::checkTilePosition(Tile* tiles[WORLD_HEIGHT][CHUNK_SIZE], Tile* extraTileArray[WORLD_HEIGHT][2], std::vector<glm::vec2>& collideTilePositions, float x, float y) {
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
        } else if((int)gridPos.x - (getChunkIndex() * CHUNK_SIZE) == -1) {
            if (extraTileArray[(int)gridPos.y][0]->isSolid()) { //  - getChunkIndex()
                collideTilePositions.push_back(glm::vec2((float)gridPos.x + 0.500f, (float)gridPos.y + 0.500f)); // CollideTilePositions are put in as gridspace coords
                return true;
            }
        } else if((int)gridPos.x - (getChunkIndex() * CHUNK_SIZE) == CHUNK_SIZE) {
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

#include <iostream>

void Entity::updateLightLevel() {
    if(m_parentChunk) {
        int entityChunkX, entityChunkY; // The entity's coords in the chunk

        entityChunkX = (int)((m_position.x) / TILE_SIZE) % CHUNK_SIZE;
        entityChunkY = (int)((m_position.y) / TILE_SIZE) + m_size.y / 2.0f;

        if(entityChunkX >= 0 && entityChunkX < CHUNK_SIZE) {
            if(entityChunkY >= 0 && entityChunkY < WORLD_HEIGHT) {
                m_light = m_parentChunk->tiles[entityChunkY][entityChunkX]->getLight();
            }
        }

        if(entityChunkX+m_size.x >= 0 && entityChunkX+m_size.x < CHUNK_SIZE) {
            if(entityChunkY >= 0 && entityChunkY < WORLD_HEIGHT) {
                m_light += m_parentChunk->tiles[entityChunkY][(int)(entityChunkX+m_size.x)]->getLight();
                m_light /= 2.0f;
            }
        }
    } else {
        m_light = 0.0f;
    }
}

void Entity::updateAI() {
    if(m_ai == Categories::AI_Type::WALKING) {
        EntityFunctions::WalkingAI(m_controls, m_targets, m_curTarget, m_velocity, m_size, m_position);
    }
}

void Entity::updateMovement() {
    if(m_controls[0]) { // UP
        if(m_onGround) {
            m_velocity.y = m_jumpHeight; // y=(jumpHeight+-0.098*60*s^2)  initial jump power is the absolute of the x at 0. jumpheight is in eights of tiles and you must add 4
            m_onGround = false;
        }
    }
    if(m_controls[1]) { // DOWN
        /// TODO: implement crouching
    }
    if(m_controls[2]) { // LEFT
        m_velocity.x -= m_speed;
    } else if(m_controls[3]) { // RIGHT
        m_velocity.x += m_speed;
    } else {
        m_velocity.x /= 5.0f;//= 5.0f;
    }

    if(m_velocity.x > MAX_SPEED) {
        m_velocity.x = MAX_SPEED;
    } else if(m_velocity.x < -MAX_SPEED) {
        m_velocity.x = -MAX_SPEED;
    }
}
