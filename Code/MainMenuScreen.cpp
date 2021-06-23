#include "MainMenuScreen.h"

#include "XMLData.h"

/// SCREEN FUNCTIONS

MainMenuScreen::MainMenuScreen(GLEngine::Window* window) : m_window(window)
{
	//ctor
}

MainMenuScreen::~MainMenuScreen()
{
	//dtor
}

int MainMenuScreen::getNextScreenIndex() const
{
	return m_nextScreenIndex;
}

int MainMenuScreen::getPreviousScreenIndex() const
{
	return SCREEN_INDEX_NO_SCREEN;
}

void MainMenuScreen::build()
{

}

void MainMenuScreen::destroy()
{

}

void MainMenuScreen::onEntry()
{

	initShaders();

	m_spriteBatch.init();
	m_spriteFont.init((ASSETS_FOLDER_PATH + "GUI/fonts/QuietHorror.ttf").c_str(), 96);

	//if(!m_inited) {
	m_uiCamera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_uiCamera.setPosition(glm::vec2(m_window->getScreenWidth() / 2.0f, m_window->getScreenHeight() / 2.0f));

	m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_camera.setPosition(glm::vec2(m_window->getScreenWidth() / 2.0f, m_window->getScreenHeight() / 2.0f));

	initUI();

	addBackgroundImage(m_backgroundImages, "Texture1.png");
	addBackgroundImage(m_backgroundImages, "Texture2.png");
	addBackgroundImage(m_backgroundImages, "Texture3.png");
	//}

	m_time = 0.0f;

	XMLModule::XMLData::init(); // This is a decent place for this. Doesn't load as the world is loading, is loaded before.
}

void addBackgroundImage(std::vector<GLEngine::GLTexture>& backgroundImages, std::string filename)
{
	backgroundImages.emplace_back(GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/MainMenuBackgrounds/" + filename));
}

void MainMenuScreen::onExit()
{
	m_spriteBatch.dispose();
	m_uiTextureProgram.dispose();
	m_spriteFont.dispose();

	m_gui.destroy();
}

void MainMenuScreen::update()
{
	checkInput();
	m_gui.update();
	m_uiCamera.update();
	m_camera.update();

	m_time++;
}

void MainMenuScreen::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	float a = m_time;
	if(a > 255.0f) a = 255.0f;

	{
		m_uiTextureProgram.use();

		GLint textureUniform = m_uiTextureProgram.getUniformLocation("textureSampler");
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

		GLint textureUniform = m_uiTextureProgram.getUniformLocation("textureSampler");
		glUniform1i(textureUniform, 0);
		glActiveTexture(GL_TEXTURE0);

		// Camera matrix
		glm::mat4 projectionMatrix = m_uiCamera.getCameraMatrix();
		GLint pUniform = m_uiTextureProgram.getUniformLocation("P");
		glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

		m_spriteBatch.begin();

		m_spriteBatch.end();
		m_spriteBatch.renderBatch();

		m_uiTextureProgram.unuse();
	}
}

/// PRIVATE FUNCTIONS

void MainMenuScreen::initUI()
{
	{
		m_gui.init(ASSETS_FOLDER_PATH + "GUI", 1);
		m_gui.loadScheme("FOTDSkin.scheme");

		m_gui.setFont("QuietHorror-42");

		m_gui.setMouseCursor("FOTDSkin/MouseArrow");
		m_gui.showMouseCursor();
		SDL_ShowCursor(0);
	}

	{
		m_titleLine1 = static_cast<CEGUI::DefaultWindow*>(m_gui.createWidget("FOTDSkin/Label", glm::vec4(0.075f, 0.05f, 0.5f, 0.25f), glm::vec4(0.0f), "TitleBox1MainMenu"));
		m_titleLine1->setText("Fear Of");
		m_titleLine1->setProperty("HorzFormatting", "LeftAligned");
		m_titleLine1->setFont("QuietHorror-118");

		m_titleLine2 = static_cast<CEGUI::DefaultWindow*>(m_gui.createWidget("FOTDSkin/Label", glm::vec4(0.075f, 0.25f, 0.5f, 0.25f), glm::vec4(0.0f), "TitleBox2MainMenu"));
		m_titleLine2->setText("     The Dark");
		m_titleLine2->setProperty("HorzFormatting", "LeftAligned");
		m_titleLine2->setFont("QuietHorror-118");
	}

	{


		const float PADDING = 0.15f;

		m_playButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("FOTDSkin/Button", glm::vec4(0.1f, 0.5f, 0.30f, 0.1f), glm::vec4(0.0f), "PlayButtonMainMenu"));
		m_playButton->subscribeEvent(CEGUI::PushButton::EventClicked,
		                             CEGUI::Event::Subscriber(&MainMenuScreen::EventPlayButtonClicked, this));
		m_playButton->setText("[padding='l:0 t:15 r:0 b:0']Play");


		m_optionsButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("FOTDSkin/Button", glm::vec4(0.1f, 0.5f + PADDING, 0.30f, 0.1f), glm::vec4(0.0f), "OptionsButtonMainMenu"));
		m_optionsButton->subscribeEvent(CEGUI::PushButton::EventClicked,
		                                CEGUI::Event::Subscriber(&MainMenuScreen::EventOptionsButtonClicked, this));
		m_optionsButton->setText("[padding='l:0 t:15 r:0 b:0']Options");


		m_exitButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("FOTDSkin/Button", glm::vec4(0.1f, 0.5f + PADDING * 2.0f, 0.30f, 0.1f), glm::vec4(0.0f), "ExitButtonMainMenu"));
		m_exitButton->subscribeEvent(CEGUI::PushButton::EventClicked,
		                             CEGUI::Event::Subscriber(&MainMenuScreen::EventExitButtonClicked, this));
		m_exitButton->setText("[padding='l:0 t:15 r:0 b:0']Exit");
	}

}

void MainMenuScreen::initShaders()
{
	m_uiTextureProgram.compileShaders(ASSETS_FOLDER_PATH + "Shaders/textureShader.vert", ASSETS_FOLDER_PATH + "Shaders/textureShader.frag");
	m_uiTextureProgram.addAttribute("vertexPosition");
	m_uiTextureProgram.addAttribute("vertexColour");
	m_uiTextureProgram.addAttribute("vertexUV");
	m_uiTextureProgram.linkShaders();
}

void MainMenuScreen::checkInput()
{
	SDL_Event evnt;
	while(SDL_PollEvent(&evnt)) {
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

void MainMenuScreen::updateMousebuttonDown(SDL_Event& evnt)
{
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

bool MainMenuScreen::EventPlayButtonClicked(const CEGUI::EventArgs& e)
{
	m_nextScreenIndex = SCREEN_INDEX_LOAD;
	m_currentState = GLEngine::ScreenState::CHANGE_NEXT;
	return true;
}

bool MainMenuScreen::EventOptionsButtonClicked(const CEGUI::EventArgs& e)
{
	m_nextScreenIndex = SCREEN_INDEX_OPTIONSMENU;
	m_currentState = GLEngine::ScreenState::CHANGE_NEXT;
	return true;
}

bool MainMenuScreen::EventExitButtonClicked(const CEGUI::EventArgs& e)
{
	m_currentState = GLEngine::ScreenState::EXIT_APPLICATION;
	return true;
}
