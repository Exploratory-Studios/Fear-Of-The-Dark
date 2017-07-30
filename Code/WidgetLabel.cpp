#include "WidgetLabel.h"

/// WIDGETLABEL FUNCTIONS

void WidgetLabel::draw(GLEngine::SpriteFont& spriteFont, GLEngine::SpriteBatch& spriteBatch, GLEngine::ColourRGBA8 colour) {
    spriteFont.draw(spriteBatch, m_text.c_str(), m_position, m_scale, 0.0f, colour, GLEngine::Justification::MIDDLE);
}
