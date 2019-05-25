#pragma once

#include <glm/glm.hpp>
#include <Vertex.h>
#include <ResourceManager.h>
#include <GLTexture.h>

#include "Tile.h"
#include "Categories.h"
#include "Scripting/ScriptQueue.h"

class Block : public Tile
{
    public:
        Block(glm::vec2 pos, Chunk* parent);
        virtual ~Block() {}

        virtual void onInteract(ScriptQueue* sq) = 0; // Class already has parent chunk

    protected:
        virtual void onTick(float& tickTime) { }
        virtual void onUpdate(float& time) { }
        /// Protected, taken from "Tile" class
        /*
        glm::vec2 m_pos;
        glm::vec2 m_size = glm::vec2(TILE_SIZE, TILE_SIZE);

        GLEngine::GLTexture m_texture;
        GLEngine::ColourRGBA8 m_colour = GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f);

        unsigned int m_id;

        m_solid
        m_transparent
        m_parentChunk (*)
        */
};

#include "Blocks.h"
