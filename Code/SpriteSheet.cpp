#include "SpriteSheet.h"

Animation::Animation(GLEngine::GLTexture spriteSheetTexture, unsigned int xSize, unsigned int ySize, unsigned int x, unsigned int y, unsigned int length)
    : m_texture(spriteSheetTexture),
      m_xSize(xSize),
      m_ySize(ySize),
      m_x(x),
      m_y(y),
      m_length(length)
{
    m_curFrame = 0; // Just to make sure. Because I'm paranoid like that.
}

Animation::~Animation() {

}

void Animation::update() {
    m_curFrame++;
    m_curFrame = m_curFrame % m_length;
}

glm::vec4 Animation::getUV() {
    // 0.0 is left (x = 0) and 1.0 is right (x = texture width)
    // 0.0 is bottom (y = 0) and 1.0 is top (y = texture height)

    // All we need to do is map current X, Y to UV coord system (0-texture width/height -> 0.0-1.0)
    // Then the z and w components are simply m_x/ySize divided by the texture size. EZ

    // Map current X/Y
    float UV_X = (float)(m_x + m_xSize * m_curFrame) / m_texture.width;
    float UV_Y = (float)m_y / m_texture.height;

    // Map size to 0.0-1.0
    float UV_W = (float)m_xSize / m_texture.width;
    float UV_H = (float)m_ySize / m_texture.height;

    return glm::vec4(UV_X, UV_Y, UV_W, UV_H);
}

unsigned int Animation::getTexture() {
    return m_texture.id;
}

SpriteSheet::SpriteSheet(std::string filepath) {
    init(filepath);
}

SpriteSheet::SpriteSheet() {

}

SpriteSheet::~SpriteSheet() {

}

void SpriteSheet::init(std::string filepath) {
    /// Loads texture and stores it.
    m_texture = GLEngine::ResourceManager::getTexture(filepath); // Literally that easy.
}

void SpriteSheet::constructAnimation(glm::vec2 entitySize, unsigned int x, unsigned int y, unsigned int length, std::string name) {
    // We must use the PresetValues.h's BLOCK_RESOLUTION macro as the number of pixels per entitySize unit
    // 1. Create the Animation object
    Animation animation(m_texture, entitySize.x * BLOCK_RESOLUTION, entitySize.y * BLOCK_RESOLUTION, x, y, length);

    // 2. Add it to the map
    m_animations[name] = animation;

    // Done!
}

Animation SpriteSheet::getAnimation(std::string name) {
    //lookup the animation and see if it's in the map
    auto mit = m_animations.find(name);

    //check if its not in the map
    if (mit == m_animations.end()) {
        // It's not. Return something and Log it!
        Logger::getInstance()->log("ERROR: Animation could not be loaded with name: " + name, true);

        Animation a();
        return a;
    }
    return mit->second;
}

        Animation getAnimation(std::string name); // Gets a copy of an animation.
