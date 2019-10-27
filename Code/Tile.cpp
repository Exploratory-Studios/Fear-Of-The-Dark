#include "Tile.h"

#include <random>
#include <boost/thread.hpp>

#include "WorldIOManager.h"
#include "Chunk.h"

Tile::Tile() {
    m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/UNDEFINED.png").id;
    m_parentChunk = nullptr;

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

void Tile::sunlight_transferToNeighbour(float& light) {
    float newLight = light - SUNLIGHT_MINUS;

    if(light <= 0.0f) return;
    Tile* down = m_parentChunk->getTile(m_pos.x, m_pos.y-1, m_layer);

    if(down) {
        down->sunlight_set(light);
        down->sunlight_transferToNeighbour(newLight);
    }
}

void Tile::sunlight_transferLeft(float& light) {
    float newLight = light - SUNLIGHT_MINUS;

    if(newLight > 0.0f) {
        Tile* left = m_parentChunk->getTile(m_pos.x - 1, m_pos.y, m_layer);
        if(left) {
            if(!left->isExposedToSunlight()) {
                left->sunlight_set(light);
                left->sunlight_transferLeft(newLight);
                left->sunlight_transferToNeighbour(newLight);
            }
        }
    }
}

void Tile::sunlight_transferRight(float& light) {
    float newLight = light - SUNLIGHT_MINUS;

    if(newLight > 0.0f) {
        Tile* right = m_parentChunk->getTile(m_pos.x + 1, m_pos.y, m_layer);
        if(right) {
            if(!right->isExposedToSunlight()) {
                right->sunlight_set(light);
                right->sunlight_transferRight(newLight);
                right->sunlight_transferToNeighbour(newLight);
            }
        }
    }
}

void Tile::sunlight_resetNeighbour() {
    m_sunLight = 0.0f;

    Tile* down = m_parentChunk->getTile(m_pos.x, m_pos.y-1, m_layer);
    if(down && down->getSunLight() > 0.0f) down->sunlight_resetNeighbour();

    Tile* left = m_parentChunk->getTile(m_pos.x - 1, m_pos.y, m_layer);
    if(left) {
        if(!left->isExposedToSunlight() && left->getSunLight() > 0.0f) {
            left->sunlight_resetNeighbour();
        }
    }
    Tile* right = m_parentChunk->getTile(m_pos.x + 1, m_pos.y, m_layer);
    if(right) {
        if(!right->isExposedToSunlight() && right->getSunLight() > 0.0f) {
            right->sunlight_resetNeighbour();
        }
    }
}

/*float Tile::getSurroundingLight() {
    int x = (int)m_pos.x;
    int y = (int)m_pos.y;

    float light = 0.0f;

    Tile* left =  m_parentChunk->getTile(x-1, y,   m_layer);
    Tile* right = m_parentChunk->getTile(x+1, y,   m_layer);
    Tile* up =    m_parentChunk->getTile(x,   y+1, m_layer);
    Tile* down =  m_parentChunk->getTile(x,   y-1, m_layer);

    if(down) {
        if(down->getLight() > light) light = down->getLight(); /// TODO: get more efficient
    }
    if(up) {
        if(up->getLight() > light) light = up->getLight();
    }
    if(right) {
        if(right->getLight() > light) light = right->getLight();
    }
    if(left) {
        if(left->getLight() > light) light = left->getLight();
    }

    if(m_transparent) {
        light -= TRANSPARENT_LIGHT_MINUS;
    } else {
        light -= OPAQUE_LIGHT_MINUS;
    }

    if(light < 0.01f) light = 0.0f;

    if(std::abs(m_lastLight - light) > 0.01f) {
        if(up) up->setToUpdate_light();
        if(down) down->setToUpdate_light();
        if(left) left->setToUpdate_light();
        if(right) right->setToUpdate_light();
    }

    return light;

    /*if(down) { // 1 below
        if(down->getLight() > light) {
            if(down->isTransparent() || isTransparent()) {
                float newLight = down->getLight() - TRANSPARENT_LIGHT_MINUS;
                if(light < newLight) light = newLight;
            } else {
                float newLight = down->getLight() - OPAQUE_LIGHT_MINUS;
                if(light < newLight) light = newLight;
            }
        }
    }
    if(up) { // 1 above
        if(up->getLight() > light) {
            if(up->isTransparent() || isTransparent()) {
                float newLight = up->getLight() - TRANSPARENT_LIGHT_MINUS;
                if(light < newLight) light = newLight;
            } else {
                float newLight = up->getLight() - OPAQUE_LIGHT_MINUS;
                if(light < newLight) light = newLight;
            }
        }
    }
    if(left) { // to the left
        if(left->getLight() > light) {
            if(left->isTransparent() || isTransparent()) {
                float newLight = left->getLight() - TRANSPARENT_LIGHT_MINUS;
                if(light < newLight) light = newLight;
            } else {
                float newLight = left->getLight() - OPAQUE_LIGHT_MINUS;
                if(light < newLight) light = newLight;
            }
        }
    }
    if(right) { // to the right
        if(right->getLight() > light) {
            if(right->isTransparent() || isTransparent()) {
                float newLight = right->getLight() - TRANSPARENT_LIGHT_MINUS;
                if(light < newLight) light = newLight;
            } else {
                float newLight = right->getLight() - OPAQUE_LIGHT_MINUS;
                if(light < newLight) light = newLight;
            }
        }
    }

    if(light < 0.01f) light = 0.0f;

    if(std::abs(m_lastLight - light) > 0.01f) {
        if(up) up->setToUpdate_light();
        if(down) down->setToUpdate_light();
        if(left) left->setToUpdate_light();
        if(right) right->setToUpdate_light();
    }

    return light;*/
//}*/

float Tile::getSurroundingHeat() {
    int x = (int)m_pos.x;
    int y = (int)m_pos.y;

    float temp = 0.0f;

    Tile* left =  m_parentChunk->getTile(x-1, y,   m_layer);
    Tile* right = m_parentChunk->getTile(x+1, y,   m_layer);
    Tile* up =    m_parentChunk->getTile(x,   y+1, m_layer);
    Tile* down =  m_parentChunk->getTile(x,   y-1, m_layer);

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

float Tile::getHeat() {
    float baseHeat = Category_Data::placeData[(unsigned int)m_parentChunk->getPlace()].baseTemperature;
    float highHeat = Category_Data::placeData[(unsigned int)m_parentChunk->getPlace()].maxTemp;

    float heat = getRawHeat();

    heat += m_sunLight * (highHeat - baseHeat);
    heat += baseHeat;

    return heat;
}

float Tile::getRawHeat() {
    return (m_emittedHeat > m_temperature ? m_emittedHeat : m_temperature);
}

void Tile::update(float time, bool updateLighting) {
    onUpdate(time);
}

void Tile::tick(float tickTime, float& sunlight) {
    if(m_updateHeat && m_id != (unsigned int)Categories::BlockIDs::AIR) {
        float heat = getSurroundingHeat();
        m_temperature = heat;
        m_updateHeat = false;
    }

    setSunlight(tickTime, sunlight);
    onTick(tickTime);
}

void Tile::draw(GLEngine::SpriteBatch& sb, int xOffset, int depthDifference) {

    if(m_draw) {
        if(m_textureId == (GLuint)-1) {
            loadTexture();
        }

        //GLint lightColourUniform = program.getUniformLocation("lightColour");
        //glUniform4fv(lightColourUniform, 3, &glm::vec3(0.0f, 1.0f, 0.0f)[0]);

        GLEngine::ColourRGBA8 colour = m_colour;
        if(depthDifference != 0) {
            if(depthDifference > 0) // in front of the player
                colour.a = 150; // 128
            colour.r = 64;
            colour.g = 64;
            colour.b = 64;
        }

        int r = m_colour.r, g = m_colour.g, b = m_colour.b;
        sb.draw(glm::vec4(m_pos.x + xOffset * CHUNK_SIZE, m_pos.y, m_size.x, m_size.y),
                glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                m_textureId,
                0.75f * (WORLD_DEPTH - m_layer),
                colour,
                glm::vec3(getLight()));
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
                glm::vec3(lightLevel));
    }
}

void Tile::destroy() {
    if(m_emittedLight > 0.0f) {
        resetNeighboursLight();
    }
    {
        m_parentChunk->getTile(m_pos.x, m_pos.y, m_layer)->setNeedsSunCheck();
        Tile* BR = m_parentChunk->getTile(m_pos.x+1, m_pos.y-1, m_layer);
        if(BR) BR->setNeedsSunCheck();
        Tile* BL = m_parentChunk->getTile(m_pos.x-1, m_pos.y-1, m_layer);
        if(BL) BL->setNeedsSunCheck();
    }
}

bool Tile::exposedToSun() {
    if(!m_transparent) {
        Tile* left = m_parentChunk->getTile(m_pos.x-1, m_pos.y, m_layer);
        if(left) {
            if(left->isExposedToSunlight() && left->isTransparent() && !left->needsSunCheck()) return true;
        }

        Tile* right = m_parentChunk->getTile(m_pos.x+1, m_pos.y, m_layer);
        if(right) {
            if(right->isExposedToSunlight() && right->isTransparent() && !right->needsSunCheck()) return true;
        }
    }
    for(int i = m_pos.y+1; i < WORLD_HEIGHT; i++) {
        if(!(m_parentChunk->getTile(m_pos.x, i, m_layer)->isTransparent())) {
            return false;
        }
    }
    return true;
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

void Tile::setSunlight(float& tickTime, float& sunLight) {
    if(m_needsSunCheck) { // check if this block is exposed to sunlight
        bool before = m_exposedToSun;
        m_exposedToSun = exposedToSun();
        if(m_exposedToSun != before) {
            Tile* down = m_parentChunk->getTile(m_pos.x, m_pos.y-1, m_layer);
            if(down) {
                down->setNeedsSunCheck();
            }
            if(!m_exposedToSun) {
                sunlight_resetNeighbour();
            }
        }
        m_needsSunCheck = false;
    }
    if(!m_exposedToSun) {
        if(m_sunLight > 0.0f) {
            setNeedsSunCheck();
        }
    } else {
        m_sunLight = sunLight;
        if(m_transparent) {
            float l = m_sunLight - SUNLIGHT_MINUS;
            sunlight_transferToNeighbour(l);
        } else {
            float l = m_sunLight - SUNLIGHT_MINUS;
            sunlight_transferLeft(l);
            sunlight_transferRight(l);
        }
    }
}

void Tile::resetNeighboursLight() {
    int range = std::floor(std::sqrt(m_emittedLight) * 4.0f);

    for(int y = -range; y < range; y++) {
        for(int x = -range; x < range; x++) {
            float dist = std::sqrt((x*x) + (y*y));
            float light = (1.0f - dist/(float)range) * m_emittedLight;
            if(light > 0.0f) m_parentChunk->getTile(m_pos.x + x, m_pos.y + y, m_layer)->addAmbientLight(-light);
        }
    }
}
#include <iostream>
void Tile::setNeighboursLight() {
    int range = std::floor(std::sqrt(std::abs(m_emittedLight)) * 4.0f);

    for(int y = -range; y < range; y++) {
        for(int x = -range; x < range; x++) {
            float dist = std::sqrt((x*x) + (y*y));
            float light = (1.0f - dist/(float)range) * m_emittedLight;
            if(light > 0.0f) m_parentChunk->getTile(m_pos.x + x, m_pos.y + y, m_layer)->addAmbientLight(light);
        }
    }
}
