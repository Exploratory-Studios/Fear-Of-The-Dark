#pragma once

#include <ResourceManager.h>

class Animation {
    /**
        This class:
         - Acts as a state machine. It stores the correct drawing UV and textureID, as well as the current "frame" and length.
         - Provides getters for proper UV and texture ID.
         - Has an update function to step current frame forward.
    **/
    public:
        Animation() {};
        Animation(GLEngine::GLTexture spriteSheetTexture, unsigned int xSize, unsigned int ySize, unsigned int x, unsigned int y, unsigned int length);
        ~Animation();

        void update();

        glm::vec2 getUV();
        unsigned int getTexture();

    private:
        GLEngine::GLTexture m_texture; // Holds width/height, as well as texture ID
        unsigned int m_xSize, m_ySize, m_x, m_y;
        unsigned int m_length;
        unsigned int m_curFrame;
};

class SpriteSheet
{
    /**
        This class:
         - Keeps a reference to all smartly constructed Animation objects in a map with names. (One for running, walking, flying, etc.)
         - Provides copies of Animation objects
         - Provides a constructor that utilizes a constant global resolution and constant placement of animations. This will mean that we'll only need the path, each animation's name, start position, entity size and length.
         - One should be created per entity/block/item TYPE. (one for all Steve entities, one for all Joe entities)
    **/
    public:
        SpriteSheet(std::string filepath); // Calls init
        SpriteSheet();
        ~SpriteSheet();

        void init(std::string filepath); // Loads texture, holding ID

        void constructAnimation(glm::vec2 entitySize, unsigned int x, unsigned int y, unsigned int length, std::string name); // Constructs an animation based on the x, y, and length, and stores it with name `name`

        Animation getAnimation(std::string name); // Gets a copy of an animation.
    private:
        GLEngine::GLTexture m_texture;
        std::unordered_map<std::string, Animation> m_animations;
};

class SpriteSheetCache {
    /**
        This class makes sure duplicate SpriteSheets aren't constructed
    **/
};
