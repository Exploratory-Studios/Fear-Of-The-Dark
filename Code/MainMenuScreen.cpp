#include "MainMenuScreen.h"

/// SCREEN FUNCTIONS

MainMenuScreen::MainMenuScreen(GLEngine::Window* window) : m_window(window)
{
    //ctor
}

MainMenuScreen::~MainMenuScreen()
{
    //dtor
}

int MainMenuScreen::getNextScreenIndex() const {
    return m_nextScreenIndex;
}

int MainMenuScreen::getPreviousScreenIndex() const {
    return SCREEN_INDEX_NO_SCREEN;
}

void MainMenuScreen::build() {

}

void MainMenuScreen::destroy() {

}

void MainMenuScreen::onEntry() {
    initShaders();

    m_spriteBatch.init();
    m_spriteFont.init("../Assets/GUI/fonts/QuietHorror.ttf", 96);

    m_uiCamera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_uiCamera.setPosition(glm::vec2(m_window->getScreenWidth() / 2.0f, m_window->getScreenHeight() / 2.0f));

    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_camera.setPosition(glm::vec2(m_window->getScreenWidth() / 2.0f, m_window->getScreenHeight() / 2.0f));

    initUI();

    addBackgroundImage(m_backgroundImages, "Texture1.png");
    addBackgroundImage(m_backgroundImages, "Texture2.png");
    addBackgroundImage(m_backgroundImages, "Texture3.png");
}

void addBackgroundImage(std::vector<GLEngine::GLTexture>& backgroundImages, std::string filename) {
    backgroundImages.emplace_back(GLEngine::ResourceManager::getTexture("../Assets/Textures/MainMenuBackgrounds/" + filename));
}

void MainMenuScreen::onExit() {

}

void MainMenuScreen::update() {
    checkInput();
    m_gui.update();
    m_uiCamera.update();
    m_camera.update();

    m_time++;
}

void MainMenuScreen::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    float a = m_time;
    if(a > 255.0f) a = 255.0f;

    {
        m_uiTextureProgram.use();

        GLint textureUniform = m_uiTextureProgram.getUniformLocation("mySampler");
        glUniform1i(textureUniform, 0);
        glActiveTexture(GL_TEXTURE0);

        // Camera matrix
        glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
        GLint pUniform = m_uiTextureProgram.getUniformLocation("P");
        glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

        m_spriteBatch.begin();

        if(m_currentBackgroundImage != (int)(m_time / (m_backgroundChangeTime * 60.0f) + 1.0f - 0.5f) % (int)m_backgroundImages.size()) {
            if(m_backgroundImageColour.a > 0.0f && (int)m_time % 2 == 0) {
                m_backgroundImageColour.a--; // 1.0
            }
        } else if(m_backgroundImageColour.a < 35.0f && (int)m_time % 2 == 0) {
            m_backgroundImageColour.a++;
        }

        /*if(a != 255.0f) {
            m_backgroundImageColour.a = a;
        }*/

        m_currentBackgroundImage = (int)(m_time / (m_backgroundChangeTime * 60.0f)) % (int)m_backgroundImages.size();

        m_spriteBatch.draw(glm::vec4(0.0f, 0.0f,
                                     m_window->getScreenWidth(),
                                     m_window->getScreenHeight()),
                                     glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                                     m_backgroundImages[m_currentBackgroundImage].id,
                                     0.0f,
                                     m_backgroundImageColour);

        m_spriteBatch.end();
        m_spriteBatch.renderBatch();

        m_uiTextureProgram.unuse();
    }

    m_playButton->setAlpha(a);
    m_optionsButton->setAlpha(a);
    m_exitButton->setAlpha(a);
    m_gui.draw();

    {
        m_uiTextureProgram.use();

        GLint textureUniform = m_uiTextureProgram.getUniformLocation("mySampler");
        glUniform1i(textureUniform, 0);
        glActiveTexture(GL_TEXTURE0);

        // Camera matrix
        glm::mat4 projectionMatrix = m_uiCamera.getCameraMatrix();
        GLint pUniform = m_uiTextureProgram.getUniformLocation("P");
        glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

        m_spriteBatch.begin();

        for(auto l : m_labels) l.draw(m_spriteFont, m_spriteBatch, GLEngine::ColourRGBA8(l.getColour().r, l.getColour().g, l.getColour().b, a));

        m_spriteBatch.end();
        m_spriteBatch.renderBatch();

        m_uiTextureProgram.unuse();
    }
}

/// PRIVATE FUNCTIONS

void MainMenuScreen::initUI() {
    {
        m_gui.init("../Assets/GUI");
        m_gui.loadScheme("VanillaSkin.scheme");
        m_gui.loadScheme("FOTDSkin.scheme");

        m_gui.setFont("Amatic-22");

        m_gui.setMouseCursor("FOTDSkin/MouseArrow");
        m_gui.showMouseCursor();
        SDL_ShowCursor(0);
    }

        m_labels.emplace_back("Fear Of", glm::vec2(1.6f, 1.6f), glm::vec2(0.2f, 0.3f), GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f), m_window);
        m_labels.emplace_back("The Dark", glm::vec2(1.6f, 1.6f), glm::vec2(0.3f, 0.45f), GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f), m_window);

    {

        const float PADDING = 0.15f;

        m_playButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("FOTDSkin/Button", glm::vec4(0.1f, 0.5f, 0.30f, 0.1f), glm::vec4(0.0f), "PlayButtonMainMenu"));
        m_playButton->subscribeEvent(CEGUI::PushButton::EventClicked,
                                                CEGUI::Event::Subscriber(&MainMenuScreen::EventPlayButtonClicked, this));
        m_labels.emplace_back("Play", glm::vec2(1.2f, 0.8f), m_playButton, GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f), m_window);


        m_optionsButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("FOTDSkin/Button", glm::vec4(0.1f, 0.5f + PADDING, 0.30f, 0.1f), glm::vec4(0.0f), "OptionsButtonMainMenu"));
        m_optionsButton->subscribeEvent(CEGUI::PushButton::EventClicked,
                                                CEGUI::Event::Subscriber(&MainMenuScreen::EventOptionsButtonClicked, this));
        m_labels.emplace_back("Options", glm::vec2(1.2f, 0.8f), m_optionsButton, GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f), m_window);


        m_exitButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("FOTDSkin/Button", glm::vec4(0.1f, 0.5f + PADDING * 2.0f, 0.30f, 0.1f), glm::vec4(0.0f), "ExitButtonMainMenu"));
        m_exitButton->subscribeEvent(CEGUI::PushButton::EventClicked,
                                                CEGUI::Event::Subscriber(&MainMenuScreen::EventExitButtonClicked, this));
        m_labels.emplace_back("Exit", glm::vec2(1.2f, 0.8f), m_exitButton, GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f), m_window);
    }

}

void MainMenuScreen::initShaders() {
    m_uiTextureProgram.compileShaders("../Assets/Shaders/uiShader.vert", "../Assets/Shaders/uiShader.frag");
    m_uiTextureProgram.addAttribute("vertexPosition");
    m_uiTextureProgram.addAttribute("vertexColour");
    m_uiTextureProgram.addAttribute("vertexUV");
    m_uiTextureProgram.linkShaders();
}

void MainMenuScreen::checkInput() {
    SDL_Event evnt;
    while (SDL_PollEvent(&evnt)) {
        m_game->onSDLEvent(evnt);
        m_gui.onSDLEvent(evnt);
        switch(evnt.type) {
            case SDL_QUIT:
                m_currentState = GLEngine::ScreenState::EXIT_APPLICATION;
                break;

            case SDL_MOUSEBUTTONDOWN:
                updateMousebuttonDown(evnt);
                break;
        }
    }
}

void MainMenuScreen::updateMousebuttonDown(SDL_Event& evnt) {
    switch(evnt.button.button) {
        case SDL_BUTTON_LEFT:
            break;
        case SDL_BUTTON_RIGHT:
            break;
        case SDL_BUTTON_MIDDLE:
            break;
    }
}

/// EVENT FUNCTIONS (GUI)

bool MainMenuScreen::EventPlayButtonClicked(const CEGUI::EventArgs& e) {
    m_nextScreenIndex = SCREEN_INDEX_LOAD;
    m_currentState = GLEngine::ScreenState::CHANGE_NEXT;
    return true;
}

bool MainMenuScreen::EventOptionsButtonClicked(const CEGUI::EventArgs& e) {
    m_nextScreenIndex = SCREEN_INDEX_OPTIONSMENU;
    m_currentState = GLEngine::ScreenState::CHANGE_NEXT;
    return true;
}

bool MainMenuScreen::EventExitButtonClicked(const CEGUI::EventArgs& e) {
    m_currentState = GLEngine::ScreenState::EXIT_APPLICATION;
    return true;
}
