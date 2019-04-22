#pragma once

#include "../Block.h"
#include "../Categories.h"

#include <GLEngineErrors.h>
#include <string.h>

/// This file holds and handles all new TILES and TILE TYPES, not just blocks

class BlockAir : public Block
{
    public:
        BlockAir(glm::vec2 pos, Chunk* parent);
        void onInteract(Player& player) {}
    protected:

    private:
};

class BlockDirt : public Block
{
    public:
        BlockDirt(glm::vec2 pos, Chunk* parent);
        void onInteract(Player& player) {}
    protected:

    private:
};

class BlockGrass : public Block
{
    public:
        BlockGrass(glm::vec2 pos, Chunk* parent);
        void onInteract(Player& player) {}
    protected:

    private:
};

class BlockTorch : public Block
{
    public:
        BlockTorch(glm::vec2 pos, Chunk* parent);
        void onInteract(Player& player) {}
    protected:

    private:
};

class BlockBush : public Block
{
    public:
        BlockBush(glm::vec2 pos, Chunk* parent);
        void onInteract(Player& player) {}
    protected:

    private:
};

static Block* createBlock(unsigned int id, glm::vec2 pos, Chunk* parent) {
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
        default: {
            GLEngine::fatalError("Tried to create block with improper id: " + id);
            break;
        }
    }
    return ret;
}
