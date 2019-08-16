#include "Tile.h"

#include <random>

#include "WorldIOManager.h"
#include "Chunk.h"

Tile::Tile() {
    m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/UNDEFINED.png").id;
    m_parentChunk = nullptr;
}

float Tile::getLight() {
    if(m_ambientLight > m_sunLight && m_ambientLight > m_emittedLight) {
        return m_ambientLight;
    } else if(m_sunLight > m_ambientLight && m_sunLight > m_emittedLight) {
        return m_sunLight;
    } else if(m_emittedLight > m_sunLight && m_emittedLight > m_ambientLight) {
        return m_emittedLight;
    }
    return m_ambientLight;
}

float Tile::getSurroundingLight() {
    int x = (int)m_pos.x;
    int y = (int)m_pos.y;

    float light = 0.0f;

    Tile* left =  m_parentChunk->getTile(x-1, y,   m_layer);
    Tile* right = m_parentChunk->getTile(x+1, y,   m_layer);
    Tile* up =    m_parentChunk->getTile(x,   y+1, m_layer);
    Tile* down =  m_parentChunk->getTile(x,   y-1, m_layer);

    if(down) { // 1 below
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

    if(light < 0.0f) light = 0.0f;

    if(std::abs(m_lastLight - light) > 0.01f) {
        if(up) up->setToUpdate_light();
        if(down) down->setToUpdate_light();
        if(left) left->setToUpdate_light();
        if(right) right->setToUpdate_light();
    }

    return light;
}

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
    if(m_updateLight && updateLighting) {
        float light = getSurroundingLight();
        setAmbientLight(light);
        m_lastLight = light;
        m_updateLight = false;
    }
    onUpdate(time);
}

void Tile::tick(float tickTime) {
    if(m_updateHeat && m_id != (unsigned int)Categories::BlockIDs::AIR) {
        float heat = getSurroundingHeat();
        m_temperature = heat;
    }

    setSunlight(tickTime);
    onTick(tickTime);
}

void Tile::draw(GLEngine::SpriteBatch& sb, int xOffset) {

    if(m_draw) {
        if(m_textureId == (GLuint)-1) {
            loadTexture();
        }

        //GLint lightColourUniform = program.getUniformLocation("lightColour");
        //glUniform4fv(lightColourUniform, 3, &glm::vec3(0.0f, 1.0f, 0.0f)[0]);

        int r = m_colour.r, g = m_colour.g, b = m_colour.b;
        sb.draw(glm::vec4(m_pos.x + xOffset * CHUNK_SIZE, m_pos.y, m_size.x, m_size.y),
                glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                m_textureId,
                0.75f * (WORLD_DEPTH - m_layer),
                GLEngine::ColourRGBA8(r, g, b, m_colour.a),
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

bool Tile::exposedToSun() {
    for(int i = WORLD_HEIGHT-1; i > m_pos.y; i--) { /// TODO: make this more efficient: From the block up?
        if(!m_parentChunk->getTile(m_pos.x, i, m_layer)->isTransparent()) {
            return false;
        }
    }
    return true;
}

TileData Tile::getSaveData() {
    TileData d;
    d.pos = m_pos;
    d.id = m_id;
    d.ambientLight = m_ambientLight;
    delete d.metaData;
    d.metaData = getMetaData();
    d.lowerLayer = new TileData(m_parentChunk->getTile(m_pos.x, m_pos.y, m_layer+1)->getSaveData());
    return d;
}
