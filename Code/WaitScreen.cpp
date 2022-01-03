#include "WaitScreen.h"

#include <GLContextManager.hpp>

WaitScreen::WaitScreen(BARE2D::Window* window, WorldIOManager* WorldIOManager) :
	m_window(window), m_worldIOManager(WorldIOManager)
{
}

WaitScreen::~WaitScreen()
{
	//dtor
}

int WaitScreen::getNextScreenIndex() const
{
	return m_nextScreenIndex;
}

void WaitScreen::initScreen()
{
}

void WaitScreen::destroyScreen()
{
}

void WaitScreen::onEntry()
{
	std::string fragShader = "Shaders/textureShader.frag";
	std::string vertShader = "Shaders/textureShader.vert";

	m_renderer = new BARE2D::BasicRenderer(fragShader, vertShader);
	m_renderer->init();

	m_fontRenderer = new BARE2D::FontRenderer(fragShader, vertShader);
	m_fontRenderer->init();

	initUI();
}

void WaitScreen::onExit()
{
	m_renderer->destroy();
	m_fontRenderer->destroy();
}

void WaitScreen::update(double dt)
{
	BARE2D::BARECEGUI::getInstance()->update();

	m_time++;

	m_progressBar->setProgress(m_worldIOManager->getProgress());
	m_messageBox->setText(m_worldIOManager->getMessage());

	if(m_worldIOManager->getProgress() >= 1.0f) {
		m_screenState = BARE2D::ScreenState::CHANGE_NEXT;
	}
}

void WaitScreen::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	m_renderer->begin();

	float alpha = 0.0f;

	if(m_time <= 255.0f / 2.0f) {
		alpha = m_time * 2.0f;
	} else {
		alpha = 255.0f;
	}

	m_progressBar->setAlpha(alpha / 255);

	BARE2D::BARECEGUI::getInstance()->draw();

	m_renderer->end();
	m_renderer->render();
}

void WaitScreen::initUI()
{
	BARE2D::BARECEGUI* gui = BARE2D::BARECEGUI::getInstance();
	{
		std::string guiPath = ASSETS_FOLDER_PATH + "GUI";
		gui->init(guiPath, 1);
		gui->loadScheme("FOTDSkin.scheme");

		gui->setFont("QuietHorror-30");

		gui->setMouseCursor("FOTDSkin/MouseArrow");
		gui->setMouseCursorShown(true);
		SDL_ShowCursor(0);
	}

	{
		m_progressBar = static_cast<CEGUI::ProgressBar*>(gui->createWidget("FOTDSkin/ProgressBar",
		                glm::vec4(0.1f, 0.1f, 0.8f, 0.1f),
		                glm::vec4(0.0f),
		                nullptr,
		                "WaitScreenProgressBar"));
		m_progressBar->setText("[padding='l:0 t:15 r:0 b:0']Progress: ");
		m_progressBar->setFont("QuietHorror-42");

		m_messageBox = static_cast<CEGUI::DefaultWindow*>(gui->createWidget("FOTDSkin/Label",
		               glm::vec4(0.1f, 0.2f, 0.8f, 0.2f),
		               glm::vec4(0.0f),
		               nullptr,
		               "WaitScreenWaitMessage"));
		m_messageBox->setText(m_worldIOManager->getMessage());
	}
}
