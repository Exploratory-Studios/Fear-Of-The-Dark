#include "StartupScreen.h"

#include <GLContextManager.hpp>

#include "Options.h"

/// Text FUNCTIONS

void Text::draw(BARE2D::FontRenderer*  fontRenderer,
				BARE2D::BasicRenderer* renderer,
				float				   time,
				int					   activeSplashScreen) {
	if(m_splashScreen == activeSplashScreen) {
		update(time);
		/// TODO: Load font.
		//fontRenderer.draw(renderer, m_text.c_str(), m_pos, m_textScale, 0.0f, m_colour, BARE2D::Justification::MIDDLE);
	}
}

void Text::update(float time) {
	if(time >= m_startTime && time < m_endTime) {
		float curFade = (-(m_startTime + m_fadeTime - time) + m_fadeTime) /
						m_fadeTime; // time = 2, fadeTime = 1, startTime = 2, curFade = 1
		if(curFade > 1.0f)
			curFade = 1.0f;
		if(curFade < 0.0f)
			curFade = 0.0f;
		m_colour.a = curFade * 255.0f;
	} else if(time >= m_endTime) {
		float curFade = ((m_endTime + m_fadeTime - time)) / m_fadeTime;
		if(curFade > 1.0f)
			curFade = 1.0f;
		if(curFade < 0.0f)
			curFade = 0.0f;
		m_colour.a = curFade * 255.0f;
	}
}

/// Image FUNCTIONS

void Image::draw(BARE2D::BasicRenderer* renderer, float time, int activeSplashScreen) {
	if(m_splashScreen == activeSplashScreen) {
		update(time);

		renderer->draw(glm::vec4(m_pos.x - m_size.x * 0.5f, m_pos.y - m_size.y * 0.5f, m_size.x, m_size.y),
					   glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
					   BARE2D::ResourceManager::loadTexture(m_filepath).id,
					   0.0f,
					   m_colour);
	}
}

void Image::update(float time) {
	if(time >= m_startTime && time < m_endTime) {
		float curFade = (-(m_startTime + m_fadeTime - time) + m_fadeTime) /
						m_fadeTime; // time = 2, fadeTime = 1, startTime = 2, curFade = 1
		if(curFade > 1.0f)
			curFade = 1.0f;
		if(curFade < 0.0f)
			curFade = 0.0f;
		m_colour.a = curFade * 255.0f;
	} else if(time >= m_endTime) {
		float curFade = ((m_endTime + m_fadeTime - time)) / m_fadeTime;
		if(curFade > 1.0f)
			curFade = 1.0f;
		if(curFade < 0.0f)
			curFade = 0.0f;
		m_colour.a = curFade * 255.0f;
	}
}

/// StartupScreen FUNCTIONS

void StartupScreen::initScreen() {
}

void StartupScreen::destroyScreen() {
}

void StartupScreen::onEntry() {
	Options::loadFromFile("Options.bin"); // Will never cause an issue; it just creates a file if one doesn't exist

	std::string fragShader = ASSETS_FOLDER_PATH + "Shaders/textureShader.frag";
	std::string vertShader = ASSETS_FOLDER_PATH + "Shaders/textureShader.vert";

	m_renderer = new BARE2D::BasicRenderer(fragShader, vertShader);
	m_renderer->init();

	m_fontRenderer = new BARE2D::FontRenderer(fragShader, vertShader);
	m_fontRenderer->init();

	std::string guiPath = ASSETS_FOLDER_PATH + "GUI";
	BARE2D::BARECEGUI::getInstance()->init(guiPath, 1);

	initAnimations();
	initUI();
}

void StartupScreen::onExit() {
}

void StartupScreen::update(double dt) {
	checkInput();
	m_time++;

	if(m_time >= 15.0f * 60.0f && m_splashScreen + 1.0f < SPLASHSCREENS_NUM) {
		m_splashScreenState = SplashScreenState::CHANGE_NEXT;
	} else if(m_splashScreen + 1.0f >= SPLASHSCREENS_NUM) {
		m_screenState = BARE2D::ScreenState::CHANGE_NEXT;
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

	m_renderer->begin();

	for(auto ATxt: m_textAnimations)
		ATxt.draw(m_fontRenderer, m_renderer, m_time, m_splashScreen);
	for(auto AImg: m_imageAnimations)
		AImg.draw(m_renderer, m_time, m_splashScreen);

	m_renderer->end();
	m_renderer->render();
}

/// StartupScreen PRIVATE FUNCTIONS

void StartupScreen::initUI() {
	SDL_ShowCursor(0);
}

void StartupScreen::initAnimations() {
	{
		m_textAnimations.emplace_back(glm::vec2(m_window->getWidth() / 2.0, m_window->getHeight() * 0.75),
									  2.0f,
									  6.0f,
									  3.0f,
									  BARE2D::Colour(255.0f, 255.0f, 255.0f, 255.0f),
									  "An experience by",
									  glm::vec2(1.0f, 1.0f),
									  SPLASHSCREEN_STUDIO);
		m_textAnimations.emplace_back(glm::vec2(m_window->getWidth() / 2.0, m_window->getHeight() * 0.05),
									  6.0f,
									  10.0f,
									  3.0f,
									  BARE2D::Colour(255.0f, 255.0f, 255.0f, 255.0f),
									  "Exploratory Studios",
									  glm::vec2(1.25f, 1.25f),
									  SPLASHSCREEN_STUDIO);

		m_imageAnimations.emplace_back(glm::vec2(m_window->getWidth() * 0.5f, m_window->getHeight() * 0.5f),
									   2.0f,
									   10.0f,
									   3.0f,
									   BARE2D::Colour(200.0f, 200.0f, 200.0f, 255.0f),
									   ASSETS_FOLDER_PATH + "Textures/StartupScreens/Studio/Logo.png",
									   glm::vec2(200.0f, 200.0f),
									   SPLASHSCREEN_STUDIO);
	}

	{
		m_textAnimations.emplace_back(glm::vec2(m_window->getWidth() / 2.0, m_window->getHeight() * 0.75),
									  2.0f,
									  6.0f,
									  3.0f,
									  BARE2D::Colour(255.0f, 255.0f, 255.0f, 255.0f),
									  "This game uses",
									  glm::vec2(1.0f, 1.0f),
									  SPLASHSCREEN_OPENGL_SDL);
		m_textAnimations.emplace_back(glm::vec2(m_window->getWidth() / 2.0, m_window->getHeight() * 0.05),
									  6.0f,
									  10.0f,
									  3.0f,
									  BARE2D::Colour(255.0f, 255.0f, 255.0f, 255.0f),
									  "OpenGL 3.0 & SDL 2.0",
									  glm::vec2(1.0f, 1.0f),
									  SPLASHSCREEN_OPENGL_SDL);
	}
}

void StartupScreen::checkInput() {
	if(m_input->isKeyPressed(SDLK_SPACE)) {
		m_splashScreenState = SplashScreenState::CHANGE_NEXT;
	}
}
