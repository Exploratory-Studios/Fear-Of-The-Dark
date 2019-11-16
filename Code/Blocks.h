#pragma once

#include "Block.h"
#include "../Categories.h"
#include "../Scripting/ScriptQueue.h"

#include <GLEngineErrors.h>
#include <string.h>

class Inventory;

/// This file holds and handles all new TILES and TILE TYPES, not just blocks

class GenericBlock : public Block {
    public:
        GenericBlock(glm::vec2 pos, unsigned int layer, Chunk* parent, MetaData metaData, bool loadTexture = true) : Block(pos, layer, parent, metaData) { }
        void onInteract(ScriptQueue* sq) {}

    protected:
        virtual void handleMetaDataInit(MetaData& data) override {}
};

class InteractableBlock : public Block {
    public:
        InteractableBlock(glm::vec2 pos, unsigned int layer, Chunk* parent, MetaData metaData, bool loadTexture = true) : Block(pos, layer, parent, metaData) {}
        void onInteract(ScriptQueue* sq) { if(m_interactScriptId != -1) sq->activateScript(m_interactScriptId); }

    protected:
        virtual void handleMetaDataInit(MetaData& data) override {}
        unsigned int m_interactScriptId = -1;
};

 /// Interactable Blocks ################################################
    class InventoryBlock : public InteractableBlock {
        public:
            InventoryBlock(glm::vec2 pos, unsigned int layer, Chunk* parent, MetaData metaData, bool loadTexture = true);
            ~InventoryBlock() { delete m_inventory; }

            void onDraw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, glm::vec4& pos, float& depth) override;

            void showInventory(bool show);
        protected:
            Inventory* m_inventory;
            bool m_showInventory = false;
    };

    class BlockSign : public InteractableBlock {
        public:
            BlockSign(glm::vec2 pos, unsigned int layer, Chunk* parent, MetaData metaData, bool loadTexture = true);



        protected:
            void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Wood_Sign.png").id; }

            std::string m_text = "UNDEF";
            bool m_changingText = false; // If this is true, capture the player's input and focus here? Maybe use CEGUI here...
            bool m_showText = false;
    };



 /// Generic Blocks #####################################################

    class LightBlock : public GenericBlock {
        public:
            LightBlock(glm::vec2 pos, unsigned int layer, Chunk* parent, MetaData metaData, float emittedLight, bool loadTexture = true) : GenericBlock(pos, layer, parent, metaData, loadTexture) {
                m_emittedLight = emittedLight;
                setLightUpdate();
            }

            void setLightUpdate() { m_updateLight = true; }

        protected:
            virtual void onUpdate(float& time) override;
            bool m_updateLight = false;
    };

    class BlockAir : public GenericBlock
    {
        public:
            BlockAir(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture = true);

        protected:
            void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/UNDEFINED.png").id; }
    };

    class BlockDirt : public GenericBlock
    {
        public:
            BlockDirt(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture = true);
            void onInteract(ScriptQueue* sq) {}
        protected:
            void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Blocks/Dirt.png").id; m_backdropTextureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Blocks/Dirt.png").id; }
            virtual void onTick(float& tickTime);
        private:
    };

    class BlockGrass : public GenericBlock
    {
        public:
            BlockGrass(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture = true);
            void onInteract(ScriptQueue* sq) {}
        protected:
            void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Blocks/Grass.png").id; m_backdropTextureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Blocks/Dirt.png").id; }
            virtual void onTick(float& tickTime);
        private:
    };

    class BlockTorch : public LightBlock
    {
        public:
            BlockTorch(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture = true);
            void onInteract(ScriptQueue* sq) {}
        protected:
            void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Blocks/Torch.png").id; }
    };

    class BlockTorchBright : public BlockTorch
    {
        public:
            BlockTorchBright(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture = true);
    };

    class BlockTorchAnti : public BlockTorch
    {
        public:
            BlockTorchAnti(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture = true);
    };

    class BlockBush : public GenericBlock
    {
        public:
            BlockBush(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture = true);
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
            BlockStone(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture = true);
            void onInteract(ScriptQueue* sq) {}
        protected:
            void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Blocks/Stone.png").id; m_backdropTextureId = m_textureId; }

        private:
    };

    class BlockFoliage : public GenericBlock
    {
        public:
            BlockFoliage(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture = true);
            void onInteract(ScriptQueue* sq) {}
        protected:
            void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/BushGreen.png").id; }
            void onUpdate(float& time);
        private:
    };

    class BlockWood : public GenericBlock
    {
        public:
            BlockWood(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture = true);
            void onInteract(ScriptQueue* sq) {}
        protected:
            void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/WoodBlock.png").id; m_backdropTextureId = m_textureId; }

        private:
    };

    class BlockWater : public GenericBlock
    {
        public:
            BlockWater(glm::vec2 pos, unsigned int layer, Chunk* parent, float level, MetaData metaData = MetaData(), bool loadTexture = true);
            void onInteract(ScriptQueue* sq) {}

            virtual void draw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, int xOffset, int depthDifference) override;

            float getLevel() { return m_waterLevel; }
            void setLevel(float& level) { m_waterLevel = level; }

        protected:
            virtual void handleMetaDataInit(MetaData& data) override {
                data.getAspect("level", m_waterLevel);
            }
            virtual MetaData* getMetaData() override {
                MetaData* m = new MetaData{MetaData_Aspect("level", m_waterLevel)};
                return m;
            }

            void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Blocks/Water.png").id; }
            void onUpdate(float& time) override;
        private:
            float m_waterLevel = 1.0f; // ranges from 1.0f (top of the block) to 0.0f (literally no water present)
    };

    class BlockPole : public GenericBlock
    {
        public:
            BlockPole(glm::vec2 pos, unsigned int layer, Chunk* parent, MetaData metaData = MetaData(), bool loadTexture = true);
            void onInteract(ScriptQueue* sq) {}

            void loadTexture() override { m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Blocks/Wood_Pole.png").id; }
    };



static Block* createBlock(unsigned int id, glm::vec2 pos, unsigned int layer, Chunk* parent, MetaData metaData = MetaData{}, bool loadTexture = true, float tickTime = -1.0f) {
    Block* ret = nullptr;
    switch(id) {
        case (unsigned int)Categories::BlockIDs::AIR: {
            ret = new BlockAir(pos, layer, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::DIRT: {
            ret = new BlockDirt(pos, layer, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::GRASS: {
            ret = new BlockGrass(pos, layer, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::BUSH: {
            ret = new BlockBush(pos, layer, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::TORCH: {
            ret = new BlockTorch(pos, layer, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::FOLIAGE: {
            ret = new BlockFoliage(pos, layer, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::STONE: {
            ret = new BlockStone(pos, layer, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::WOOD: {
            ret = new BlockWood(pos, layer, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::WATER: {
            ret = new BlockWater(pos, layer, parent, 1.0f, metaData, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::WOOD_POLE: {
            ret = new BlockPole(pos, layer, parent, metaData, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::TORCH_BRIGHT: {
            ret = new BlockTorchBright(pos, layer, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::TORCH_ANTI: {
            ret = new BlockTorchAnti(pos, layer, parent, loadTexture);
            break;
        }
        case (unsigned int)Categories::BlockIDs::SIGN_WOOD: {
            ret = new BlockSign(pos, layer, parent, metaData, loadTexture);
            break;
        }
        default: {
            GLEngine::fatalError("Tried to create block with improper id: " + id);
            break;
        }
    }
    if(tickTime != -1.0f) {
        float sunlight = std::cos(tickTime / (DAY_LENGTH / 6.28318f)) / 2.0f + 0.5f;
        ret->setSunlight(tickTime, sunlight);
    }
    return ret;
}
