#include "MainScreen.h"

MainScreen::MainScreen(GLEngine::Window* window) {
}

MainScreen::~MainScreen() {
}

void MainScreen::build() {
	initUI();
	initShaders();
}

void MainScreen::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
}

void MainScreen::update() {
	checkInput();
}

void MainScreen::checkInput() {
	SDL_Event evnt;
	while(SDL_PollEvent(&evnt)) {
		m_game->onSDLEvent(evnt);
		//m_gui.onSDLEvent(evnt);
		switch(evnt.type) {
			case SDL_QUIT:
				m_currentState = GLEngine::ScreenState::EXIT_APPLICATION;
				break;

			case SDL_MOUSEBUTTONDOWN:
				//updateMousebuttonDown(evnt);
				break;
		}
	}
}

void MainScreen::initUI() {

}

void MainScreen::initShaders() {

}
