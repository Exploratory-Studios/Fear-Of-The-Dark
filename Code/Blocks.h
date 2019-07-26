#pragma once

#include "Block.h"
#include "../Categories.h"
#include "../Scripting/ScriptQueue.h"

#include <GLEngineErrors.h>
#include <string.h>

/// This file holds and handles all new TILES and TILE TYPES, not just blocks

class GenericBlock : public Block {
    public:
        GenericBlock(glm::vec2 pos, Chunk* parent, bool loadTexture = true) : Block(pos, parent) {}
        void onInteract(ScriptQueue* sq) {}

        virtual TileData getSaveData() const {
            TileData tile;
            tile.pos = getPosition();
            tile.id = getID();
            tile.ambientLight = getAmbientLight();
            return tile;
        }

    protected:
};

class BlockAir : public GenericBlock
{
    public:
        BlockAir(glm::vec2 pos, Chunk* parent, bool loadTexture = true);

    protected:
        void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/UNDEFINED.png").id; }
};

class BlockDirt : public GenericBlock
{
    public:
        BlockDirt(glm::vec2 pos, Chunk* parent, bool loadTexture = true);
        void onInteract(ScriptQueue* sq) {}
    protected:
        void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Blocks/Dirt.png").id; m_backdropTextureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Blocks/Dirt.png").id; }
        virtual void onTick(float& tickTime);
    private:
};

class BlockGrass : public GenericBlock
{
    public:
        BlockGrass(glm::vec2 pos, Chunk* parent, bool loadTexture = true);
        void onInteract(ScriptQueue* sq) {}
    protected:
        void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Blocks/Grass.png").id; m_backdropTextureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Blocks/Dirt.png").id; }
        virtual void onTick(float& tickTime);
    private:
};

class BlockTorch : public GenericBlock
{
    public:
        BlockTorch(glm::vec2 pos, Chunk* parent, bool loadTexture = true);
        void onInteract(ScriptQueue* sq) {}
    protected:
        void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Blocks/Torch.png").id; }

    private:
};

class BlockBush : public GenericBlock
{
    public:
        BlockBush(glm::vec2 pos, Chunk* parent, bool loadTexture = true);
        void onInteract(ScriptQueue* sq) {}
    protected:
        void loadTexture() override {
            int r = rand();
            if(r % 2 == 0) {
                m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/WhiteFlower.png").id;
            } else {
                m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/RedFlower.png").id;
            }
        }

    private:
};

class BlockStone : public GenericBlock
{
    public:
        BlockStone(glm::vec2 pos, Chunk* parent, bool loadTexture = true);
        void onInteract(ScriptQueue* sq) {}
    protected:
        void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Blocks/Stone.png").id; m_backdropTextureId = m_textureId; }

    private:
};

class BlockFoliage : public GenericBlock
{
    public:
        BlockFoliage(glm::vec2 pos, Chunk* parent, bool loadTexture = true);
        void onInteract(ScriptQueue* sq) {}
    protected:
        void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/BushGreen.png").id; }
        void onUpdate(float& time);
    private:
};

class BlockWood : public GenericBlock
{
    public:
        BlockWood(glm::vec2 pos, Chunk* parent, bool loadTexture = true);
        void onInteract(ScriptQueue* sq) {}
    protected:
        void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/WoodBlock.png").id; m_backdropTextureId = m_textureId; }

    private:
};

class BlockWater : public GenericBlock
{
    public:
        BlockWater(glm::vec2 pos, Chunk* parent, float level, bool loadTexture = true);
        void onInteract(ScriptQueue* sq) {}

        virtual void draw(GLEngine::SpriteBatch& sb, int xOffset) override;

        float getLevel() { return m_waterLevel; }
        void setLevel(float& level) { m_waterLevel = level; }

        virtual TileData getSaveData() const override {
            TileData tile;
            tile.pos = getPosition();
            tile.id = getID();
            tile.ambientLight = getAmbientLight();

            /*GenericData levelData;
            levelData.data = (void*)&m_waterLevel;
            levelData.type = DataType::FLOAT;
            tile.otherData.push_back(levelData);*/

            return tile;
        }

    protected:
        void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Blocks/Water.png").id; }
        void onUpdate(float& time) override;
    private:
        float m_waterLevel = 1.0f; // ranges from 1.0f (top of the block) to 0.0f (literally no water present)
};

static Block* createBlock(unsigned int id, glm::vec2 pos, Chunk* parent, bool loadTexture = true, float tickTime = -1.0f) {
    Block* ret = nullptr;
    switch(id) {
        case (unsigned int)Categories::BlockIDs::AIR: {
            ret = new BlockAir(pos, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::DIRT: {
            ret = new BlockDirt(pos, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::GRASS: {
            ret = new BlockGrass(pos, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::BUSH: {
            ret = new BlockBush(pos, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::TORCH: {
            ret = new BlockTorch(pos, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::FOLIAGE: {
            ret = new BlockFoliage(pos, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::STONE: {
            ret = new BlockStone(pos, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::WOOD: {
            ret = new BlockWood(pos, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::WATER: {
            ret = new BlockWater(pos, parent, 1.0f, loadTexture);
            break;
        }
        default: {
            GLEngine::fatalError("Tried to create block with improper id: " + id);
            break;
        }
    }
    if(tickTime != -1.0f) {
        ret->setSunlight(tickTime);
    }
    return ret;
}
