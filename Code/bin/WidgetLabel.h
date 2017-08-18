#pragma once

#include <string>
#include <glm/glm.hpp>
#include <Vertex.h>
#include <CEGUI/CEGUI.h>
#include <Window.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>

class WidgetLabel {
    public:
        WidgetLabel(const std::string& text, glm::vec2 scale, CEGUI::Window* widget, GLEngine::ColourRGBA8 colour, GLEngine::Window* window) : m_text(text), m_scale(scale), m_colour(colour), m_window(window) {
            m_position.x = (widget->getXPosition().d_scale + widget->getWidth().d_scale / 2) * m_window->getScreenWidth();
            m_position.y = m_window->getScreenHeight() + -(widget->getYPosition().d_scale + widget->getHeight().d_scale * 1.5 /* ((m_scale.y * 96) / (widget->getHeight().d_scale * m_window->getScreenHeight()))*/) * m_window->getScreenHeight();// / 10.0f); // This end part just centres it on the y axis                                                                                                                                                                    /// Note: This means all fonts must be of resolution 96
        } /// TODO: Work these goddamn maths out on a whiteboard!!!1!11
        WidgetLabel(const std::string& text, glm::vec2 scale, glm::vec2 pos, GLEngine::ColourRGBA8 colour, GLEngine::Window* window) : m_text(text), m_scale(scale), m_colour(colour), m_window(window) {
            m_position.x = pos.x * m_window->getScreenWidth();
            m_position.y = m_window->getScreenHeight() + -(pos.y * m_window->getScreenHeight());
        }
        WidgetLabel(const std::string& text, glm::vec2 scale, CEGUI::Window* widget, GLEngine::ColourRGBA8 colour, GLEngine::Window* window, CEGUI::Window* parent) : m_text(text), m_scale(scale), m_colour(colour), m_window(window), m_parent(parent) {
            m_position.x = (widget->getXPosition().d_scale + widget->getWidth().d_scale / 2.0f) * m_window->getScreenWidth() * (m_parent->getXPosition().d_scale + m_parent->getWidth().d_scale / 2.0f);
            m_position.y = m_window->getScreenHeight() + -(widget->getYPosition().d_scale + widget->getHeight().d_scale) * m_window->getScreenHeight() * (m_parent->getYPosition().d_scale + m_parent->getHeight().d_scale / 2.0f) + -(96.0f / m_scale.y) + 96;// / 10.0f); // This end part just centres it on the y axis
        }
        void draw(GLEngine::SpriteFont& spriteFont, GLEngine::SpriteBatch& spriteBatch, GLEngine::ColourRGBA8 colour);
        void setColour(GLEngine::ColourRGBA8 colour) { m_colour = colour; }

        std::string getText() { return m_text; }
        GLEngine::ColourRGBA8 getColour() { return m_colour; }

    private:
        std::string m_text;
        glm::vec2 m_scale;
        GLEngine::ColourRGBA8 m_colour;
        glm::vec2 m_position;
        GLEngine::Window* m_window = nullptr;
        CEGUI::Window* m_parent = nullptr;
};
