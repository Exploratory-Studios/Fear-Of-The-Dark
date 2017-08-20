#include "Entity.h"

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
            } else {
                return;
            }
        }
    }
}

void Entity::update() {

}

void Entity::draw(GLEngine::SpriteBatch& sb) {
    glm::vec4 destRect = glm::vec4(m_position.x, m_position.y, m_size.x * TILE_SIZE, m_size.y * TILE_SIZE);
    glm::vec4 uvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    GLEngine::ColourRGBA8 colour(255, 255, 255, 255);

    sb.draw(destRect, uvRect, m_texture.id, 0.0f, colour);
}

void Entity::collide(std::vector<Entity*> entities, Tile chunkTiles[WORLD_HEIGHT][CHUNK_SIZE]) {


    /// ENTITY COLLISION STARTS HERE
    for(auto e : entities) {
        if(e != this) {
            if((e->getFaction() > Categories::Faction::NEUTRAL && m_faction < Categories::Faction::NEUTRAL) ||
               (e->getFaction() < Categories::Faction::NEUTRAL && m_faction > Categories::Faction::NEUTRAL)) {
                /// They are different (opposing) factions. Make them collide with each other

                if(e->getPosition().x < m_position.x + m_size.x && e->getPosition().x + e->getSize().x > m_position.x) {
                    if(e->getPosition().y < m_position.y + m_size.y && e->getPosition().y + e->getSize().y > m_position.y) {
                        e->setPosition(e->getPosition() + m_position / glm::vec2(2.0f));
                        m_position = e->getPosition() + m_position / glm::vec2(-2.0f);
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
        checkTilePosition(chunkTiles,
                          collideTilePositions,
                          m_position.x,
                          m_position.y);
        // Second Corner
        checkTilePosition(chunkTiles,
                          collideTilePositions,
                          m_position.x + m_size.x,
                          m_position.y);

        // Third Corner
        checkTilePosition(chunkTiles,
                          collideTilePositions,
                          m_position.x,
                          m_position.y + m_size.y);

        // Third Corner
        checkTilePosition(chunkTiles,
                          collideTilePositions,
                          m_position.x + m_size.x,
                          m_position.y + m_size.y);

        // Do the collision
        for (int i = 0; i < collideTilePositions.size(); i++) {
            collideWithTile(collideTilePositions[i]);
        }
    }
    /// TILE COLLISION ENDS HERE
}

void Entity::checkTilePosition(Tile tiles[WORLD_HEIGHT][CHUNK_SIZE], std::vector<glm::vec2>& collideTilePositions, float x, float y) {
    // Get the position of this corner in grid-space
    glm::vec2 gridPos = glm::vec2(floor(x) / TILE_SIZE,
                                      floor(y) / TILE_SIZE); // gridspace coords

    // If we are outside the world, just return
    if (gridPos.x < 0 || gridPos.x >= CHUNK_SIZE ||
        gridPos.y < 0 || gridPos.y >= WORLD_HEIGHT) {
        return;
    }

    // If this is not an air tile, we should collide with it
    if (tiles[(int)gridPos.y][(int)gridPos.x].isSolid()) {
        collideTilePositions.push_back(gridPos + glm::vec2((float)TILE_SIZE / 2.0f)); // CollideTilePositions are put in as gridspace coords
    }
}

void Entity::collideWithTile(glm::vec2 tilePos) {
    const float TILE_RADIUS = (float)TILE_SIZE / 2.0f; // Pixel Coords
    // The minimum distance before a collision occurs
    const float MIN_X_DISTANCE = m_size.x / TILE_SIZE + TILE_SIZE; // Gridspace coords
    const float MIN_Y_DISTANCE = m_size.y / TILE_SIZE + TILE_SIZE;

    // Vector from the agent to the tile
    glm::vec2 distVec = m_position / glm::vec2(TILE_SIZE) + m_size / glm::vec2(TILE_SIZE) / 2.0f - tilePos; //  gridspace coords, tilepos is already gridspace

    // Get the depth of the collision
    float xDepth = MIN_X_DISTANCE - abs(distVec.x);
    float yDepth = MIN_Y_DISTANCE - abs(distVec.y);

    // If both the depths are > 0, then we collided
    if (abs(xDepth) > 0 && abs(yDepth) > 0) {

        // Check which collision depth is less
        if (std::max(xDepth, 0.0f) > std::max(yDepth, 0.0f)) {
            // X collsion depth is smaller so we push in X direction
            if (distVec.x < 0) {
                m_position.x -= xDepth / TILE_SIZE;
            } else {
                m_position.x += xDepth / TILE_SIZE;
            }
        } else {
            // Y collsion depth is smaller so we push in X direction
            if (distVec.y < 0) {
                m_position.y += yDepth / TILE_SIZE;
            } else {
                m_position.y -= yDepth / TILE_SIZE;
            }
        }
    }
}
