#include "WaitScreen.h"

#include <GLContextManager.h>

WaitScreen::WaitScreen(GLEngine::Window* window, WorldIOManager* WorldIOManager) :
	m_window(window), m_worldIOManager(WorldIOManager) {
}

WaitScreen::~WaitScreen() {
	//dtor
}

int WaitScreen::getNextScreenIndex() const {
	return m_nextScreenIndex;
}

int WaitScreen::getPreviousScreenIndex() const {
	return SCREEN_INDEX_NO_SCREEN;
}

void WaitScreen::build() {
}

void WaitScreen::destroy() {
}

void WaitScreen::onEntry() {
	initShaders();

	m_spriteBatch.init();
	m_spriteFont.init((ASSETS_FOLDER_PATH + "GUI/fonts/QuietHorror.ttf").c_str(), 96);

	m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_camera.setPosition(glm::vec2(m_window->getScreenWidth() / 2.0f, m_window->getScreenHeight() / 2.0f));

	m_uiCamera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_uiCamera.setPosition(glm::vec2(m_window->getScreenWidth() / 2.0f, m_window->getScreenHeight() / 2.0f));

	initUI();
}

void WaitScreen::onExit() {
}

void WaitScreen::update() {
	checkInput();

	m_uiCamera.update();

	m_gui.update();

	m_time++;

	m_progressBar->setProgress(m_worldIOManager->getProgress());
	m_messageBox->setText(m_worldIOManager->getMessage());

	if(m_worldIOManager->getProgress() >= 1.0f) {
		m_currentState = GLEngine::ScreenState::CHANGE_NEXT;
	}
}

void WaitScreen::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	m_textureProgram.use();

	GLint textureUniform = m_textureProgram.getUniformLocation("textureSampler");
	glUniform1i(textureUniform, 0);
	GLEngine::GLContextManager::getGLContext()->setActiveTexture(GL_TEXTURE0);

	// Camera matrix
	glm::mat4 projectionMatrix = m_uiCamera.getCameraMatrix();
	GLint	  pUniform		   = m_textureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	m_spriteBatch.begin();

	float alpha = 0.0f;

	if(m_time <= 255.0f / 2.0f) {
		alpha = m_time * 2.0f;
	} else {
		alpha = 255.0f;
	}

	m_progressBar->setAlpha(alpha / 255);

	m_gui.draw();

	m_spriteBatch.end();
	m_spriteBatch.renderBatch();

	m_textureProgram.unuse();
}

void WaitScreen::checkInput() {
	SDL_Event evnt;
	while(SDL_PollEvent(&evnt)) {
		m_game->onSDLEvent(evnt);
		m_gui.onSDLEvent(evnt);
		switch(evnt.type) {
			case SDL_QUIT:
				m_currentState = GLEngine::ScreenState::EXIT_APPLICATION;
				break;
		}
	}
}

void WaitScreen::initUI() {
	{
		m_gui.init(ASSETS_FOLDER_PATH + "GUI", 1);
		m_gui.loadScheme("FOTDSkin.scheme");

		m_gui.setFont("QuietHorror-30");

		m_gui.setMouseCursor("FOTDSkin/MouseArrow");
		m_gui.showMouseCursor();
		SDL_ShowCursor(0);
	}

	{
		m_progressBar = static_cast<CEGUI::ProgressBar*>(m_gui.createWidget("FOTDSkin/ProgressBar",
																			glm::vec4(0.1f, 0.1f, 0.8f, 0.1f),
																			glm::vec4(0.0f),
																			"WaitScreenProgressBar"));
		m_progressBar->setText("[padding='l:0 t:15 r:0 b:0']Progress: ");
		m_progressBar->setFont("QuietHorror-42");

		m_messageBox = static_cast<CEGUI::DefaultWindow*>(m_gui.createWidget("FOTDSkin/Label",
																			 glm::vec4(0.1f, 0.2f, 0.8f, 0.2f),
																			 glm::vec4(0.0f),
																			 "WaitScreenWaitMessage"));
		m_messageBox->setText(m_worldIOManager->getMessage());
	}
}

void WaitScreen::initShaders() {
	m_textureProgram.compileShaders(ASSETS_FOLDER_PATH + "Shaders/textureShader.vert",
									ASSETS_FOLDER_PATH + "Shaders/textureShader.frag");
	m_textureProgram.addAttribute("vertexPosition");
	m_textureProgram.addAttribute("vertexColour");
	m_textureProgram.addAttribute("vertexUV");
	m_textureProgram.linkShaders();
}
