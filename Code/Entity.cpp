#include "Entity.h"

#include "Categories.h"

#include <DebugRenderer.h>
#include <math.h>

#include "Player.h"

#include "Entities.h"

void Limb::draw(GLEngine::SpriteBatch& sb) {
    glm::vec2 position = m_parentEntity->getPosition() + (m_pos * (m_parentEntity->getSize()));

    glm::vec4 destRect(position.x, position.y, m_size.x, m_size.y);
    glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

    GLEngine::ColourRGBA8 fullColour(255, 255, 255, 255);

    sb.draw(destRect, uvRect, m_textureId, 0, 0.51f, fullColour, m_angle, glm::vec4(m_parentEntity->getLightLevel()), m_por);
}

Entity::~Entity()
{

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

void Entity::update(World* world, AudioManager* audio, float timeStep, ScriptQueue* sq) {
    //for(int i = 0; i < timeStep; i++) {
        updateAI();
        updateLimbs();
        move(1); /// Fix timestepping, make sure that each step, the entity collides :facepalm:
        updateSounds(world, audio);
        interact(world, sq);
        updateMovement(world);
        updateLightLevel(world);

        if(m_health <= 0.0f) {
            die(world);
        }
    //}
}
#include <iostream>
void Entity::tick(World* world, AudioManager* audio) {
    if(m_makesNoise) {
        if(std::rand() % 100 <= m_noiseFrequency/* && m_currentNoise == -1 Must create some way to check if the sound is finished sounding (TODO: <-- This)*/) {
            Player* p = world->getPlayer();

            unsigned int dist;
            unsigned int crossDist;
            unsigned int pureDist;

            if(p->getPosition().x < m_position.x) { // Crossover is on right
                crossDist = (WORLD_SIZE - m_position.x) + (p->getPosition().x);
            } else { // Crossover on left
                crossDist = (m_position.x) + (WORLD_SIZE - p->getPosition().x);
            }

            pureDist = std::abs(m_position.x - p->getPosition().x);

            unsigned int xDist = crossDist > pureDist ? pureDist : crossDist;
            unsigned int yDist = std::abs(p->getPosition().y - m_position.y);

            dist = (std::sqrt(xDist * xDist + yDist * yDist)); //  Good 'ol pythagoras

            unsigned int vol = ((dist)*(-MIX_MAX_VOLUME)/(MAX_DIST_HEARD) + MIX_MAX_VOLUME); // Mapping 0-max_dist_heard -> mix_max_volume-0

            if(dist >= MAX_DIST_HEARD) {
                vol = 0;
            }

            m_currentNoise = std::rand() % m_ambientNoiseSound.size();
            audio->playSoundEffect((unsigned int)m_ambientNoiseSound[m_currentNoise], vol);
        }
        if(m_currentNoise != -1) {
            Player* p = world->getPlayer();

            unsigned int dist;
            unsigned int crossDist;
            unsigned int pureDist;

            if(p->getPosition().x < m_position.x) { // Crossover is on right
                crossDist = (WORLD_SIZE - m_position.x) + (p->getPosition().x);
            } else { // Crossover on left
                crossDist = (m_position.x) + (WORLD_SIZE - p->getPosition().x);
            }

            pureDist = std::abs(m_position.x - p->getPosition().x);

            unsigned int xDist = crossDist > pureDist ? pureDist : crossDist;
            unsigned int yDist = std::abs(p->getPosition().y - m_position.y);

            dist = (std::sqrt(xDist * xDist + yDist * yDist)); //  Good 'ol pythagoras

            unsigned int vol = ((dist)*(-MIX_MAX_VOLUME)/(MAX_DIST_HEARD) + MIX_MAX_VOLUME); // Mapping 0-max_dist_heard -> mix_max_volume-0

            if(dist >= MAX_DIST_HEARD) {
                vol = 0;
            }

            audio->updateSoundEffect((unsigned int)m_ambientNoiseSound[m_currentNoise], vol);
        }
    }
}

void Entity::draw(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset) {

    //GLint lightUniform = program->getUniformLocation("lightColour");
    //glUniform3fv(lightUniform, 3, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);

    glm::vec4 destRect = glm::vec4(m_position.x + (xOffset * CHUNK_SIZE), m_position.y, m_size.x, m_size.y);

    float x, y;
    if(m_velocity.x > m_speed) {
        x = (int)time*abs((int)m_velocity.x)+1 % m_animation_runFrames;
        y = 1;
        m_flippedTexture = false;
    } else if(m_velocity.x < -m_speed) {
        x = (int)time*abs((int)m_velocity.x)+1 % m_animation_runFrames;
        y = 1;
        m_flippedTexture = true;
    } else {
        x = 0;
        y = 1;
    }
    if(m_velocity.y > 0.0f) {
        x = (int)time % m_animation_jumpFrames;
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
    if(layerDifference != 0) {
        if(layerDifference > 0) {// in front of the player
            colour.a = 150;
            colour.r = 64;
            colour.g = 64;
            colour.b = 64;
        } else {
            float c = 100 / -layerDifference; // Same layer will be (255, 255, 255), 1 layer back (64, 64, 64), 2 layers (32, 32, 32), etc.
            colour.r = c;
            colour.g = c;
            colour.b = c;
        }
    }

    float depth = 0.5f * (WORLD_DEPTH - m_layer);
    if(m_type == Categories::Entity_Type::ITEM) {
        depth = (WORLD_DEPTH - m_layer);
    }

    sb.draw(destRect, uvRect, m_texture.id, m_bumpMap.id, depth, colour, m_cornerLight);

    for(unsigned int i = 0; i < m_limbs.size(); i++) {
        m_limbs[i]->draw(sb); // no sb.begin() or end()
    }
}

void Entity::debugDraw(GLEngine::DebugRenderer& dr, float xOffset) {
    for(unsigned int i = 0; i < m_targets.size(); i++) {
        GLEngine::ColourRGBA8 c(255, 255, 255, 255);
        if(i == m_curTarget) c.r = 0;
        dr.drawCircle(glm::vec2(m_targets[i].x + 0.5f, m_targets[i].y + 0.5f), c, 0.2f);
    }
}

void Entity::move(float timeStepVariable) {
    if(m_velocity.x > MAX_SPEED) m_velocity.x = MAX_SPEED;
    if(m_velocity.x < -MAX_SPEED) m_velocity.x = -MAX_SPEED;
    if(m_velocity.y > MAX_SPEED) m_velocity.y = MAX_SPEED;
    if(m_velocity.y < -MAX_SPEED) m_velocity.y = -MAX_SPEED;

    m_position += m_velocity * timeStepVariable;

    if(m_takesFallDamage) {
        if(m_velocity.y < 0.0f) {
            m_fallenDistance += -m_velocity.y * timeStepVariable;
        } else {
            if(m_fallenDistance > 4.0f) {
                m_health -= std::pow(m_fallenDistance - 4.0f, 1.5f) * 0.08f;
            }
            m_fallenDistance = 0.0f;
        }
    }

    if((int)m_position.x > WORLD_SIZE) {
        m_position.x -= WORLD_SIZE;
    } else if((int)m_position.x < 0) {
        m_position.x += WORLD_SIZE;
    }

    m_velocity.y -= 1.225f / 60.0f * timeStepVariable; // Earth gravity is far too harsh for games. We use about 1/8th
}

void Entity::collide(World* world, unsigned int entityIndex) {
    /*
        World's entities are in a vector, sorted from closest to farthest from x=0. When colliding with other entities, entities can use their own index as a sort of reference point to who they should collide with.
    */


    std::vector<Entity*> entities = world->getEntities();

    /// ENTITY COLLISION STARTS HERE
    for(unsigned int i = entityIndex+1; i < entities.size(); i++) { /// To the right
        if(entities[i]->getType() != Categories::Entity_Type::ITEM) {
            float xDist = (m_position.x + m_size.x / 2.0f) - (entities[i]->getPosition().x + entities[i]->getSize().x / 2.0f);
            float yDist = (m_position.y + m_size.y / 2.0f) - (entities[i]->getPosition().y + entities[i]->getSize().y / 2.0f);
            if(abs(xDist) < abs(m_size.x / 2.0f + entities[i]->getSize().x / 2.0f)) {
                if(abs(yDist) < abs(m_size.y / 2.0f + entities[i]->getSize().y / 2.0f)) {

                    float depth = xDist - (m_size.x / 2.0f + entities[i]->getSize().x / 2.0f);
                    float force = (depth / 2.0f) * (depth / 2.0f) / ((m_size.x / 2.0f + entities[i]->getSize().x / 2.0f) * 512.0f);


                    m_position.x -= force;
                    entities[i]->setPosition(glm::vec2(entities[i]->getPosition().x + force, entities[i]->getPosition().y));
                    continue;
                }
            }
        } else if(entities[i]->getType() == Categories::Entity_Type::ITEM) {
            float xDist = std::abs(entities[i]->getPosition().x - m_position.x);
            float yDist = std::abs(entities[i]->getPosition().y - m_position.y);
            float dist = std::sqrt(xDist * xDist + yDist * yDist);

            if(dist <= 3.0f) {
                m_inventory->addItem((static_cast<EntityNeutralItem*>(entities[i])->getItem()));
                world->removeEntity(i);
            }
            continue;
        }
        break;
    }

    for(int i = entityIndex-1; i >= 0; i--) { /// To the left
        if(entities[i]->getType() != Categories::Entity_Type::ITEM) {
            float xDist = (m_position.x + m_size.x / 2.0f) - (entities[i]->getPosition().x + entities[i]->getSize().x / 2.0f);
            float yDist = (m_position.y + m_size.y / 2.0f) - (entities[i]->getPosition().y + entities[i]->getSize().y / 2.0f);
            if(abs(xDist) < abs(m_size.x / 2.0f + entities[i]->getSize().x / 2.0f)) {
                if(abs(yDist) < abs(m_size.y / 2.0f + entities[i]->getSize().y / 2.0f)) {

                    float depth = xDist - (m_size.x / 2.0f + entities[i]->getSize().x / 2.0f);
                    float force = (depth / 2.0f) * (depth / 2.0f) / ((m_size.x / 2.0f + entities[i]->getSize().x / 2.0f) * 512.0f);


                    m_position.x -= force;
                    entities[i]->setPosition(glm::vec2(entities[i]->getPosition().x + force, entities[i]->getPosition().y));
                    continue;
                }
            }
        }
        break;
    }
    /// ENTITY COLLISION ENDS HERE

    /// TILE COLLISION STARTS HERE
    {
        /// Many thanks to Ben Arnold. He taught me almost everything I know about programming through his Youtube channel, "Makinggameswithben"
        /// This is just a small piece of code that handles and reacts to dynamic rectangle and tile collisions
        std::vector<glm::vec2> collideTilePositions;
        std::vector<glm::vec2> groundTilePositions;

        float x = m_position.x, y = m_position.y, width = m_size.x, height = m_size.y;

        glm::vec2 posBL(x, y);
        glm::vec2 posBR(x + width, y);
        glm::vec2 posTL(x, y + height);
        glm::vec2 posTR(x + width, y + height);

        const float testVar = 1.0f/16.0f;

        // Check for ground/ceiling

        // Bottom right corner
        checkTilePosition(world, groundTilePositions,
                          posBR.x - testVar,
                          posBR.y);


        // Bottom left corner
        checkTilePosition(world, groundTilePositions,
                          posBL.x + testVar,
                          posBL.y);


        // Top right corner
        checkTilePosition(world, groundTilePositions,
                          posTR.x - testVar,
                          posTR.y);


        // Top left corner
        checkTilePosition(world, groundTilePositions,
                          posTL.x + testVar,
                          posTL.y);

        // Top/Bottom sides
        for(float yMod = 0; yMod < height; yMod += height) {
            for(float xMod = 0; xMod < width - (2*testVar); xMod += 1.0f) {
                checkTilePosition(world, groundTilePositions,
                                  posBL.x + xMod + testVar,
                                  posBL.y + yMod);
            }
        }



        // Check the sides (not ground)
        checkTilePosition(world, collideTilePositions,
                          posBR.x,
                          posBR.y + testVar);

        checkTilePosition(world, collideTilePositions,
                          posBL.x,
                          posBL.y + testVar);

        checkTilePosition(world, collideTilePositions,
                          posTL.x,
                          posTL.y - testVar);

        checkTilePosition(world, collideTilePositions,
                          posTR.x,
                          posTR.y - testVar);

        // Sides
        for(float xMod = 0; xMod <= width; xMod += width) {
            for(float yMod = 0; yMod < height - (2*testVar); yMod += 1.0f) {
                checkTilePosition(world, collideTilePositions,
                                  posBL.x + xMod,
                                  posBL.y + yMod + testVar);
            }
        }

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

                checkTilePosition(world, predictiveTiles,
                                  p_posBR.x,
                                  p_posBR.y);

                checkTilePosition(world, predictiveTiles,
                                  p_posBL.x,
                                  p_posBL.y);

                checkTilePosition(world, predictiveTiles,
                                  p_posTL.x,
                                  p_posTL.y);

                checkTilePosition(world, predictiveTiles,
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

/// PRIVATE FUNCTIONS

bool Entity::checkTilePosition(World* world, std::vector<glm::vec2>& collideTilePositions, float x, float y) {
    // Get the position of this corner in grid-space
    glm::vec2 gridPos = glm::vec2(floor(x), floor(y)); // grid-space coords

    if(gridPos.y >= 0) {
        // If this is not an air tile, we should collide with it
        if (world->getTile(gridPos.x, gridPos.y, m_layer)->isSolid()) {
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
    Pos_TL = glm::vec2(m_position.x           , m_position.y + m_size.y);
    Pos_TR = glm::vec2(m_position.x + m_size.x, m_position.y + m_size.y);
    Pos_BR = glm::vec2(m_position.x + m_size.x, m_position.y           );
    Pos_BL = glm::vec2(m_position.x           , m_position.y           );

    // Find tiles
    Tile *Tile_TR, *Tile_TL, *Tile_BR, *Tile_BL;
    Tile_TR = world->getTile(Pos_TR.x, Pos_TR.y, m_layer);
    Tile_TL = world->getTile(Pos_TL.x, Pos_TL.y, m_layer);
    Tile_BR = world->getTile(Pos_BR.x, Pos_BR.y, m_layer);
    Tile_BL = world->getTile(Pos_BL.x, Pos_BL.y, m_layer);

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

void Entity::updateMovement(World* world) {
    if(m_controls[0]) { // UP
        if(m_onGround) {
            m_velocity.y = m_jumpHeight;
            m_onGround = false;
        }
    }
    if(m_controls[1]) { // DOWN
        /// TODO: implement crouching
    }
    if(m_controls[2] && !m_controls[3]) { // LEFT
        if(std::abs(m_velocity.x) < m_maxSpeed) {
            m_velocity.x -= m_speed;
        }
    } else if(m_controls[3] && !m_controls[2]) { // RIGHT
        if(std::abs(m_velocity.x) < m_maxSpeed) {
            m_velocity.x += m_speed;
        }
    } else {
        m_velocity.x /= 5.0f;
    }
    if(m_controls[4]) { // Backwards (layer++)
        moveDownLayer(world);
    } else if(m_controls[5]) { // Forwards (layer--)
        moveUpLayer(world);
    }

    if(m_velocity.x > MAX_SPEED) {
        m_velocity.x = MAX_SPEED;
    } else if(m_velocity.x < -MAX_SPEED) {
        m_velocity.x = -MAX_SPEED;
    }

    for(int i = 0; i < 6; i++) {
        m_controls[i] = false;
    }
}

void Entity::updateSounds(World* world, AudioManager* audio) {
    if(m_onGround && std::abs(m_velocity.x) > 0.01f) {
        m_soundTimer += std::abs(m_velocity.x * 3) + 1;
    } else if(std::abs(m_velocity.x) <= 0.01f || !m_onGround) {
        m_soundTimer = 0;
    }
    if(m_onGround && m_soundTimer > 20) { // 20 is arbitrary, should probably add a variable to each type of entity (for longer strides, etc.)
        glm::vec2 tileCoordsFloor = glm::vec2((int)(m_position.x + 0.5f), (int)(m_position.y - 0.5f));

        audio->playSoundEffect(world->getTile(tileCoordsFloor.x, tileCoordsFloor.y, m_layer)->getWalkedOnSoundEffectID(), MIX_MAX_VOLUME);
        m_soundTimer = 0;
    }
}

void Entity::interact(World* world, ScriptQueue* sq) {
    if(m_onGround && std::abs(m_velocity.x) > 0.1f) {
        glm::vec2 tileCoordsFloor = glm::vec2((int)(m_position.x + 0.5f), (int)(m_position.y - 0.5f));
        world->getTile(tileCoordsFloor.x, tileCoordsFloor.y, m_layer)->interact_WalkedOn(sq);
    }
}

void Entity::setAITarget(World* world, unsigned int selfIndex) {
    unsigned int entCount = world->getEntities().size();

    Entity* targetL = nullptr;
    Entity* targetR = nullptr;

    for(int i = selfIndex-1; i > -entCount - selfIndex; i--) {
        // This loop will run negatively, for length of world->getEntities().size() MAX. Often breaks out of loop early
        unsigned int normalized = (i + entCount*2) % entCount; // index from 0-size

        if(normalized == selfIndex) continue;

        Entity* target = world->getEntities()[normalized];

        if((int)getFaction() > (int)Categories::Faction::NEUTRAL &&
           (int)target->getFaction() <= (int)Categories::Faction::NEUTRAL) {
            targetL = target;
            break;
        }
    }

    for(int i = selfIndex+1; i < entCount + selfIndex; i++) { // Runs at most once through every entity.
        unsigned int normalized = i % entCount; // index from 0-size;

        if(normalized == selfIndex) continue;

        Entity* target = world->getEntities()[normalized];

        if((int)getFaction() > (int)Categories::Faction::NEUTRAL &&
           (int)target->getFaction() <= (int)Categories::Faction::NEUTRAL) {
            targetR = target;
            break; /// TODO: Accoutn for items lol
        }
    }

    float distToTargetL = targetL ? world->getDistance(m_position, targetL->getPosition()) : (unsigned int)-1;
    float distToTargetR = targetR ? world->getDistance(m_position, targetR->getPosition()) : (unsigned int)-1;

    if(distToTargetL < distToTargetR) {
        if(targetL) {
            pathfindToTarget(world, glm::vec3((int)targetL->getPosition().x, (int)targetL->getPosition().y, targetL->getLayer()), true);
        }
    } else {
        if(targetR) {
            pathfindToTarget(world, glm::vec3((int)targetR->getPosition().x, (int)targetR->getPosition().y, targetR->getLayer()), false);
        }
    }
}

#include <iostream>

void Entity::pathfindToTarget(World* world, glm::vec3 target, bool goLeft) {
    /**
        1. Set outer bounds (how far away the algorithm will expand before deciding there's no path)
        2. 'Link' all accessible tiles as "NavTile"s, effectively creating a system of nodes
        3. Calculate costs of each path to target coords
        4. Use the shortest route, adding each NavTile's coords to AI targets
        5. Return!
    */

    m_targets.clear();

    /// TODO: Implement jumping over gaps!

    // 1. Set outer bounds
    int outerBoundXMin = m_position.x < target.x ? m_position.x : target.x; // The lesser of the two
    int outerBoundXMax = m_position.x > target.x ? m_position.x : target.x; // The greater of the two


    // 2. Link accessible tiles from start
    NavTile* start = nullptr;

    int jumpHeight = std::floor((60.0f*m_jumpHeight*m_jumpHeight) / 2.45f + 0.1f);

    start = expandTile(world, glm::vec3((int)m_position.x, (int)m_position.y, m_layer), jumpHeight, m_size, nullptr, target);

    // Check to make sure if there's a path:
    if(!start) {
        /// TODO: Implement wandering.
        return;
    }


    // 4. Use shortest route, adding each NavTile coords to targets

    /**
        Construct 'frontier' (just the child tiles of start)

        Repeat until a path is found (Just one):
            Choose smallest h value on frontier, remove from frontier and add children to frontier (expand node)
            if tile with smallest h value is target, go backwards. Start with the tile with smallest h, and add each parent to targets.

    */
    std::vector<NavTile*> frontier;
    frontier.push_back(start);

    bool pathFound = false;
    NavTile* targetNavTile = nullptr;

    // Expand start tile, add em to frontier, then expand nextTile with lowest h, add em to frontier, repeat.


    unsigned int searchIteration = 0;
    unsigned int highestH = 0;

    while(!pathFound && highestH < 100 && searchIteration < 50 && frontier.size() > 0) {

        // Choose lowest h value
        int lowestIndex = 0;
        for(unsigned int i = 0; i < frontier.size(); i++) {
            if(frontier[lowestIndex]->h > frontier[i]->h) lowestIndex = i;
            if(frontier[i]->h > highestH) highestH = frontier[i]->h;
        }

        if(frontier[lowestIndex]->pos == target) {
            // We found a path!
            targetNavTile = frontier[lowestIndex];
            pathFound = true;
        }

        // Expand and remove (if there are no nextNodes, it will simply be removed)
        for(int i = 0; i < frontier[lowestIndex]->nextNodes.size(); i++) {
            addToFrontier(expandTile(world, frontier[lowestIndex]->nextNodes[i], jumpHeight, m_size, frontier[lowestIndex], target), frontier);
        }
        for(int i = lowestIndex; i < frontier.size()-1; i++) {
            frontier[i] = frontier[i+1];
        }
        frontier.pop_back();

        searchIteration++;
    }


    if(pathFound) {
        std::vector<glm::vec3> targetsReversed; // Targets to go to, reversed :)

        while(targetNavTile) {
            glm::vec3 p(targetNavTile->pos);
            targetsReversed.push_back(p);
            targetNavTile = targetNavTile->parent;
        }

        for(unsigned int i = 1; i < targetsReversed.size(); i++) { // We don't need the very last one, that's just on top of the entity
            m_targets.push_back(targetsReversed[targetsReversed.size()-1 - i]);
        }
    }

    return;
}

NavTile* Entity::expandTile(World* world, glm::vec3 pos, int jumpHeight, glm::vec2 size, NavTile* parent, glm::vec3 target) {

    NavTile* ret = new NavTile;
    ret->parent = parent;
    ret->pos = pos;

    // Check Left
    Tile* left = world->getTile(pos.x-1, pos.y, pos.z);
    if(fitsOnTile(world, left, true)) {
        glm::vec3 p(left->getPosition().x, left->getPosition().y, left->getLayer());
        if(!parent || p != parent->pos) {
            ret->nextNodes.push_back(p);
        }
    } else {
        for(int i = 0; i < jumpHeight; i++) { /// TODO: Implement some sort of 'safe' fall distance
            Tile* leftDown = world->getTile(pos.x-1, pos.y-i, pos.z);// Check tiles below so that we can fall distances.
            if(leftDown) {
                if(fitsOnTile(world, leftDown)) {
                    glm::vec3 p(leftDown->getPosition().x, leftDown->getPosition().y, leftDown->getLayer());
                    if(!parent || p != parent->pos) {
                        ret->nextNodes.push_back(p);
                    }
                }
            }
        }
    }

    Tile* right = world->getTile(pos.x+1, pos.y, pos.z);
    if(fitsOnTile(world, right, true)) {
        glm::vec3 p(right->getPosition().x, right->getPosition().y, right->getLayer());
        if(!parent || p != parent->pos) {
            ret->nextNodes.push_back(p);
        }
    } else {
        for(int i = 0; i < jumpHeight; i++) { /// TODO: Implement some sort of 'safe' fall distance
            Tile* rightDown = world->getTile(pos.x+1, pos.y-i, pos.z);// Check tiles below so that we can fall distances.
            if(rightDown) {
                if(fitsOnTile(world, rightDown)) {
                    glm::vec3 p(rightDown->getPosition().x, rightDown->getPosition().y, rightDown->getLayer());
                    if(!parent || p != parent->pos) {
                        ret->nextNodes.push_back(p);
                    }
                }
            }
        }
    }


    if(pos.z < WORLD_DEPTH-1) {
        // Check Behind
        Tile* back = world->getTile(pos.x, pos.y, pos.z+1);
        if(fitsOnTile(world, back, true)) {
            glm::vec3 p(back->getPosition().x, back->getPosition().y, back->getLayer());
            if(!parent || p != parent->pos) {
                ret->nextNodes.push_back(p);
            }
        } else {
            for(int i = 0; i < jumpHeight; i++) { /// TODO: Implement some sort of 'safe' fall distance
                Tile* backDown = world->getTile(pos.x, pos.y-i, pos.z+1);// Check tiles below so that we can fall distances.
                if(backDown) {
                    if(fitsOnTile(world, backDown)) {
                        glm::vec3 p(backDown->getPosition().x, backDown->getPosition().y, backDown->getLayer());
                        if(!parent || p != parent->pos) {
                            ret->nextNodes.push_back(p);
                        }
                    }
                }
            }
        }
    }


    if(pos.z > 0) {
        // Check In Front
        Tile* front = world->getTile(pos.x, pos.y, pos.z-1);
        if(fitsOnTile(world, front, true)) {
            glm::vec3 p(front->getPosition().x, front->getPosition().y, front->getLayer());
            if(!parent || p != parent->pos) {
                ret->nextNodes.push_back(p);
            }
        } else {
            for(int i = 0; i < jumpHeight; i++) { /// TODO: Implement some sort of 'safe' fall distance
                Tile* frontDown = world->getTile(pos.x, pos.y-i, pos.z-1);// Check tiles below so that we can fall distances.
                if(frontDown) {
                    if(fitsOnTile(world, frontDown)) {
                        glm::vec3 p(frontDown->getPosition().x, frontDown->getPosition().y, frontDown->getLayer());
                        if(!parent || p != parent->pos) {
                            ret->nextNodes.push_back(p);
                        }
                    }
                }
            }
        }
    }

    // Check up!
    for(int i = 0; i < jumpHeight; i++) {
        Tile* up = world->getTile(pos.x, pos.y+i, pos.z);
        if(up) {
            if(fitsOnTile(world, up, false)) {
                glm::vec3 p(up->getPosition().x, up->getPosition().y, up->getLayer());
                if(!parent || p != parent->pos) {
                    ret->nextNodes.push_back(p);
                }
            } else {
                break; // Something is blocking our jump
            }
        }
    }

    calculateCost(world, ret, target);

    return ret;
}

bool Entity::fitsOnTile(World* world, Tile* t, bool needsFloor) {
    bool fits = true;
    int start = needsFloor ? -1 : 0;

    for(int y = start; fits && y < m_size.y; y++) {
        for(int x = 0; fits && x < m_size.x; x++) {
            Tile* tmp = world->getTile(t->getPosition().x + x, t->getPosition().y + y, t->getLayer());
            if(tmp) {
                if(y >= 0) {
                    if(tmp->isSolid()) fits = false;
                } else {
                    if(!tmp->isSolid()) fits = false;
                }
            }
        }
    }
    return fits;
}

void Entity::calculateCost(World* world, NavTile* tile, glm::vec3 target) {
    tile->h = world->getDistance(glm::vec2(tile->pos.x, tile->pos.y), glm::vec2(target.x, target.y)) + std::abs(tile->pos.z - target.z); /// TODO: Implement crossover
    if(tile->parent) tile->h += tile->parent->h/2.0f; // The distance to the entity is more important than the path. Plus, this improves performance.
}

void Entity::addToFrontier(NavTile* tile, std::vector<NavTile*>& frontier) {
    bool clone = false;

    for(int i = 0; i < frontier.size(); i++) {
        if(frontier[i]->pos == tile->pos) {
            if(frontier[i]->h > tile->h) { // Tile is a better route to get to frontier[i]
                frontier[i] = tile;
            }
            clone = true;
        }
    }

    if(clone) return;
    frontier.push_back(tile);
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
    if(m_layer < WORLD_DEPTH-1) {
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

void Entity::die(World* world) {
    if(m_canDie) {
        if(m_lootTableStart.getRarity() != -1.0f) {
            for(int i = 0; i < m_lootRolls; i++) {
                EntityNeutralItem* item = new EntityNeutralItem(m_position + glm::vec2(0.0f, 2.0f), m_layer, createItem((unsigned int)m_lootTableStart.roll(), 1));
                world->addEntity(item);
            }
        }
        m_isDead = true;
    } else {
        m_isDead = false; // Just in case
    }
}

void Entity::attack(Entity* enemy) {
    /// TODO: determine limbSection

    float damage = 0.0f;
    LimbSection limb = LimbSection::BODY;
    ItemWeapon* weapon = nullptr;

    for(int i = 0; i < m_equippedItems.size(); i++) {
        if(m_inventory->getItem(m_equippedItems[i])->isWeapon()) weapon = static_cast<ItemWeapon*>(m_inventory->getItem(m_equippedItems[i]));
    }

    damage = weapon ? (weapon->getDamage()) : 1.0f;

    enemy->defend(this, damage, limb, weapon);
}

void Entity::defend(Entity* attacker, float damage, LimbSection section, Item* weapon) {

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

}
