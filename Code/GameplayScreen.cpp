#include "GameplayScreen.h"

#include "Entity.h"

#include <SDL2/SDL_timer.h>

#define DEV_CONTROLS

#ifdef DEV_CONTROLS
#include <iostream>
#include <regex>
#endif // DEV_CONTROLS

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
    m_spriteFont.init("../Assets/GUI/fonts/Amatic-Bold.ttf", 96);

    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_camera.setPosition(m_camera.getPosition() + (glm::vec2(m_window->getScreenWidth() / 2, m_window->getScreenHeight() / 2)));

    m_uiCamera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_uiCamera.setPosition(m_uiCamera.getPosition() + (glm::vec2(m_window->getScreenWidth() / 2, m_window->getScreenHeight() / 2)));

    m_dr.init();

    m_worldManager.init(m_WorldIOManager);

    initUI();


}

void GameplayScreen::onExit() {

}

void GameplayScreen::update() {
    m_deltaTime = std::abs((60 / m_game->getFps()) + -1);
    m_deltaTime++;

    checkInput();

    m_camera.update();
    m_uiCamera.update();

    m_gui.update();

    m_camera.setPosition(m_lastPlayerPos + m_worldManager.getPlayer()->getSize() / glm::vec2(2.0f));
    m_lastPlayerPos = m_lastPlayerPos + (m_worldManager.getPlayer()->getPosition() - m_lastPlayerPos) / glm::vec2(4.0f);

    m_camera.setScale(m_scale);

    m_worldManager.update(&m_camera, m_deltaTime, m_time);

    m_time++; /// Change the increment if time is slowed or quicker (potion effects?)
    m_frame++;

    if((int)m_frame++ % (int)(60 / m_tickRate) == 0) { // m_frame is equal to current frame
        tick();
    }

}

void GameplayScreen::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.8f, 1.0f);

    {
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



        m_textureProgram.unuse();
    }

    {
        m_uiTextureProgram.use();

        GLint textureUniform = m_uiTextureProgram.getUniformLocation("mySampler");
        glUniform1i(textureUniform, 0);
        glActiveTexture(GL_TEXTURE0);

        // Camera matrix
        glm::mat4 projectionMatrix = m_uiCamera.getCameraMatrix();
        GLint pUniform = m_uiTextureProgram.getUniformLocation("P");
        glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

        m_worldManager.getPlayer()->drawGUI(m_spriteBatch, m_spriteFont);

        m_gui.draw();

        m_dr.end();
        m_dr.render(projectionMatrix, 3);

        m_uiTextureProgram.unuse();
    }

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

    #ifdef DEV_CONTROLS
    if(m_game->inputManager.isKeyPressed(SDLK_BACKSLASH)) {
        std::string in;
        std::getline(std::cin, in);

        std::regex fullCmd(".+(\\s(\\d+))*");
        std::smatch matches;
        if(std::regex_match(in, matches, fullCmd)) {
            int i = 0;
            std::string cmd;
            while(i < in.length()) {
                if(in[i] == ' ' || in[i] == '\0') {
                    break;
                }
                cmd += in[i];
                i++;
            }
            int argNum;
            if(cmd == "time") {
                argNum = 1;
            } else if(cmd == "give") {
                argNum = 2;
            } else if(cmd == "getTime") {
                argNum = 0;
            } else if(cmd == "tickRate") {
                argNum = 1;
            }

            int args[argNum];
            for(int k = 0; k < argNum; k++) {
                int j = i+1;
                std::string arg;
                while(true) {
                    arg += in[j];
                    if(in[j] == ' ' || j == in.length()-1) {
                        i = j;
                        args[k] = std::stoi(arg);
                        break;
                    }
                    j++;
                }
            }
            if(cmd == "time") {
                m_tickTime = (float)args[0];
            } else if (cmd == "give") {
            } else if(cmd == "getTime") {
                std::cout << std::endl << "Time: " << m_tickTime;
            } else if(cmd == "tickRate") {
                m_tickRate = (float)args[0];
            }
        }
    }
    #endif // DEV_CONTROLS

}

void GameplayScreen::initShaders() {
    m_textureProgram.compileShaders("../Assets/Shaders/textureShading.vert", "../Assets/Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColour");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.linkShaders();

    m_uiTextureProgram.compileShaders("../Assets/Shaders/uiShader.vert", "../Assets/Shaders/uiShader.frag");
    m_uiTextureProgram.addAttribute("vertexPosition");
    m_uiTextureProgram.addAttribute("vertexColour");
    m_uiTextureProgram.addAttribute("vertexUV");
    m_uiTextureProgram.linkShaders();
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

void GameplayScreen::tick() {
    m_worldManager.tick(m_tickTime);
    m_tickTime++;
}

void GameplayScreen::scrollEvent(const SDL_Event& evnt) {
    m_scale += (float)evnt.wheel.y * m_scale / 10.0f;
    if(m_scale < MIN_ZOOM) {
        m_scale = MIN_ZOOM;
    } else if(m_scale > MAX_ZOOM) {
        m_scale = MAX_ZOOM;
    }
}
