#pragma once

#include "../Block.h"
#include "../Categories.h"
#include "../Scripting/ScriptQueue.h"

#include <GLEngineErrors.h>
#include <string.h>

/// This file holds and handles all new TILES and TILE TYPES, not just blocks

class BlockAir : public Block
{
    public:
        BlockAir(glm::vec2 pos, Chunk* parent);
        void onInteract(ScriptQueue* sq) {}
    protected:

    private:
};

class BlockDirt : public Block
{
    public:
        BlockDirt(glm::vec2 pos, Chunk* parent);
        void onInteract(ScriptQueue* sq) {}
    protected:
        virtual void onTick(float& tickTime);
    private:
};

class BlockGrass : public Block
{
    public:
        BlockGrass(glm::vec2 pos, Chunk* parent);
        void onInteract(ScriptQueue* sq) {}
    protected:
        virtual void onTick(float& tickTime);
    private:
};

class BlockTorch : public Block
{
    public:
        BlockTorch(glm::vec2 pos, Chunk* parent);
        void onInteract(ScriptQueue* sq) {}
    protected:

    private:
};

class BlockBush : public Block
{
    public:
        BlockBush(glm::vec2 pos, Chunk* parent);
        void onInteract(ScriptQueue* sq) {}
    protected:

    private:
};

class BlockStone : public Block
{
    public:
        BlockStone(glm::vec2 pos, Chunk* parent);
        void onInteract(ScriptQueue* sq) {}
    protected:

    private:
};

class BlockFoliage : public Block
{
    public:
        BlockFoliage(glm::vec2 pos, Chunk* parent);
        void onInteract(ScriptQueue* sq) {}
    protected:
        void onUpdate(float& time);
    private:
};

class BlockWood : public Block
{
    public:
        BlockWood(glm::vec2 pos, Chunk* parent);
        void onInteract(ScriptQueue* sq) {}
    protected:

    private:
};

static Block* createBlock(unsigned int id, glm::vec2 pos, Chunk* parent, float tickTime = -1.0f) {
    Block* ret = nullptr;
    switch(id) {
        case (unsigned int)Categories::BlockIDs::AIR: {
            ret = new BlockAir(pos, parent);
            break;
        }
        case (unsigned int)Categories::BlockIDs::DIRT: {
            ret = new BlockDirt(pos, parent);
            break;
        }
        case (unsigned int)Categories::BlockIDs::GRASS: {
            ret = new BlockGrass(pos, parent);
            break;
        }
        case (unsigned int)Categories::BlockIDs::BUSH: {
            ret = new BlockBush(pos, parent);
            break;
        }
        case (unsigned int)Categories::BlockIDs::TORCH: {
            ret = new BlockTorch(pos, parent);
            break;
        }
        case (unsigned int)Categories::BlockIDs::FOLIAGE: {
            ret = new BlockFoliage(pos, parent);
            break;
        }
        case (unsigned int)Categories::BlockIDs::STONE: {
            ret = new BlockStone(pos, parent);
            break;
        }
        case (unsigned int)Categories::BlockIDs::WOOD: {
            ret = new BlockWood(pos, parent);
            break;
        }
        default: {
            GLEngine::fatalError("Tried to create block with improper id: " + id);
            break;
        }
    }
    if(tickTime > -1.0f) {
        ret->setSunlight(tickTime);
    }
    return ret;
}
