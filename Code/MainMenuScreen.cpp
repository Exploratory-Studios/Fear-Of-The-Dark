#include "MainMenuScreen.h"

#include <GLContextManager.hpp>

#include <XMLDataManager.hpp>

/// SCREEN FUNCTIONS

MainMenuScreen::MainMenuScreen(BARE2D::Window* window) : m_window(window) {
	//ctor
}

MainMenuScreen::~MainMenuScreen() {
	//dtor
}

int MainMenuScreen::getNextScreenIndex() const {
	return m_nextScreenIndex;
}

void MainMenuScreen::initScreen() {
}

void MainMenuScreen::destroyScreen() {
}

void MainMenuScreen::onEntry() {
	std::string fragShader = ASSETS_FOLDER_PATH + "Shaders/textureShader.frag";
	std::string vertShader = ASSETS_FOLDER_PATH + "Shaders/textureShader.vert";

	m_renderer = new BARE2D::BasicRenderer(fragShader, vertShader);
	m_renderer->init();

	m_fontRenderer = new BARE2D::FontRenderer(fragShader, vertShader);
	m_fontRenderer->init();

	initUI();

	addBackgroundImage(m_backgroundImages, "Texture1.png");
	addBackgroundImage(m_backgroundImages, "Texture2.png");
	addBackgroundImage(m_backgroundImages, "Texture3.png");
	//}

	m_time = 0.0f;
}

void addBackgroundImage(std::vector<BARE2D::Texture>& backgroundImages, std::string filename) {
	std::string file = ASSETS_FOLDER_PATH + "Textures/MainMenuBackgrounds/" + filename;
	backgroundImages.emplace_back(BARE2D::ResourceManager::loadTexture(file));
}

void MainMenuScreen::onExit() {
	m_renderer->destroy();
	m_fontRenderer->destroy();
}

void MainMenuScreen::update(double dt) {
	BARE2D::BARECEGUI::getInstance()->update();

	m_time++;
}

void MainMenuScreen::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	float a = m_time;
	if(a > 255.0f)
		a = 255.0f;

	{
		m_renderer->begin();

		if(m_currentBackgroundImage !=
		   (int)(m_time / (m_backgroundChangeTime * 60.0f) + 1.0f - 0.5f) % (int)m_backgroundImages.size()) {
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

		m_renderer->draw(glm::vec4(0.0f, 0.0f, m_window->getWidth(), m_window->getHeight()),
						 glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
						 m_backgroundImages[m_currentBackgroundImage].id,
						 0.0f,
						 m_backgroundImageColour);

		m_renderer->end();
		m_renderer->render();
	}

	m_playButton->setAlpha(a);
	m_optionsButton->setAlpha(a);
	m_exitButton->setAlpha(a);
	BARE2D::BARECEGUI::getInstance()->draw();
}

/// PRIVATE FUNCTIONS

void MainMenuScreen::initUI() {
	BARE2D::BARECEGUI* gui = BARE2D::BARECEGUI::getInstance();
	{
		std::string guiPath = ASSETS_FOLDER_PATH + "GUI";
		gui->init(guiPath, 1);
		gui->loadScheme("FOTDSkin.scheme");

		gui->setFont("QuietHorror-42");

		gui->setMouseCursor("FOTDSkin/MouseArrow");
		gui->setMouseCursorShown(true);
		SDL_ShowCursor(0);
	}

	{
		m_titleLine1 = static_cast<CEGUI::DefaultWindow*>(gui->createWidget("FOTDSkin/Label",
																			glm::vec4(0.075f, 0.05f, 0.5f, 0.25f),
																			glm::vec4(0.0f),
																			nullptr,
																			"TitleBox1MainMenu"));
		m_titleLine1->setText("Fear Of");
		m_titleLine1->setProperty("HorzFormatting", "LeftAligned");
		m_titleLine1->setFont("QuietHorror-118");

		m_titleLine2 = static_cast<CEGUI::DefaultWindow*>(gui->createWidget("FOTDSkin/Label",
																			glm::vec4(0.075f, 0.25f, 0.5f, 0.25f),
																			glm::vec4(0.0f),
																			nullptr,
																			"TitleBox2MainMenu"));
		m_titleLine2->setText("     The Dark");
		m_titleLine2->setProperty("HorzFormatting", "LeftAligned");
		m_titleLine2->setFont("QuietHorror-118");
	}

	{
		const float PADDING = 0.15f;

		m_playButton = static_cast<CEGUI::PushButton*>(gui->createWidget("FOTDSkin/Button",
																		 glm::vec4(0.1f, 0.5f, 0.30f, 0.1f),
																		 glm::vec4(0.0f),
																		 nullptr,
																		 "PlayButtonMainMenu"));
		m_playButton->subscribeEvent(CEGUI::PushButton::EventClicked,
									 CEGUI::Event::Subscriber(&MainMenuScreen::EventPlayButtonClicked, this));
		m_playButton->setText("[padding='l:0 t:15 r:0 b:0']Play");

		m_optionsButton =
			static_cast<CEGUI::PushButton*>(gui->createWidget("FOTDSkin/Button",
															  glm::vec4(0.1f, 0.5f + PADDING, 0.30f, 0.1f),
															  glm::vec4(0.0f),
															  nullptr,
															  "OptionsButtonMainMenu"));
		m_optionsButton->subscribeEvent(CEGUI::PushButton::EventClicked,
										CEGUI::Event::Subscriber(&MainMenuScreen::EventOptionsButtonClicked, this));
		m_optionsButton->setText("[padding='l:0 t:15 r:0 b:0']Options");

		m_exitButton =
			static_cast<CEGUI::PushButton*>(gui->createWidget("FOTDSkin/Button",
															  glm::vec4(0.1f, 0.5f + PADDING * 2.0f, 0.30f, 0.1f),
															  glm::vec4(0.0f),
															  nullptr,
															  "ExitButtonMainMenu"));
		m_exitButton->subscribeEvent(CEGUI::PushButton::EventClicked,
									 CEGUI::Event::Subscriber(&MainMenuScreen::EventExitButtonClicked, this));
		m_exitButton->setText("[padding='l:0 t:15 r:0 b:0']Exit");
	}
}

/// EVENT FUNCTIONS (GUI)

bool MainMenuScreen::EventPlayButtonClicked(const CEGUI::EventArgs& e) {
	m_nextScreenIndex = SCREEN_INDEX_LOAD;
	m_screenState	  = BARE2D::ScreenState::CHANGE_NEXT;
	return true;
}

bool MainMenuScreen::EventOptionsButtonClicked(const CEGUI::EventArgs& e) {
	m_nextScreenIndex = SCREEN_INDEX_OPTIONSMENU;
	m_screenState	  = BARE2D::ScreenState::CHANGE_NEXT;
	return true;
}

bool MainMenuScreen::EventExitButtonClicked(const CEGUI::EventArgs& e) {
	m_screenState = BARE2D::ScreenState::EXIT_APPLICATION;
	return true;
}
