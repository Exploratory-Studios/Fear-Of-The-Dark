#include "Tile.h"

#include "World.h"

#include <random>
#include <boost/thread.hpp>
#include <ResourceManager.h>

#include "XMLData.h"

Tile::Tile() {

}

Tile::Tile(glm::vec2 pos, unsigned int layer, unsigned int id, MetaData data, bool loadTex) : m_pos(pos), m_layer(layer), m_id(id), m_metaData(data) {
    XML_TileData t = XMLData::getTileData(id);

    m_texturePath = t.textureFilepath;
    m_bumpMapPath = t.bumpMapFilepath;
    m_emittedLight = t.emittedLight;
    m_emittedHeat = t.emittedHeat;
    m_size = t.size;
    m_solid = t.solid;
    m_draw = t.drawn;
    m_natural = t.natural;
    m_transparent = t.transparent;
    m_updateScriptID = t.updateScriptID;
    m_tickScriptID = t.tickScriptID;
    m_interactScriptID_walkedOn = t.interactScriptID_walkedOn;
    m_interactScriptID_used = t.interactScriptID_used;

    if(loadTex && m_draw) {
        loadTexture();
    } else {
        m_textureId = (GLuint)-1;
    }
}

Tile::Tile(glm::vec2 pos, unsigned int layer, TileIDs id, MetaData data, bool loadTex) : m_pos(pos), m_layer(layer), m_id((unsigned int)id), m_metaData(data) {
    XML_TileData t = XMLData::getTileData((unsigned int)id);

    m_texturePath = t.textureFilepath;
    m_bumpMapPath = t.bumpMapFilepath;
    m_emittedLight = t.emittedLight;
    m_emittedLight = t.emittedHeat;
    m_size = t.size;
    m_solid = t.solid;
    m_draw = t.drawn;
    m_natural = t.natural;
    m_transparent = t.transparent;
    m_updateScriptID = t.updateScriptID;
    m_tickScriptID = t.tickScriptID;
    m_destroyScriptID = t.destructionScriptID;
    m_interactScriptID_walkedOn = t.interactScriptID_walkedOn;
    m_interactScriptID_used = t.interactScriptID_used;

    if(loadTex && m_draw) {
        loadTexture();
    } else {
        m_textureId = (GLuint)-1;
    }
}

void Tile::initParticles(GLEngine::ParticleEngine2D* engine) {
    /*std::string fp = ASSETS_FOLDER_PATH + "/Textures/Blocks/Dirt.png";
    std::string bm = ASSETS_FOLDER_PATH + "/Textures/BumpMaps/Dirt.png";
    //m_walkParticleBatch = engine->getParticleBatch(MAX_TYPE_PARTICLES, 0.01f, fp, bm, updateParticle);*/ /// TODO: Move particle system to scripting system
}

void Tile::loadTexture() {
    m_textureId = GLEngine::ResourceManager::getTexture(m_texturePath).id;
    m_bumpMapId = GLEngine::ResourceManager::getTexture(m_bumpMapPath).id;
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
    if(m_emittedLight > 0.0f) {
        if(!m_setLighting) {
            setNeighboursLight(world);
            m_setLighting = true;
        }
    }

    if(updateLighting) {
        if(m_needsSunCheck) {
            resetSunlightCorners();

            m_needsSunCheck = false;
            m_exposedToSun = exposedToSun(world);

            calculateSunlight(world, sunlight);
        }
    }

    onUpdate();
}

void Tile::tick(World* world, float tickTime, const float& sunlight) {
    if(m_updateHeat && m_id != (unsigned int)TileIDs::AIR) {
        float heat = getSurroundingHeat(world);
        m_temperature = heat;
        m_updateHeat = false;
    }

    calculateSunlight(world, sunlight);

    onTick();
}

void Tile::draw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, int xOffset, int depthDifference) {

    if(m_draw) {
        if(m_textureId == (GLuint)-1 || m_bumpMapId == (GLuint)-1) {
            loadTexture();
        }

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
        float depth = 0.4f * (WORLD_DEPTH - m_layer);

        sb.draw(pos,
                glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                m_textureId,
                m_bumpMapId,
                depth,
                colour,
                /*m_cornerLight + */m_cornerSunlight);

        onDraw(sb, sf, pos, depth);
    }
}

float Tile::getLightAtPoint(glm::vec2 posFromBL) {
    float TL = m_cornerLight.x + m_cornerSunlight.x;
    float TR = m_cornerLight.y + m_cornerSunlight.y;
    float BR = m_cornerLight.z + m_cornerSunlight.z;
    float BL = m_cornerLight.w + m_cornerSunlight.w;

    glm::vec2 relCoords = posFromBL / m_size;

    // Cosine interpolation from TL-TR (x)
    float TL_TR_X;
    {
        float mu = (1.0 - cos(relCoords.x * 3.141592)) / 2.0;
        TL_TR_X = (TL * (1.0 - mu) + TR * mu);
   	}

   	// Cosine interpolation from BL-BR (x)
   	float BL_BR_X;
    {
        float mu = (1.0 - cos(relCoords.x * 3.141592)) / 2.0;
        BL_BR_X = (BL * (1.0 - mu) + BR * mu);
   	}

   	// Cosine interpolation from each of those, using the y coord
   	float light;
    {
        float mu = (1.0 - cos(relCoords.y * 3.141592)) / 2.0;
        light = (TL_TR_X * (1.0 - mu) + BL_BR_X * mu);
   	}

   	return light;
}

void Tile::destroy(World* world) {
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
                    float dist = std::sqrt((x+0.5f)*(x+0.5f) + (y+0.5f)*(y+0.5f) + (layer*layer));
                    float light = (1.0f - dist/(float)range) * m_emittedLight;

                    if(light > 0.0f) {
                        t->addAmbientLight(-light);
                    }
                    { // Bottom left corner (x, y)
                        float d = std::sqrt((x)*(x) + (y)*(y) + (layer*layer));
                        BL = ((1.0f - d/(float)(range)) * m_emittedLight); // x + 0, y + 0
                    }
                    { // Bottom right corner (x+1, y)
                        float d = std::sqrt((x+1.0f)*(x+1.0f) + (y)*(y) + (layer*layer));
                        BR = ((1.0f - d/(float)(range)) * m_emittedLight); // x + 0, y + 0
                    }
                    { // top left corner (x, y+1)
                        float d = std::sqrt((x)*(x) + (y+1.0f)*(y+1.0f) + (layer*layer));
                        TL = ((1.0f - d/(float)(range)) * m_emittedLight); // x + 0, y + 0
                    }
                    { // top right corner (x+1, y+1)
                        float d = std::sqrt((x+1.0f)*(x+1.0f) + (y+1.0f)*(y+1.0f) + (layer*layer));
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
                    float dist = std::sqrt((x+0.5f)*(x+0.5f) + (y+0.5f)*(y+0.5f) + (layer*layer));
                    float light = (1.0f - dist/(float)range) * m_emittedLight;

                    if(light > 0.0f) {
                        t->addAmbientLight(light);
                    }
                    { // Bottom left corner (x, y)
                        float d = std::sqrt((x)*(x) + (y)*(y) + (layer*layer));
                        BL = ((1.0f - d/(float)(range)) * m_emittedLight); // x + 0, y + 0
                    }
                    { // Bottom right corner (x+1, y)
                        float d = std::sqrt((x+1.0f)*(x+1.0f) + (y)*(y) + (layer*layer));
                        BR = ((1.0f - d/(float)(range)) * m_emittedLight); // x + 0, y + 0
                    }
                    { // top left corner (x, y+1)
                        float d = std::sqrt((x)*(x) + (y+1.0f)*(y+1.0f) + (layer*layer));
                        TL = ((1.0f - d/(float)(range)) * m_emittedLight); // x + 0, y + 0
                    }
                    { // top right corner (x+1, y+1)
                        float d = std::sqrt((x+1.0f)*(x+1.0f) + (y+1.0f)*(y+1.0f) + (layer*layer));
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
