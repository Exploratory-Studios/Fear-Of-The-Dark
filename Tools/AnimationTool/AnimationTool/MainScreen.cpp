#include "MainScreen.h"

MainScreen::MainScreen(GLEngine::Window* window) : m_window(window) {
}

MainScreen::~MainScreen() {
}

void MainScreen::build() {

}

void MainScreen::onEntry() {
	initShaders();

	m_spriteBatch.init();

	m_cam.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_cam.setPosition((glm::vec2(m_window->getScreenWidth() / 2, m_window->getScreenHeight() / 2)));
	m_cam.setScale(1.0f);

	initUI();
}

void MainScreen::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	float width = m_window->getScreenWidth();
	float height = m_window->getScreenHeight();

	m_backgroundProgram.use();

	glm::mat4 projectionMatrix = m_cam.getCameraMatrix();
	GLint pUniform = m_backgroundProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	m_spriteBatch.begin();
	m_spriteBatch.draw(glm::vec4(0.0f, 0.0f, width, height), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0, 0.0f, GLEngine::ColourRGBA8(255, 255, 255, 255));
	m_spriteBatch.end();
	m_spriteBatch.renderBatch();

	m_backgroundProgram.unuse();

	m_gui.draw();
}

void MainScreen::update() {
	m_cam.update();

	checkInput();
}

void MainScreen::checkInput() {
	SDL_Event evnt;
	while(SDL_PollEvent(&evnt)) {
		m_game->onSDLEvent(evnt);
		m_gui.onSDLEvent(evnt);
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
	m_gui.init("GUI");
	m_gui.loadScheme("WindowsLook.scheme");
	m_gui.setFont("DejaVuSans-12");
	m_gui.hideMouseCursor();

	{
		m_fileFrame = static_cast<CEGUI::FrameWindow*>(m_gui.createWidget("WindowsLook/FrameWindow", glm::vec4(0.25f, 0.85f, 0.75f, 0.15f), glm::vec4(0.0f), "fileframe"));
		m_fileFrame->setCloseButtonEnabled(false);
		m_fileFrame->setDragMovingEnabled(false);
		m_fileFrame->setRollupEnabled(false);
		m_fileFrame->setTitleBarEnabled(false);

		m_exportButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_fileFrame, "WindowsLook/Button", glm::vec4(0.55f, 0.55f, 0.4f, 0.4f), glm::vec4(0.0f), "fileframeExport"));
		m_exportButton->setText("Export");

		m_nameBox = static_cast<CEGUI::Editbox*>(m_gui.createWidget(m_fileFrame, "WindowsLook/Editbox", glm::vec4(0.55f, 0.05f, 0.4f, 0.4f), glm::vec4(0.0f), "fileframeName"));

		m_newLimbButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_fileFrame, "WindowsLook/Button", glm::vec4(0.05f, 0.05f, 0.3f, 0.4f), glm::vec4(0.0f), "fileframeNewLimb"));
		m_newLimbButton->setText("Add Limb");
	}

	{
		m_toolFrame = static_cast<CEGUI::FrameWindow*>(m_gui.createWidget("WindowsLook/FrameWindow", glm::vec4(0.25f, 0.0f, 0.75f, 0.1f), glm::vec4(0.0f), "toolframe"));
		m_toolFrame->setCloseButtonEnabled(false);
		m_toolFrame->setDragMovingEnabled(false);
		m_toolFrame->setRollupEnabled(false);
		m_toolFrame->setTitleBarEnabled(false);

		m_removeLimbButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_toolFrame, "WindowsLook/Button", glm::vec4(0.85f, 0.05f, 0.1f, 0.9f), glm::vec4(0.0f), "toolframeRemoveLimb"));
		m_removeLimbButton->setText("Remove Limb");

		m_rotateOffsetButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_toolFrame, "WindowsLook/Button", glm::vec4(0.05f, 0.05f, 0.4f, 0.9f), glm::vec4(0.0f), "toolframeToolSwitcher"));
		m_rotateOffsetButton->setText("Rotating");
		m_tool = ToolType::ROTATE;
	}

	{
		m_keyframesFrame = static_cast<CEGUI::FrameWindow*>(m_gui.createWidget("WindowsLook/FrameWindow", glm::vec4(0.0f, 0.0f, 0.25f, 1.0f), glm::vec4(0.0f), "keyframeframe"));
		m_keyframesFrame->setCloseButtonEnabled(false);
		m_keyframesFrame->setDragMovingEnabled(false);
		m_keyframesFrame->setRollupEnabled(false);
		m_keyframesFrame->setTitleBarEnabled(false);

		m_playPauseButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_keyframesFrame, "WindowsLook/Button", glm::vec4(0.8f, 0.9f, 0.15f, 0.05f), glm::vec4(0.0f), "keyframeframePlayPause"));
		m_playPauseButton->setText("Play");

		m_addFrameButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_keyframesFrame, "WindowsLook/Button", glm::vec4(0.05f, 0.9f, 0.15f, 0.05f), glm::vec4(0.0f), "keyframeframeAddFrame"));
		m_addFrameButton->setText("+");

		m_removeFrameButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_keyframesFrame, "WindowsLook/Button", glm::vec4(0.25f, 0.9f, 0.15f, 0.05f), glm::vec4(0.0f), "keyframeframeRemoveFrame"));
		m_removeFrameButton->setText("-");
	}
}

void MainScreen::initShaders() {
	std::string backgroundSrcVert = "\
#version 130\n\
in vec3 vertexPosition;\n\
in vec4 vertexColour;\n\
in vec2 vertexUV;\n\
out vec3 fragmentPosition;\n\
out vec4 fragmentColour;\n\
out vec2 fragmentUV;\n\
uniform mat4 P;\n\
void main() {\n\
gl_Position.xyz = (P * vec4(vertexPosition, 1.0)).xyz;\n\
gl_Position.w = 1.0;\n\
fragmentPosition = vertexPosition;\n\
fragmentColour = vertexColour;\n\
fragmentUV = vec2(vertexUV.x, vertexUV.y); // Doesn't even invert Y.\n\
}";

	std::string backgroundSrcFrag = "\
#version 130\n\
in vec3 fragmentPosition;\n\
in vec4 fragmentColour;\n\
in vec2 fragmentUV;\n\
out vec4 colour;\n\
void main() {\n\
colour.a = 1.0f;\n\
\n\
float x = ceil(fragmentPosition.x / 10.0f);\n\
float y = ceil(fragmentPosition.y / 10.0f);\n\
\n\
if(int(x) % 2 == int(y) % 2) {\n\
colour.rgb = vec3(0.792f);\n\
} else {\n\
colour.rgb = vec3(1.0f);\n\
}\n\
}";

	m_backgroundProgram.compileShadersFromSource(backgroundSrcVert.c_str(), backgroundSrcFrag.c_str());
	m_backgroundProgram.addAttribute("vertexPosition");
	m_backgroundProgram.addAttribute("vertexColour");
	m_backgroundProgram.addAttribute("vertexUV");
	m_backgroundProgram.linkShaders();
}



/// CEGUI Event Functions

bool EventExportButtonClicked(const CEGUI::EventArgs& e) {
	//std::string filepath = m_nameBox->getText();

	return true;
}

bool EventNewLimbButtonClicked(const CEGUI::EventArgs& e) {

	return true;
}


bool EventRemoveLimbButtonClicked(const CEGUI::EventArgs& e) {

	return true;
}

bool EventRotateOffsetLimbButtonClicked(const CEGUI::EventArgs& e) {

	return true;
}


bool EventPlayPauseButtonClicked(const CEGUI::EventArgs& e) {

	return true;
}

bool EventAddFrameButtonClicked(const CEGUI::EventArgs& e) {

	return true;
}

bool EventRemoveFrameButtonClicked(const CEGUI::EventArgs& e) {

	return true;
}
