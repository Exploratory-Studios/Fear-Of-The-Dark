#include "MainScreen.h"

#include <filesystem>
#include <ResourceManager.h>

#include "Presets.h"

MainScreen::MainScreen(GLEngine::Window* window) : m_window(window) {
}

MainScreen::~MainScreen() {
}

void MainScreen::build() {

}

void MainScreen::onEntry() {
	initShaders();

	m_spriteBatch.init();
	m_debugRenderer.init();

	m_cam.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_cam.setPosition((glm::vec2(m_window->getScreenWidth() / 2, m_window->getScreenHeight() / 2)));
	m_cam.setScale(1.0f);

	m_backgroundCam.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_backgroundCam.setPosition((glm::vec2(m_window->getScreenWidth() / 2, m_window->getScreenHeight() / 2)));

	initUI();
}

void MainScreen::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	float width = m_window->getScreenWidth();
	float height = m_window->getScreenHeight();

	{
		m_backgroundProgram.use();

		glm::mat4 projectionMatrix = m_backgroundCam.getCameraMatrix();
		GLint pUniform = m_backgroundProgram.getUniformLocation("P");
		glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

		m_spriteBatch.begin();
		m_spriteBatch.draw(glm::vec4(0.0f, 0.0f, width, height), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0, 0.0f, GLEngine::ColourRGBA8(255, 255, 255, 255));
		m_spriteBatch.end();
		m_spriteBatch.renderBatch();

		m_backgroundProgram.unuse();
	}

	{
		m_textureProgram.use();

		glm::mat4 projectionMatrix = m_cam.getCameraMatrix();
		GLint pUniform = m_textureProgram.getUniformLocation("P");
		glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

		GLint textureUniform = m_textureProgram.getUniformLocation("textureSampler");
		glUniform1i(textureUniform, 0);

		m_spriteBatch.begin();


		for(unsigned int i = 0; i < m_limbs.size(); i++) {
			glm::vec4 destRect(width / 2, height / 2, m_limbs[i].getAnimation().getFrameWidth(), m_limbs[i].getAnimation().getFrameHeight());
			if(m_limbsVisibility[i]) {
				float depth = 0.0f;
				m_limbs[i].draw(m_spriteBatch, destRect, depth);
			} else {
				float angle = m_limbs[i].getAngle();
				m_debugRenderer.drawBox(destRect, GLEngine::ColourRGBA8(255, 0, 0, 128), angle);
			}
		}

		m_spriteBatch.end();
		m_spriteBatch.renderBatch();

		m_textureProgram.unuse();
	}

	if(m_limbs.size() > m_selectedLimb && m_limbSelected) {
		glm::vec4 destRect;
		destRect.x = width / 2 + m_limbs[m_selectedLimb].getOffset().x;
		destRect.y = height / 2 + m_limbs[m_selectedLimb].getOffset().y;
		destRect.z = m_limbs[m_selectedLimb].getAnimation().getFrameWidth();
		destRect.w = m_limbs[m_selectedLimb].getAnimation().getFrameHeight();
		m_debugRenderer.drawBox(destRect, GLEngine::ColourRGBA8(0, 0, 0, 255), m_limbs[m_selectedLimb].getAngle());
	}

	m_debugRenderer.end();
	m_debugRenderer.render(m_cam.getCameraMatrix(), 2);

	m_gui.draw();
}

void MainScreen::update() {
	m_cam.update();
	m_backgroundCam.update();

	if(m_previewing) {
		for(unsigned int i = 0; i < m_limbs.size(); i++) {
			m_limbs[i].update();
			if(m_frame % (m_frame / TICK_RATE) == 0) {
				m_limbs[i].tick();
			}
		}
	}

	m_frame++;

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

			case SDL_MOUSEWHEEL:
				m_scale += (float)m_game->inputManager.getMouseScrollPosition() * m_scale / 20.0f;
				if(m_scale > 100.0f) m_scale = 100.0f;
				if(m_scale < 1.0f) m_scale = 1.0f;
				m_cam.setScale(m_scale);
				break;

			case SDL_MOUSEBUTTONDOWN:
				break;
		}
	}

	if(m_game->inputManager.isKeyPressed(SDL_BUTTON_LEFT)) {
		glm::vec2 mouse = m_game->inputManager.getMouseCoords();
		m_limbSelected = selectLimb(mouse);
		m_grabPosition = m_cam.convertScreenToWorld(mouse);
	}
	if(m_limbSelected) {
		if(m_game->inputManager.isKeyDown(SDL_BUTTON_LEFT)) {
			if(m_tool == ToolType::OFFSET) {
				glm::vec2 change = m_cam.convertScreenToWorld(m_game->inputManager.getMouseCoords()) - m_grabPosition;

				glm::vec2 newOffset = m_limbs[m_selectedLimb].getOffset() + change;
				newOffset.x = (int)newOffset.x;
				newOffset.y = (int)newOffset.y;

				m_limbs[m_selectedLimb].setOffset(newOffset);

				m_grabPosition = m_cam.convertScreenToWorld(m_game->inputManager.getMouseCoords());
			} else if(m_tool == ToolType::ROTATE) {
				glm::vec2 screenSize = glm::vec2(m_window->getScreenWidth() / 2, m_window->getScreenHeight() / 2);
				glm::vec2 old = glm::normalize(m_grabPosition - screenSize);
				m_grabPosition = m_cam.convertScreenToWorld(m_game->inputManager.getMouseCoords());
				glm::vec2 now = glm::normalize(m_grabPosition - screenSize);

				float dot = old.x * now.x + old.y * now.y;
				float det = old.x * now.y - old.y * now.x;
				float newAngle = (m_limbs[m_selectedLimb].getAngle() + std::atan2(det, dot));

				m_limbs[m_selectedLimb].setAngle(newAngle);
			}
		}
	}
	if(m_game->inputManager.isKeyPressed(SDL_BUTTON_RIGHT)) {
		m_limbsVisibility[m_selectedLimb] = false;
	}
}

bool MainScreen::selectLimb(glm::vec2& mouseCoords) {
	float width = m_window->getScreenWidth();
	float height = m_window->getScreenHeight();

	for(int i = m_limbs.size() - 1; i >= 0; i--) {
		if(m_limbsVisibility[i]) {
			// Get copies of each position
			glm::vec2 size = glm::vec2(m_limbs[i].getAnimation().getFrameWidth(), m_limbs[i].getAnimation().getFrameHeight());
			glm::vec2 mousePos = m_cam.convertScreenToWorld(mouseCoords) - glm::vec2(width / 2, height / 2) - size * glm::vec2(0.5f);
			glm::vec2 limbPosBL = m_limbs[i].getOffset() - size * glm::vec2(0.5f);

			//(xcosθ−ysinθ ,xsinθ+ycosθ)
			float theta = -m_limbs[i].getAngle(); // radians
			glm::vec2 mouseTrans = glm::vec2(mousePos.x * std::cos(theta) - mousePos.y * std::sin(theta), mousePos.x * std::sin(theta) + mousePos.y * std::cos(theta));

			glm::vec2 BLTrans = limbPosBL;//glm::vec2(limbPosBL.x * std::cos(theta) - limbPosBL.y * std::sin(theta), limbPosBL.x * std::sin(theta) + limbPosBL.y * std::cos(theta));
			glm::vec2 TRTrans = BLTrans + glm::vec2(m_limbs[i].getAnimation().getFrameWidth(), m_limbs[i].getAnimation().getFrameHeight());

			// Check for intersection
			if(mouseTrans.x > BLTrans.x && mouseTrans.x < TRTrans.x) {
				if(mouseTrans.y > BLTrans.y && mouseTrans.y < TRTrans.y) {
					m_selectedLimb = i;
					return true;
				}
			}
		}
	}
	return false;
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

		m_exportButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_fileFrame, "WindowsLook/Button", glm::vec4(0.55f, 0.55f, 0.175f, 0.4f), glm::vec4(0.0f), "fileframeExport"));
		m_exportButton->setText("Export");
		m_exportButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainScreen::EventExportButtonClicked, this));

		m_importButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_fileFrame, "WindowsLook/Button", glm::vec4(0.775f, 0.55f, 0.175f, 0.4f), glm::vec4(0.0f), "fileframeImport"));
		m_importButton->setText("Import");
		m_importButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainScreen::EventImportButtonClicked, this));

		m_nameBox = static_cast<CEGUI::Editbox*>(m_gui.createWidget(m_fileFrame, "WindowsLook/Editbox", glm::vec4(0.55f, 0.05f, 0.4f, 0.4f), glm::vec4(0.0f), "fileframeName"));

		//m_loadSkinButton m_skinBox m_importButton m_skinX m_skinY
		m_skinBox = static_cast<CEGUI::Editbox*>(m_gui.createWidget(m_fileFrame, "WindowsLook/Editbox", glm::vec4(0.05f, 0.05f, 0.4f, 0.4f), glm::vec4(0.0f), "fileframeskinbox"));

		m_loadSkinButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_fileFrame, "WindowsLook/Button", glm::vec4(0.05f, 0.55f, 0.2f, 0.4f), glm::vec4(0.0f), "fileframeloadskin"));
		m_loadSkinButton->setText("Load Skin");
		m_loadSkinButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainScreen::EventLoadSkinButtonClicked, this));

		m_skinX = static_cast<CEGUI::Spinner*>(m_gui.createWidget(m_fileFrame, "WindowsLook/Spinner", glm::vec4(0.275f, 0.55f, 0.075f, 0.4f), glm::vec4(0.0f), "fileframeskinXSpinner"));
		CEGUI::DefaultWindow* lx = static_cast<CEGUI::DefaultWindow*>(m_gui.createWidget(m_fileFrame, "WindowsLook/Label", glm::vec4(0.225f, 0.55f, 0.075f, 0.4f), glm::vec4(0.0f), "fileframeXSpinnerLabel"));
		lx->setText("W:");
		lx->setMousePassThroughEnabled(true);
		m_skinY = static_cast<CEGUI::Spinner*>(m_gui.createWidget(m_fileFrame, "WindowsLook/Spinner", glm::vec4(0.375f, 0.55f, 0.075f, 0.4f), glm::vec4(0.0f), "fileframeskinYSpinner"));
		CEGUI::DefaultWindow* ly = static_cast<CEGUI::DefaultWindow*>(m_gui.createWidget(m_fileFrame, "WindowsLook/Label", glm::vec4(0.325f, 0.55f, 0.075f, 0.4f), glm::vec4(0.0f), "fileframeYSpinnerLabel"));
		ly->setText("H:");
		ly->setMousePassThroughEnabled(true);

	}

	{
		m_toolFrame = static_cast<CEGUI::FrameWindow*>(m_gui.createWidget("WindowsLook/FrameWindow", glm::vec4(0.25f, 0.0f, 0.75f, 0.1f), glm::vec4(0.0f), "toolframe"));
		m_toolFrame->setCloseButtonEnabled(false);
		m_toolFrame->setDragMovingEnabled(false);
		m_toolFrame->setRollupEnabled(false);
		m_toolFrame->setTitleBarEnabled(false);

		m_newLimbButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_toolFrame, "WindowsLook/Button", glm::vec4(0.7f, 0.05f, 0.1f, 0.9f), glm::vec4(0.0f), "toolframeNewLimb"));
		m_newLimbButton->setText("Add Limb");
		m_newLimbButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainScreen::EventNewLimbButtonClicked, this));

		m_removeLimbButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_toolFrame, "WindowsLook/Button", glm::vec4(0.85f, 0.05f, 0.1f, 0.9f), glm::vec4(0.0f), "toolframeRemoveLimb"));
		m_removeLimbButton->setText("Remove Limb");
		m_removeLimbButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainScreen::EventRemoveLimbButtonClicked, this));

		m_rotateOffsetButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_toolFrame, "WindowsLook/Button", glm::vec4(0.05f, 0.05f, 0.3f, 0.9f), glm::vec4(0.0f), "toolframeToolSwitcher"));
		m_rotateOffsetButton->setText("Rotating");
		m_tool = ToolType::ROTATE;
		m_rotateOffsetButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainScreen::EventRotateOffsetLimbButtonClicked, this));

		m_makeAllVisibleButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_toolFrame, "WindowsLook/Button", glm::vec4(0.375f, 0.05f, 0.3f, 0.9f), glm::vec4(0.0f), "toolframemakeallvisible"));
		m_makeAllVisibleButton->setText("Make all visible");
		m_makeAllVisibleButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainScreen::EventMakeAllVisibleButtonClicked, this));
	}

	{
		m_keyframesFrame = static_cast<CEGUI::FrameWindow*>(m_gui.createWidget("WindowsLook/FrameWindow", glm::vec4(0.0f, 0.0f, 0.25f, 1.0f), glm::vec4(0.0f), "keyframeframe"));
		m_keyframesFrame->setCloseButtonEnabled(false);
		m_keyframesFrame->setDragMovingEnabled(false);
		m_keyframesFrame->setRollupEnabled(false);
		m_keyframesFrame->setTitleBarEnabled(false);

		m_keyframesList = static_cast<CEGUI::Listbox*>(m_gui.createWidget(m_keyframesFrame, "WindowsLook/Listbox", glm::vec4(0.05f, 0.05f, 0.9f, 0.8f), glm::vec4(0.0f), "keyframeframeList"));
		m_keyframesList->subscribeEvent(CEGUI::Listbox::EventSelectionChanged, CEGUI::Event::Subscriber(&MainScreen::EventKeyFrameClicked, this));
		m_keyframesList->setMultiselectEnabled(false);

		m_playPauseButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_keyframesFrame, "WindowsLook/Button", glm::vec4(0.75f, 0.9f, 0.2f, 0.05f), glm::vec4(0.0f), "keyframeframePlayPause"));
		m_playPauseButton->setText("Play");
		m_playPauseButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainScreen::EventPlayPauseButtonClicked, this));

		m_addFrameButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_keyframesFrame, "WindowsLook/Button", glm::vec4(0.05f, 0.9f, 0.15f, 0.05f), glm::vec4(0.0f), "keyframeframeAddFrame"));
		m_addFrameButton->setText("+");
		m_addFrameButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainScreen::EventAddFrameButtonClicked, this));

		m_removeFrameButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget(m_keyframesFrame, "WindowsLook/Button", glm::vec4(0.25f, 0.9f, 0.15f, 0.05f), glm::vec4(0.0f), "keyframeframeRemoveFrame"));
		m_removeFrameButton->setText("-");
		m_removeFrameButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainScreen::EventRemoveFrameButtonClicked, this));
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
fragmentUV = vec2(vertexUV.x, 1.0 - vertexUV.y); // Doesn't even invert Y.\n\
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
if(floor(fragmentPosition.x) == floor(511.0f) || floor(fragmentPosition.y) == floor(311.0f)) {\n\
	colour.rgb = vec3(0.0f);\n\
}\n\
}";

	std::string textureSrcFrag = "\
#version 130\n\
in vec3 fragmentPosition;\n\
in vec4 fragmentColour;\n\
in vec2 fragmentUV;\n\
uniform sampler2D textureSampler;\n\
out vec4 colour;\n\
void main() {\n\
colour = texture(textureSampler, fragmentUV.xy);\n\
}";

	m_backgroundProgram.compileShadersFromSource(backgroundSrcVert.c_str(), backgroundSrcFrag.c_str());
	m_backgroundProgram.addAttribute("vertexPosition");
	m_backgroundProgram.addAttribute("vertexColour");
	m_backgroundProgram.addAttribute("vertexUV");
	m_backgroundProgram.linkShaders();

	m_textureProgram.compileShadersFromSource(backgroundSrcVert.c_str(), textureSrcFrag.c_str());
	m_textureProgram.addAttribute("vertexPosition");
	m_textureProgram.addAttribute("vertexColour");
	m_textureProgram.addAttribute("vertexUV");
	m_textureProgram.linkShaders();
}



/// CEGUI Event Functions
#include <iostream>
bool MainScreen::EventExportButtonClicked(const CEGUI::EventArgs& e) {
	std::cout << "Export" << std::endl;

	return true;
}

bool MainScreen::EventImportButtonClicked(const CEGUI::EventArgs& e) {
	std::cout << "Import" << std::endl;

	return true;
}

bool MainScreen::EventLoadSkinButtonClicked(const CEGUI::EventArgs& e) {
	std::cout << "Load Skin" << std::endl;

	std::string filepath = std::filesystem::current_path().generic_string() + "/" + std::string(m_skinBox->getText().c_str());

	std::cout << "Filepath: " << filepath << std::endl;

	m_skins.clear();

	GLEngine::GLTexture tex = GLEngine::ResourceManager::getTexture(filepath);
	if(tex.width > 0) {
		// find the y, framewidth, frameheight
		unsigned int frameWidth = m_skinX->getCurrentValue();
		unsigned int frameHeight = m_skinY->getCurrentValue();
		for(unsigned int y = 0; y < tex.height / frameHeight; y++) {
			AnimationModule::AugAnimation anim(tex, y, frameWidth, frameHeight);

			m_skins.push_back(anim);
		}
	}

	for(unsigned int i = 0; i < m_limbs.size(); i++) {
		if(i < m_skins.size())
			m_limbs[i].setSkin(m_skins[i]);
	}

	return true;
}

bool MainScreen::EventNewLimbButtonClicked(const CEGUI::EventArgs& e) {
	std::cout << "New Limb" << std::endl;

	AnimationModule::AugLimb limb(m_limbs.size());
	if(m_skins.size() >= m_limbs.size() + 1) limb.setSkin(m_skins[m_limbs.size()]);

	m_limbs.push_back(limb);
	m_limbsVisibility.push_back(true);

	return true;
}

bool MainScreen::EventMakeAllVisibleButtonClicked(const CEGUI::EventArgs& e) {
	for(unsigned int i = 0; i < m_limbsVisibility.size(); i++) {
		m_limbsVisibility[i] = true;
	}
}

bool MainScreen::EventRemoveLimbButtonClicked(const CEGUI::EventArgs& e) {
	std::cout << "Remove Limb" << std::endl;

	if(m_limbs.size() > m_selectedLimb) {
		for(unsigned int i = m_selectedLimb; i < m_limbs.size() - 1; i++) {
			m_limbs[i] = m_limbs[i + 1];
			m_limbsVisibility[i] = m_limbsVisibility[i + 1];
		}
		for(unsigned int j = 0; j < m_keyframes.size(); j++) {
			for(unsigned int k = m_selectedLimb; k < m_keyframes[j].angles.size() - 1; k++) {
				m_keyframes[j].angles[k] = m_keyframes[j].angles[k + 1];
				m_keyframes[j].offsets[k] = m_keyframes[j].offsets[k + 1];
			}
			m_keyframes[j].angles.pop_back();
			m_keyframes[j].offsets.pop_back();
		}
		m_limbs.pop_back();
		m_limbsVisibility.pop_back();
	}

	return true;
}

bool MainScreen::EventRotateOffsetLimbButtonClicked(const CEGUI::EventArgs& e) {
	std::cout << "Rotate/Offset" << std::endl;

	if(m_tool == ToolType::OFFSET) {
		m_tool = ToolType::ROTATE;
		m_rotateOffsetButton->setText("Rotating");
	} else if(m_tool == ToolType::ROTATE) {
		m_tool = ToolType::OFFSET;
		m_rotateOffsetButton->setText("Moving");
	}

	return true;
}

bool MainScreen::EventKeyFrameClicked(const CEGUI::EventArgs& e) {
	std::cout << "Key Frame Selected: ";

	unsigned int index = m_keyframesList->getItemIndex(m_keyframesList->getFirstSelectedItem());
	m_selectedFrame = index;

	std::cout << index << std::endl;

	for(unsigned int i = 0; i < m_limbs.size(); i++) {
		m_limbs[i].setAngle(m_keyframes[index].angles[i + index * m_limbs.size()]);
		m_limbs[i].setOffset(m_keyframes[index].offsets[i + index * m_limbs.size()]);
	}

	return false;
}


bool MainScreen::EventPlayPauseButtonClicked(const CEGUI::EventArgs& e) {
	std::cout << "Play/Pause" << std::endl;

	m_previewing = !m_previewing;

	if(m_previewing) {
		m_playPauseButton->setText("Pause");
	} else {
		m_playPauseButton->setText("Play");
	}

	for(unsigned int i = 0; i < m_limbs.size(); i++) {
		//m_limbs[i].activateSkeletalAnimation(getSkeletal());
	}

	return true;
}

bool MainScreen::EventAddFrameButtonClicked(const CEGUI::EventArgs& e) {
	std::cout << "Add Frame" << std::endl;

	KeyFrame kf;
	kf.angles.resize(m_limbs.size() + 1);
	kf.offsets.resize(m_limbs.size() + 1);
	m_keyframes.push_back(kf);

	unsigned int ms = 1000 / TICK_RATE;

	CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem("Keyframe (" + std::to_string(ms) + "ms)");

	item->setTextColours(CEGUI::Colour(0.0f, 0.0f, 0.0f, 1.0f));
	item->setSelectionBrushImage("WindowsLook/Background");
	item->setSelectionColours(CEGUI::Colour(0.7f, 0.7f, 0.7f, 1.0f));

	m_keyframesList->addItem(item);
	m_keyframesList->ensureItemIsVisible(item);

	return true;
}

bool MainScreen::EventRemoveFrameButtonClicked(const CEGUI::EventArgs& e) {
	std::cout << "Remove Frame" << std::endl;

	CEGUI::ListboxItem* item = m_keyframesList->getFirstSelectedItem();

	if(item) {
		m_keyframesList->removeItem(item);
	}

	for(unsigned int i = m_selectedFrame; i < m_keyframes.size() - 1; i++) {
		m_keyframes[i] = m_keyframes[i + 1];
	}
	m_keyframes.pop_back();

	return true;
}
