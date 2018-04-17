#include "ScreenIndices.h"

#include <IGameScreen.h>
#include <Window.h>
#include <Camera2D.h>
#include <GUI.h>
#include <SpriteBatch.h>
#include <GLSLProgram.h>
#include <SpriteFont.h>
#include <Vertex.h>
#include <ResourceManager.h>

#define SPLASHSCREEN_STUDIO 0
#define SPLASHSCREEN_OPENGL_SDL 1
#define SPLASHSCREEN_CEGUI 2
#define SPLASHSCREEN_TITLE 3
#define SPLASHSCREENS_NUM 4

enum class SplashScreenState {
    CHANGE_NEXT,
    NONE
};

class Text {
    public:
        Text(glm::vec2 pos,
             float startTime,
             float endTime,
             float fadeTime,
             GLEngine::ColourRGBA8 colour,
             std::string text,
             glm::vec2 textScale,
             int splashScreen) : m_pos(pos),
                                     m_startTime(startTime*60.0f),
                                     m_endTime(endTime*60.0f),
                                     m_fadeTime(fadeTime*60.0f),
                                     m_colour(GLEngine::ColourRGBA8(colour.r, colour.g, colour.b, 0.0f)),
                                     m_text(text),
                                     m_textScale(textScale),
                                     m_splashScreen(splashScreen) { }

        void draw(GLEngine::SpriteFont& sf, GLEngine::SpriteBatch& sb, float time, int activeSplashScreen);
        void update(float time);

    private:
        glm::vec2 m_pos;
        float m_startTime;
        float m_endTime;
        float m_fadeTime;
        GLEngine::ColourRGBA8 m_colour;
        std::string m_text;
        glm::vec2 m_textScale;
        int m_splashScreen;

};

class Image {
    public:
        Image(glm::vec2 pos,
             float startTime,
             float endTime,
             float fadeTime,
             GLEngine::ColourRGBA8 colour,
             std::string filepath,
             glm::vec2 size,
             int splashScreen) : m_pos(pos),
                                     m_startTime(startTime*60.0f),
                                     m_endTime(endTime*60.0f),
                                     m_fadeTime(fadeTime*60.0f),
                                     m_colour(GLEngine::ColourRGBA8(colour.r, colour.g, colour.b, 0.0f)),
                                     m_filepath(filepath),
                                     m_size(size),
                                     m_splashScreen(splashScreen) { }

        void draw(GLEngine::SpriteBatch& sb, float time, int activeSplashScreen);
        void update(float time);

    private:
        glm::vec2 m_pos;
        float m_startTime;
        float m_endTime;
        float m_fadeTime;
        GLEngine::ColourRGBA8 m_colour;
        std::string m_filepath;
        glm::vec2 m_size;
        int m_splashScreen;

};

class StartupScreen : public GLEngine::IGameScreen
{
    public:
        StartupScreen(GLEngine::Window* window) : m_window(window) { }
        virtual ~StartupScreen() { }

        virtual int getNextScreenIndex() const override { return SCREEN_INDEX_MAINMENU; }
        virtual int getPreviousScreenIndex() const override { return SCREEN_INDEX_NO_SCREEN; }
        virtual void build() override;
        virtual void destroy() override;
        virtual void onEntry() override;
        virtual void onExit() override;
        virtual void update() override;
        virtual void draw() override;

    private:
        void initShaders();
        void initUI();
        void initAnimations();

        void checkInput();

        std::vector<Text> m_textAnimations;
        std::vector<Image> m_imageAnimations;

        float m_time = 0.0f;

        SplashScreenState m_splashScreenState = SplashScreenState::NONE;
        int m_splashScreen = SPLASHSCREEN_STUDIO;

        GLEngine::Camera2D m_camera;
        GLEngine::Camera2D m_uiCamera;
        GLEngine::Window* m_window;
        GLEngine::GUI m_gui;
        GLEngine::SpriteBatch m_spriteBatch;
        GLEngine::GLSLProgram m_uiTextureProgram;
        GLEngine::SpriteFont m_spriteFont;
};
