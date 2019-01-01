#include "OptionsMenuScreen.h"

OptionsMenuScreen::OptionsMenuScreen(GLEngine::Window* window) : m_window(window)
{
    //ctor
}

OptionsMenuScreen::~OptionsMenuScreen()
{
    //dtor
}

int OptionsMenuScreen::getNextScreenIndex() const {
    return SCREEN_INDEX_NO_SCREEN;
}

int OptionsMenuScreen::getPreviousScreenIndex() const {
    return SCREEN_INDEX_MAINMENU;
}

void OptionsMenuScreen::build() {

}

void OptionsMenuScreen::destroy() {

}

void OptionsMenuScreen::onEntry() {
    initShaders();

    m_spriteBatch.init();
    m_spriteFont.init("../Assets/GUI/fonts/QuietHorror.ttf", 96);

    m_uiCamera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_uiCamera.setPosition(glm::vec2(m_window->getScreenWidth() / 2.0f, m_window->getScreenHeight() / 2.0f));

    initUI();
}

void OptionsMenuScreen::onExit() {

}

void OptionsMenuScreen::update() {
    checkInput();
    m_gui.update();
    m_uiCamera.update();

    m_time++;
}

void OptionsMenuScreen::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    float a = m_time;
    if(a > 255.0f) a = 255.0f;

    m_backButton->setAlpha(a);
    m_gui.draw();
}

void OptionsMenuScreen::initUI() {
    {
        m_gui.init("../Assets/GUI");
        m_gui.loadScheme("VanillaSkin.scheme");
        m_gui.loadScheme("FOTDSkin.scheme");

        m_gui.setFont("QuietHorror-42");

        m_gui.setMouseCursor("FOTDSkin/MouseArrow");
        m_gui.showMouseCursor();
        SDL_ShowCursor(0);
    }

    m_backButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("FOTDSkin/Button", glm::vec4(0.1f, 0.8f, 0.30f, 0.1f), glm::vec4(0.0f), "BackButtonOptionsMenu"));
    m_backButton->subscribeEvent(CEGUI::PushButton::EventClicked,
                                            CEGUI::Event::Subscriber(&OptionsMenuScreen::EventBackButtonClicked, this));
    m_backButton->setText("[padding='l:0 t:15 r:0 b:0']Back");
}

void OptionsMenuScreen::initShaders() {
    m_uiTextureProgram.compileShaders("../Assets/Shaders/uiShader.vert", "../Assets/Shaders/uiShader.frag");
    m_uiTextureProgram.addAttribute("vertexPosition");
    m_uiTextureProgram.addAttribute("vertexColour");
    m_uiTextureProgram.addAttribute("vertexUV");
    m_uiTextureProgram.linkShaders();
}

void OptionsMenuScreen::checkInput() {
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

void OptionsMenuScreen::updateMousebuttonDown(SDL_Event& evnt) {
    switch(evnt.button.button) {
        case SDL_BUTTON_LEFT:
            break;
        case SDL_BUTTON_RIGHT:
            break;
        case SDL_BUTTON_MIDDLE:
            break;
    }
}
bool OptionsMenuScreen::EventBackButtonClicked(const CEGUI::EventArgs& e) {
    m_currentState = GLEngine::ScreenState::CHANGE_PREVIOUS;
}

