#include "GameplayScreen.h"

#include "Entity.h"

#include <SDL/SDL_timer.h>

GameplayScreen::GameplayScreen(GLEngine::Window* window, WorldIOManager* WorldIOManager) : m_window(window), m_WorldIOManager(WorldIOManager)
{

}

GameplayScreen::~GameplayScreen()
{

}

int GameplayScreen::getNextScreenIndex() const {

}

int GameplayScreen::getPreviousScreenIndex() const {

}

void GameplayScreen::build() {

}

void GameplayScreen::destroy() {

}

void GameplayScreen::onEntry() {
    initShaders();

    m_spriteBatch.init();
    m_spriteFont.init("../Assets/GUI/fonts/QuietHorror.ttf", 96);

    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_camera.setPosition(m_camera.getPosition() + (glm::vec2(m_window->getScreenWidth() / 2, m_window->getScreenHeight() / 2)));
    m_uiCamera.init(m_window->getScreenWidth(), m_window->getScreenHeight());

    initUI();

    m_dr.init();

    m_worldManager.init(m_WorldIOManager);
}

void GameplayScreen::onExit() {

}

void GameplayScreen::update() {
    m_deltaTime = std::abs((60 / m_game->getFps()) + -1);
    m_deltaTime++;

    for(int i = 0; i < m_deltaTime; i++) {
        checkInput();

        m_camera.update();
        m_uiCamera.update();

        m_gui.update();

        m_camera.setPosition(m_worldManager.getPlayer()->getPosition() + m_worldManager.getPlayer()->getSize() / glm::vec2(2.0f));
        m_camera.setScale(m_scale);
    }

    m_worldManager.update(m_deltaTime);

    m_deltaTime = 0;

    m_time++; /// Change the increment if time is slowed or quicker (potion effects?)
}

void GameplayScreen::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.8f, 1.0f);

    m_textureProgram.use();

    GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);

    // Camera matrix
    glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
    GLint pUniform = m_textureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    m_spriteBatch.begin();

    m_worldManager.draw(m_spriteBatch, m_dr);

    m_spriteBatch.end();
    m_spriteBatch.renderBatch();

    m_dr.end();
    m_dr.render(projectionMatrix, 3);

    m_textureProgram.unuse();

    m_textureProgram.use();

    // Camera matrix
    projectionMatrix = m_uiCamera.getCameraMatrix();
    pUniform = m_textureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    m_spriteBatch.begin();

    char fpsStr[8];
    std::sprintf(fpsStr, "%s%i", "FPS: ", (int)m_game->getFps());

    m_spriteFont.draw(m_spriteBatch, fpsStr, glm::vec2(-m_window->getScreenWidth() / 2 + 50, m_window->getScreenHeight() / 2 - 50 - 96), glm::vec2(1), 0.0f, GLEngine::ColourRGBA8(255, 255, 255, 255));

    m_spriteBatch.end();
    m_spriteBatch.renderBatch();

    m_textureProgram.unuse();

    /// TODO: Don't forget to have a background image for the world based on biome
}

/// GameplayScreen PRIVATE FUNCTIONS

void GameplayScreen::checkInput() {
    SDL_Event evnt;
    while (SDL_PollEvent(&evnt)) {
        m_game->onSDLEvent(evnt);
        m_gui.onSDLEvent(evnt);
        switch(evnt.type) {
            case SDL_QUIT:
                m_currentState = GLEngine::ScreenState::EXIT_APPLICATION;
                break;
            case SDL_MOUSEBUTTONDOWN:

                break;
            case SDL_MOUSEWHEEL:
                scrollEvent(evnt);
                break;
        }
    }
}

void GameplayScreen::initShaders() {
    m_textureProgram.compileShaders("../Assets/Shaders/textureShading.vert", "../Assets/Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColour");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.linkShaders();
}

void GameplayScreen::initUI() {
    {
        m_gui.init("../Assets/GUI");
        m_gui.loadScheme("FOTDSkin.scheme");
        m_gui.setFont("QuietHorror-22");

        m_gui.setMouseCursor("FOTDSkin/MouseArrow");
        m_gui.showMouseCursor();
        SDL_ShowCursor(0);
    }
}

void GameplayScreen::scrollEvent(const SDL_Event& evnt) {
    m_scale += (float)evnt.wheel.y * m_scale / 10;
    if(m_scale <= MIN_ZOOM) {
        m_scale = MIN_ZOOM;
    } else if(m_scale >= MAX_ZOOM) {
        m_scale = MAX_ZOOM;
    }
}
