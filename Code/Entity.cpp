#include "Entity.h"

#include <DebugRenderer.h>

Entity::Entity()
{
    //ctor
}

Entity::Entity(glm::vec2 position, Categories::Entity_Type type, unsigned int id) {
    init(position, type, id);
}

Entity::~Entity()
{
    //dtor
}

void Entity::init(glm::vec2 position, Categories::Entity_Type type, unsigned int id) {
    m_position = position;
    switch(type) {
        case Categories::Entity_Type::MOB: /// Temporary
        {
            if(Category_Data::mobData[id].id == id) {
                m_texture = GLEngine::ResourceManager::getTexture(Category_Data::mobData[id].texturePath);
                m_size = Category_Data::mobData[id].size;
                m_faction = Category_Data::mobData[id].faction;
            } else {
                return;
            }
        }
    }
}

void Entity::update(Chunk* chunks[WORLD_SIZE]) {
    setParentChunk(chunks);
}

void Entity::draw(GLEngine::SpriteBatch& sb, GLEngine::DebugRenderer& dr) {
    glm::vec4 destRect = glm::vec4(m_position.x, m_position.y, m_size.x * TILE_SIZE, m_size.y * TILE_SIZE);
    glm::vec4 uvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    GLEngine::ColourRGBA8 colour(255, 255, 255, 255);

    sb.draw(destRect, uvRect, m_texture.id, 0.0f, colour);

    dr.drawBox(glm::vec4(m_position.x, m_position.y, std::floor(m_size.x) * TILE_SIZE, std::floor(m_size.y) * TILE_SIZE), GLEngine::ColourRGBA8(255, 255, 255, 255), 0.0f);
}

void Entity::collide(std::vector<Entity*> entities, int chunkI) {

    if(chunkI == m_parentChunkIndex) {
        /// ENTITY COLLISION STARTS HERE
        for(auto e : entities) {
            if(e != this) {
                if((e->getFaction() > Categories::Faction::NEUTRAL && m_faction < Categories::Faction::NEUTRAL) ||
                   (e->getFaction() < Categories::Faction::NEUTRAL && m_faction > Categories::Faction::NEUTRAL)) {
                    /// They are different (opposing) factions. Make them collide with each other WORKING
                    if(e->getPosition().x < m_position.x + m_size.x || e->getPosition().x + e->getSize().x > m_position.x) {
                        if(e->getPosition().y < m_position.y - m_size.y && e->getPosition().y - e->getSize().y > m_position.y) {
                            //m_position += m_position - e->getPosition() - m_size;
                            /// I'll do this later
                        }
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

            // Check the four corners
            // First corner
            checkTilePosition(m_parentChunk->tiles,
                              chunkI,
                              collideTilePositions,
                              m_position.x,
                              m_position.y);
            // Second Corner
            checkTilePosition(m_parentChunk->tiles,
                              chunkI,
                              collideTilePositions,
                              m_position.x + m_size.x * TILE_SIZE,
                              m_position.y);

            // Third Corner
            checkTilePosition(m_parentChunk->tiles,
                              chunkI,
                              collideTilePositions,
                              m_position.x,
                              m_position.y + m_size.y * TILE_SIZE / 1.0f);

            // Fourth Corner
            checkTilePosition(m_parentChunk->tiles,
                              chunkI,
                              collideTilePositions,
                              m_position.x + m_size.x * TILE_SIZE,
                              m_position.y + m_size.y * TILE_SIZE / 1.0f);

//            // Left Side
//            checkTilePosition(m_parentChunk->tiles,
//                              chunkI,
//                              collideTilePositions,
//                              m_position.x,
//                              m_position.y - m_size.y * TILE_SIZE / 2.0f);
//
//            // Right Side
//            checkTilePosition(m_parentChunk->tiles,
//                              chunkI,
//                              collideTilePositions,
//                              m_position.x + m_size.x * TILE_SIZE,
//                              m_position.y - m_size.y * TILE_SIZE / 2.0f);

            // Do the collision
            for (int i = 0; i < collideTilePositions.size(); i++) {
                collideWithTile(collideTilePositions[i]);
            }
        }
        /// TILE COLLISION ENDS HERE
    }
}



/// PRIVATE FUNCTIONS

void Entity::setParentChunk(Chunk* worldChunks[WORLD_SIZE]) {
    int index = std::floor(m_position.x / TILE_SIZE / (CHUNK_SIZE+1));

    if(index != m_parentChunkIndex) {
        m_parentChunk = worldChunks[index];
        m_parentChunkIndex = index;
    }

}

void Entity::checkTilePosition(Tile tiles[WORLD_HEIGHT][TRUE_CHUNK_SIZE], int chunkI, std::vector<glm::vec2>& collideTilePositions, float x, float y) {
    // Get the position of this corner in grid-space
    glm::vec2 gridPos = glm::vec2(floor(x / TILE_SIZE),
                                      floor(y / TILE_SIZE)); // grid-space coords

    // If we are outside the world, just return
    if (gridPos.x < CHUNK_SIZE * m_parentChunkIndex || gridPos.x >= CHUNK_SIZE + CHUNK_SIZE * m_parentChunkIndex ||
        gridPos.y < 0 || gridPos.y >= WORLD_HEIGHT) {
        return;
    }

    // If this is not an air tile, we should collide with it
    if (tiles[(int)gridPos.y][(int)gridPos.x].isSolid()) {
        collideTilePositions.push_back(glm::vec2((int)gridPos.x + 0.5, (int)gridPos.y - 0.5)); // CollideTilePositions are put in as gridspace coords
    }
}

void Entity::collideWithTile(glm::vec2 tilePos) {
    // Find the distance between tile and rectangle's(!) axes(!) in percentage of it's axis (ex. (depth.x = 42) -> (size.x * .42))
    glm::vec2 centrePos = glm::vec2(m_position.x + m_size.x * TILE_SIZE / 2.0f, m_position.y - m_size.y * TILE_SIZE / 2.0f);
    glm::vec2 distVec = centrePos - (tilePos * glm::vec2(TILE_SIZE));

    glm::vec2 depthVec = (abs(distVec) - glm::vec2(TILE_SIZE)) / m_size;

    // Determine if it's shorter to go in the X or Y direction
    if(abs(depthVec.x) < abs(depthVec.y)) { // X direction is shorter // Maybe change these variables from depthvec to distvec and reverse the comparison to >
        if(distVec.x < 0) {
            m_position.x += depthVec.x; // TILE ON RIGHT worked
        } else {
            m_position.x -= depthVec.x; // TILE ON LEFT WORKS
        }
    } else {
        if(distVec.y < 0) {
            m_position.y -= depthVec.y / m_size.y; // TILE ON BOTTOM worked
        } else {
            m_position.y -= depthVec.y / m_size.y; // TILE ON TOP WORKS
        }
    }

    // Go the (shorter) distance in the X or Y direction
}
