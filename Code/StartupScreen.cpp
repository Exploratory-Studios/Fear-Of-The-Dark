#include "StartupScreen.h"


/// Text FUNCTIONS

void Text::draw(GLEngine::SpriteFont& sf, GLEngine::SpriteBatch& sb, float time, int activeSplashScreen) {
    if(m_splashScreen == activeSplashScreen) {
        update(time);

        sf.draw(sb, m_text.c_str(), m_pos, m_textScale, 0.0f, m_colour, GLEngine::Justification::MIDDLE);
    }
}

void Text::update(float time) {
    if(time >= m_startTime&& time < m_endTime) {
        float curFade = (-(m_startTime + m_fadeTime - time) + m_fadeTime) / m_fadeTime; // time = 2, fadeTime = 1, startTime = 2, curFade = 1
        if(curFade > 1.0f) curFade = 1.0f;
        if(curFade < 0.0f) curFade = 0.0f;
        m_colour.a = curFade * 255.0f;
    } else if(time >= m_endTime) {
        float curFade = ((m_endTime + m_fadeTime - time)) / m_fadeTime;
        if(curFade > 1.0f) curFade = 1.0f;
        if(curFade < 0.0f) curFade = 0.0f;
        m_colour.a = curFade * 255.0f;
    }
}

/// Image FUNCTIONS

void Image::draw(GLEngine::SpriteBatch& sb, float time, int activeSplashScreen) {
    if(m_splashScreen == activeSplashScreen) {
        update(time);

        sb.draw(glm::vec4(m_pos.x - m_size.x * 0.5f, m_pos.y - m_size.y * 0.5f, m_size.x, m_size.y), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), GLEngine::ResourceManager::getTexture(m_filepath).id, 0.0f, m_colour);
    }
}

void Image::update(float time) {
    if(time >= m_startTime&& time < m_endTime) {
        float curFade = (-(m_startTime + m_fadeTime - time) + m_fadeTime) / m_fadeTime; // time = 2, fadeTime = 1, startTime = 2, curFade = 1
        if(curFade > 1.0f) curFade = 1.0f;
        if(curFade < 0.0f) curFade = 0.0f;
        m_colour.a = curFade * 255.0f;
    } else if(time >= m_endTime) {
        float curFade = ((m_endTime + m_fadeTime - time)) / m_fadeTime;
        if(curFade > 1.0f) curFade = 1.0f;
        if(curFade < 0.0f) curFade = 0.0f;
        m_colour.a = curFade * 255.0f;
    }
}

/// StartupScreen FUNCTIONS

void StartupScreen::build() {

}

void StartupScreen::destroy() {

}

void StartupScreen::onEntry() {
    initShaders();

    m_spriteBatch.init();
    m_spriteFont.init("../Assets/GUI/fonts/QuietHorror.ttf", 96);

    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_camera.setPosition(glm::vec2(m_window->getScreenWidth() / 2.0f, m_window->getScreenHeight() / 2.0f));
    m_uiCamera.init(m_window->getScreenWidth(), m_window->getScreenHeight());

    m_gui.init("../Assets/GUI");

    initAnimations();
    initUI();
}

void StartupScreen::onExit() {

}

void StartupScreen::update() {
    m_camera.update();
    m_uiCamera.update();
    m_gui.update();
    checkInput();
    m_time++;

    if(m_time >= 15.0f * 60.0f && m_splashScreen + 1.0f < SPLASHSCREENS_NUM) {
        m_splashScreenState = SplashScreenState::CHANGE_NEXT;
    } else if(m_splashScreen + 1.0f >= SPLASHSCREENS_NUM) {
        m_currentState = GLEngine::ScreenState::CHANGE_NEXT;
    }

    if(m_splashScreenState != SplashScreenState::NONE) {
        switch(m_splashScreenState) {
            case SplashScreenState::CHANGE_NEXT:
                m_splashScreenState = SplashScreenState::NONE;
                m_splashScreen++;
                m_time = 0.0f;
                break;
            default:
                break;
        }
    }
}

void StartupScreen::draw() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_uiTextureProgram.use();

    GLint textureUniform = m_uiTextureProgram.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);

    // Camera matrix
    glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
    GLint pUniform = m_uiTextureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    m_spriteBatch.begin();

    for(auto ATxt : m_textAnimations) ATxt.draw(m_spriteFont, m_spriteBatch, m_time, m_splashScreen);
    for(auto AImg : m_imageAnimations) AImg.draw(m_spriteBatch, m_time, m_splashScreen);

    m_spriteBatch.end();
    m_spriteBatch.renderBatch();

    m_uiTextureProgram.unuse();
}

/// StartupScreen PRIVATE FUNCTIONS

void StartupScreen::initShaders() {
    m_uiTextureProgram.compileShaders("../Assets/Shaders/uiShader.vert", "../Assets/Shaders/uiShader.frag");
    m_uiTextureProgram.addAttribute("vertexPosition");
    m_uiTextureProgram.addAttribute("vertexColour");
    m_uiTextureProgram.addAttribute("vertexUV");
    m_uiTextureProgram.linkShaders();
}

void StartupScreen::initUI() {
    SDL_ShowCursor(0);
}

void StartupScreen::initAnimations() {

    {
        m_textAnimations.emplace_back(glm::vec2(m_window->getScreenWidth() / 2.0, m_window->getScreenHeight() * 0.75), 2.0f, 6.0f, 3.0f, GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f), "An experience by", glm::vec2(1.0f, 1.0f), SPLASHSCREEN_STUDIO);
        m_textAnimations.emplace_back(glm::vec2(m_window->getScreenWidth() / 2.0, m_window->getScreenHeight() * 0.05), 6.0f, 10.0f, 3.0f, GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f), "Exploratory Studios", glm::vec2(1.25f, 1.25f), SPLASHSCREEN_STUDIO);

        m_imageAnimations.emplace_back(glm::vec2(m_window->getScreenWidth() * 0.5f, m_window->getScreenHeight() * 0.5f), 2.0f, 10.0f, 3.0f, GLEngine::ColourRGBA8(200.0f, 200.0f, 200.0f, 255.0f), "../Assets/Textures/StartupScreens/Studio/Logo.png", glm::vec2(200.0f, 200.0f), SPLASHSCREEN_STUDIO);
    }

    {
        m_textAnimations.emplace_back(glm::vec2(m_window->getScreenWidth() / 2.0, m_window->getScreenHeight() * 0.75), 2.0f, 6.0f, 3.0f, GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f), "This game uses", glm::vec2(1.0f, 1.0f), SPLASHSCREEN_OPENGL_SDL);
        m_textAnimations.emplace_back(glm::vec2(m_window->getScreenWidth() / 2.0, m_window->getScreenHeight() * 0.05), 6.0f, 10.0f, 3.0f, GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f), "OpenGL 3.0 & SDL 2.0", glm::vec2(1.0f, 1.0f), SPLASHSCREEN_OPENGL_SDL);
    }



}

void StartupScreen::checkInput() {
    SDL_Event evnt;
    while (SDL_PollEvent(&evnt)) {
        m_game->onSDLEvent(evnt);
        m_gui.onSDLEvent(evnt);
        switch(evnt.type) {
            case SDL_QUIT:
                m_currentState = GLEngine::ScreenState::EXIT_APPLICATION;
                break;
            case SDL_KEYUP:
                m_splashScreenState = SplashScreenState::CHANGE_NEXT;
                break;
        }
    }
}
