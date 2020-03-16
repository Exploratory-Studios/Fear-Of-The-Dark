#include "Tile.h"

#include "World.h"

#include <random>
#include <boost/thread.hpp>

Tile::Tile() {
    //m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/UNDEFINED.png").id;
}

float Tile::getLight() {
    if(m_ambientLight > m_sunLight && m_ambientLight > m_emittedLight) {
        return m_ambientLight;
    } else if(m_sunLight > m_emittedLight) {
        return m_sunLight;
    } else {
        return m_emittedLight;
    }
    return m_ambientLight;
}

float Tile::getSurroundingHeat(World* world) {
    int x = (int)m_pos.x;
    int y = (int)m_pos.y;

    float temp = 0.0f;

    Tile* left =  world->getTile(x-1, y,   m_layer);
    Tile* right = world->getTile(x+1, y,   m_layer);
    Tile* up =    world->getTile(x,   y+1, m_layer);
    Tile* down =  world->getTile(x,   y-1, m_layer);

    if(down) {// 1 below
        if(!down->isSolid() || !isSolid()) {
            float newHeat = down->getRawHeat() - TRANSPARENT_LIGHT_MINUS;
            temp += newHeat;
        } else {
            float newHeat = down->getRawHeat() - OPAQUE_LIGHT_MINUS;
            temp += newHeat;
        }
    }
    if(up) { // 1 above
        if(!up->isSolid() || !isSolid()) {
            float newHeat = up->getRawHeat() - TRANSPARENT_LIGHT_MINUS;
            temp += newHeat;
        } else {
            float newHeat = up->getRawHeat() - OPAQUE_LIGHT_MINUS;
            temp += newHeat;
        }
    }
    if(left) { // to the left
        if(!left->isSolid() || !isSolid()) {
            float newHeat = left->getRawHeat() - TRANSPARENT_LIGHT_MINUS;
            temp += newHeat;
        } else {
            float newHeat = left->getRawHeat() - OPAQUE_LIGHT_MINUS;
            temp += newHeat;
        }
    }
    if(right) { // to the right
        if(!right->isSolid() || !isSolid()) {
            float newHeat = right->getRawHeat() - TRANSPARENT_LIGHT_MINUS;
            temp += newHeat;
        } else {
            float newHeat = right->getRawHeat() - OPAQUE_LIGHT_MINUS;
            temp += newHeat;
        }
    }

    if(std::abs(temp / 4.0f - m_lastTemperature) > 0.01f) {
        if(up) up->setToUpdate_heat();
        if(down) down->setToUpdate_heat();
        if(left) left->setToUpdate_heat();
        if(right) right->setToUpdate_heat();
    }

    return temp / 4.0f;
}

float Tile::getHeat(World* world) {
    float baseHeat = Category_Data::placeData[(unsigned int)world->getPlace(m_pos.x)].baseTemperature;
    float highHeat = Category_Data::placeData[(unsigned int)world->getPlace(m_pos.x)].maxTemp;

    float heat = getRawHeat();

    heat += m_sunLight * (highHeat - baseHeat);
    heat += baseHeat;

    return heat;
}

float Tile::getRawHeat() {
    return (m_emittedHeat > m_temperature ? m_emittedHeat : m_temperature);
}

void Tile::update(World* world, float time, bool updateLighting, const float& sunlight) {
    if(updateLighting) {
        if(m_needsSunCheck) {
            resetSunlightCorners();

            m_needsSunCheck = false;
            m_exposedToSun = exposedToSun(world);

            calculateSunlight(world, sunlight);
        }
    }

    onUpdate(world, time);
}

void Tile::tick(World* world, float tickTime, const float& sunlight) {
    if(m_updateHeat && m_id != (unsigned int)Categories::BlockIDs::AIR) {
        float heat = getSurroundingHeat(world);
        m_temperature = heat;
        m_updateHeat = false;
    }

    calculateSunlight(world, sunlight);

    onTick(world, tickTime);
}

void Tile::draw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, int xOffset, int depthDifference) {

    if(m_draw) {
        if(m_textureId == (GLuint)-1) {
            loadTexture();
        }

        //GLint lightColourUniform = program.getUniformLocation("lightColour");
        //glUniform4fv(lightColourUniform, 3, &glm::vec3(0.0f, 1.0f, 0.0f)[0]);

        GLEngine::ColourRGBA8 colour = m_colour;
        if(depthDifference != 0) {
            if(depthDifference > 0) {// in front of the player
                colour.a = 150;
                colour.r = 64;
                colour.g = 64;
                colour.b = 64;
            } else {
                float c = 100 / -depthDifference; // Same layer will be (255, 255, 255), 1 layer back (64, 64, 64), 2 layers (32, 32, 32), etc.
                colour.r = c;
                colour.g = c;
                colour.b = c;
            }
        }

        int r = m_colour.r, g = m_colour.g, b = m_colour.b;
        glm::vec4 pos = glm::vec4(m_pos.x + xOffset, m_pos.y, m_size.x, m_size.y);
        float depth = 0.75f * (WORLD_DEPTH - m_layer);

        sb.draw(pos,
                glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                m_textureId,
                depth,
                colour,
                m_cornerLight + m_cornerSunlight);

        onDraw(sb, sf, pos, depth);
    }
}

void Tile::drawBackdrop(GLEngine::SpriteBatch& sb, int xOffset, int yOffset, float lightLevel) {
    if(m_draw && m_backdrop) {
        //GLint lightColourUniform = program.getUniformLocation("lightColour");
        //glUniform4fv(lightColourUniform, 3, &glm::vec3(0.0f, 1.0f, 0.0f)[0]);

        int r = m_colour.r * 0.2f, g = m_colour.g * 0.2f, b = m_colour.b * 0.2f;
        sb.draw(glm::vec4(m_pos.x + xOffset * CHUNK_SIZE, m_pos.y + yOffset, m_size.x, m_size.y),
                glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                m_backdropTextureId,
                0.1f * (WORLD_DEPTH - m_layer),
                GLEngine::ColourRGBA8(r, g, b, m_colour.a),
                m_cornerLight);
    }
}

void Tile::destroy(World* world) {
    if(m_emittedLight > 0.0f) {
        resetNeighboursLight(world);
    }
    {
        world->getTile(m_pos.x, m_pos.y, m_layer)->setNeedsSunCheck();
        Tile* R = world->getTile(m_pos.x+1, m_pos.y, m_layer);
        if(R) R->setNeedsSunCheck();
        Tile* L = world->getTile(m_pos.x-1, m_pos.y, m_layer);
        if(L) L->setNeedsSunCheck();
        Tile* B = world->getTile(m_pos.x, m_pos.y, m_layer+1);
        if(B) B->setNeedsSunCheck();
        Tile* F = world->getTile(m_pos.x, m_pos.y, m_layer-1);
        if(F) F->setNeedsSunCheck();
    }
}

bool Tile::exposedToSun(World* world) {
    if(!m_transparent) {
        /*Tile* left = world->getTile(m_pos.x-1, m_pos.y, m_layer);
        if(left) {
            if(left->isTransparent()) {
                if(left->needsSunCheck()) {
                    setNeedsSunCheck();
                }
                if(left->isExposedToSunlight()) {
                    return true;
                }
            }
        }

        Tile* right = world->getTile(m_pos.x+1, m_pos.y, m_layer);
        if(right) {
            if(right->isTransparent()) {
                if(right->needsSunCheck()) { // The difference is due to the fact that the blocks on the right will tick after these ones.
                    setNeedsSunCheck();
                }
                if(right->isExposedToSunlight()) {
                    return true;
                }
            }
        }
        Tile* back = world->getTile(m_pos.x, m_pos.y, m_layer+1);
        if(back) {
            if(back->isTransparent()) {
                if(back->needsSunCheck()) {
                    setNeedsSunCheck();
                }
                if(back->isExposedToSunlight()) {
                    return true;
                }
            }
        }
        Tile* front = world->getTile(m_pos.x, m_pos.y, m_layer-1);
        if(front) {
            if(front->isTransparent()) {
                if(front->needsSunCheck()) {
                    setNeedsSunCheck();
                }
                if(front->isExposedToSunlight()) {
                    return true;
                }
            }
        }*/
    }
    for(int i = m_pos.y+1; i < WORLD_HEIGHT; i++) {
        if(!(world->getTile(m_pos.x, i, m_layer)->isTransparent())) {
            return false;
        }
    }
    return true;
}

void Tile::calculateSunlight(World* world, float sunlight) {
    if(m_exposedToSun) {
        bool left = true, right = true;
        if(!m_transparent) {
            Tile* leftT = world->getTile(m_pos.x-1, m_pos.y, m_layer);
            if(leftT) {
                if(!(leftT->isTransparent() && leftT->isExposedToSunlight())) {
                    left = false;
                }
            }
            Tile* rightT = world->getTile(m_pos.x+1, m_pos.y, m_layer);
            if(rightT) {
                if(!(rightT->isTransparent() && rightT->isExposedToSunlight())) {
                    right = false;
                }
            }
        }
        Tile* leftT = world->getTile(m_pos.x-1, m_pos.y, m_layer);
        if(leftT) {
            if(!(leftT->isTransparent() || leftT->isExposedToSunlight())) {
                leftT->setSunLight(sunlight, false, true, false, m_transparent);
            }
        }
        Tile* rightT = world->getTile(m_pos.x+1, m_pos.y, m_layer);
        if(rightT) {
            if(!(rightT->isTransparent() || rightT->isExposedToSunlight())) {
                rightT->setSunLight(sunlight, true, false, false, m_transparent);
            }
        }
        setSunLight(sunlight, left, right, true, true);
    }
}

TileData Tile::getSaveData() {
    TileData d;
    d.pos = m_pos;
    d.id = m_id;
    delete d.metaData;
    d.metaData = getMetaData();
    d.layer = m_layer;
    return d;
}

void Tile::resetNeighboursLight(World* world) {
    float range = std::sqrt(std::abs(m_emittedLight)) * 4.0f;

    float TL, TR, BL, BR;

    for(int y = -std::ceil(range); y <= std::ceil(range); y++) {
        for(int x = -std::ceil(range); x <= std::ceil(range); x++) {
            for(int layer = -std::ceil(range); layer <= std::ceil(range); layer++) {
                Tile* t = world->getTile(m_pos.x + x, m_pos.y + y, m_layer + layer);
                if(t) {
                    float dist = std::sqrt((x)*(x) + (y)*(y) + (layer*layer));
                    float light = (1.0f - dist/(float)range) * m_emittedLight;

                    if(light > 0.0f) {
                        t->addAmbientLight(-light);
                    }
                    { // Bottom left corner (x, y)
                        float d = std::sqrt((x-0.5f)*(x-0.5f) + (y-0.5f)*(y-0.5f) + (layer*layer));
                        BL = ((1.0f - d/(float)(range)) * m_emittedLight); // x + 0, y + 0
                    }
                    { // Bottom right corner (x+1, y)
                        float d = std::sqrt((x+0.5f)*(x+0.5f) + (y-0.5f)*(y-0.5f) + (layer*layer));
                        BR = ((1.0f - d/(float)(range)) * m_emittedLight); // x + 0, y + 0
                    }
                    { // top left corner (x, y+1)
                        float d = std::sqrt((x-0.5f)*(x-0.5f) + (y+0.5f)*(y+0.5f) + (layer*layer));
                        TL = ((1.0f - d/(float)(range)) * m_emittedLight); // x + 0, y + 0
                    }
                    { // top right corner (x+1, y+1)
                        float d = std::sqrt((x+0.5f)*(x+0.5f) + (y+0.5f)*(y+0.5f) + (layer*layer));
                        TR = ((1.0f - d/(float)(range)) * m_emittedLight); // x + 0, y + 0
                    }

                    if(BL > 0 || BR > 0 || TR > 0 || TL > 0) {
                        t->subtractCornerLight(TL, TR, BR, BL);
                    }
                }
            }
        }
    }
}
#include <iostream>
void Tile::setNeighboursLight(World* world) {
    float range = std::sqrt(std::abs(m_emittedLight)) * 4.0f;

    float TL, TR, BL, BR;

    for(int y = -std::ceil(range); y <= std::ceil(range); y++) {
        for(int x = -std::ceil(range); x <= std::ceil(range); x++) {
            for(int layer = -std::ceil(range); layer <= std::ceil(range); layer++) {
                Tile* t = world->getTile(m_pos.x + x, m_pos.y + y, m_layer + layer);
                if(t) {
                    float dist = std::sqrt((x)*(x) + (y)*(y) + (layer*layer));
                    float light = (1.0f - dist/(float)range) * m_emittedLight;

                    if(light > 0.0f) {
                        t->addAmbientLight(light);
                    }
                    { // Bottom left corner (x, y)
                        float d = std::sqrt((x-0.5f)*(x-0.5f) + (y-0.5f)*(y-0.5f) + (layer*layer));
                        BL = ((1.0f - d/(float)(range)) * m_emittedLight); // x + 0, y + 0
                    }
                    { // Bottom right corner (x+1, y)
                        float d = std::sqrt((x+0.5f)*(x+0.5f) + (y-0.5f)*(y-0.5f) + (layer*layer));
                        BR = ((1.0f - d/(float)(range)) * m_emittedLight); // x + 0, y + 0
                    }
                    { // top left corner (x, y+1)
                        float d = std::sqrt((x-0.5f)*(x-0.5f) + (y+0.5f)*(y+0.5f) + (layer*layer));
                        TL = ((1.0f - d/(float)(range)) * m_emittedLight); // x + 0, y + 0
                    }
                    { // top right corner (x+1, y+1)
                        float d = std::sqrt((x+0.5f)*(x+0.5f) + (y+0.5f)*(y+0.5f) + (layer*layer));
                        TR = ((1.0f - d/(float)(range)) * m_emittedLight); // x + 0, y + 0
                    }

                    if(BL > 0 || BR > 0 || TR > 0 || TL > 0) {
                        t->addCornerLight(TL, TR, BR, BL);
                    }
                }
            }
        }
    }
}
