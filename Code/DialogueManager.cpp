#include "DialogueManager.h"

#include "XMLData.h"

#include "QuestManager.h"

namespace DialogueModule {

	ResponseFrame::ResponseFrame(GLEngine::GUI* gui, CEGUI::FrameWindow* parent, unsigned int responseID, unsigned int index) {
		// Gets text, and name from XMLData (index is used for placement)

		XMLModule::DialogueResponseData d = XMLModule::XMLData::getDialogueResponseData(responseID);

		std::string text;
		text = d.text;

		init(gui, parent, text, index);
	}

	ResponseFrame::ResponseFrame(GLEngine::GUI* gui, CEGUI::FrameWindow* parent, std::string text, unsigned int index) {
		init(gui, parent, text, index);
	}

	ResponseFrame::~ResponseFrame() {
		// This actually destroys the widgets used, so that they can be reused with the same names.
		CEGUI::WindowManager::getSingleton().destroyWindow(m_button);
	}

	void ResponseFrame::init(GLEngine::GUI* gui, CEGUI::FrameWindow* parent, std::string text, unsigned int index) {
		// Should generate frame widget
		float height = 0.3f;
		float padding = 0.05f;

		m_button = static_cast<CEGUI::PushButton*>(gui->createWidget(parent, "FOTDSkin/Button", glm::vec4(0.1f, 0.35f + (height + padding) * index, 0.8f, height), glm::vec4(0.0f), "DialogueResponseFrame" + std::to_string(index)));
		m_button->setText(text);
	}

	DialogueFrame::DialogueFrame(GLEngine::GUI* gui, QuestModule::QuestManager* qm) : m_gui(gui), m_qm(qm) {
		m_frame = static_cast<CEGUI::FrameWindow*>(m_gui->createWidget("FOTDSkin/FrameWindow", glm::vec4(0.1f, 0.6f, 0.8f, 0.3f), glm::vec4(0.0f), "DialogueMainFrame"));
		m_frame->setCloseButtonEnabled(false);
		m_frame->setDragMovingEnabled(false);
		m_frame->setRollupEnabled(false);
		m_frame->setTitleBarEnabled(false);

		m_text = static_cast<CEGUI::DefaultWindow*>(m_gui->createWidget(m_frame, "FOTDSkin/Label", glm::vec4(0.1f, 0.1f, 0.8f, 0.3f), glm::vec4(0.0f), "DialogueMainFrameText"));

		hide();
	}

	DialogueFrame::~DialogueFrame() {
		// This actually destroys the widgets used, so that they can be reused with the same names laters.
		deleteResponses();

		CEGUI::WindowManager::getSingleton().destroyWindow(m_frame);
		CEGUI::WindowManager::getSingleton().destroyWindow(m_text);
	}

	void DialogueFrame::show() {
		// Should set the frame to shown and enabled
		m_frame->setVisible(true);
		m_frame->enable();
	}

	void DialogueFrame::hide() {
		// Should set the frame to hidden and disabled
		m_frame->setVisible(false);
		m_frame->disable();
	}

	void DialogueFrame::init(unsigned int questionID) {
		deleteResponses();
		generateResponses(questionID);

		XMLModule::DialogueQuestionData d = XMLModule::XMLData::getDialogueQuestionData(questionID);
		m_text->setText(d.text);
	}

	void DialogueFrame::deleteResponses() {
		/// Should delete all ResponseFrames
		for(unsigned int i = 0; i < m_responses.size(); i++) {
			delete m_responses[i]; // destroys used widgets.
		}
	}

	void DialogueFrame::generateResponses(unsigned int questionID) {
		/// Should fetch the IDs of all responses for the question from the XMLData manager, check if the responses should be shown, then init only responses that should be shown.

		// Fetch all IDs
		XMLModule::DialogueQuestionData d = XMLModule::XMLData::getDialogueQuestionData(questionID);

		// Get responses that should be shown:
		std::vector<unsigned int> shownResponses;
		for(unsigned int i = 0; i < d.nextResponses.size(); i++) {
			// Get response data
			XMLModule::DialogueResponseData r = XMLModule::XMLData::getDialogueResponseData(d.nextResponses[i]);

			// Check if all flags are true
			bool shouldShow = true; // Stays true if all flags are true.
			for(unsigned int j = 0; j < r.requiredFlags.size(); j++) {
				if(!m_qm->getFlag(r.requiredFlags[j])) {
					shouldShow = false;
					break; // Flag not true, we should skip the rest of the flags and check the next response.
				}
			}

			// If it's shown, add it to the shownResponses
			if(shouldShow) {
				shownResponses.push_back(d.nextResponses[i]);
			}
		}

		// Init only responses that should be shown
		for(unsigned int i = 0; i < shownResponses.size(); i++) {
			ResponseFrame* response = new ResponseFrame(m_gui, m_frame, shownResponses[i], i);

			m_responses.push_back(response);
		}

		// Fin!

	}

	DialogueManager::DialogueManager(GLEngine::GUI* gui, QuestModule::QuestManager* qm) {
		// Constructor
		/// Should create a new DialogueFrame using the GUI.
		m_dialogueGUI = new DialogueFrame(gui, qm);
	}

	DialogueManager::~DialogueManager() {
		// Destructor
		/// Delete m_dialogueGUI (destroys all widgets)
		delete m_dialogueGUI;
	}

	void DialogueManager::activateDialogue(unsigned int questionID) {
		/// Should initialize the DialogueFrame, set the current dialogue ID, and set the state to true.
		m_currentQuestion = questionID;
		m_inDialogue = true;

		m_dialogueGUI->init(m_currentQuestion);
		m_dialogueGUI->show();
	}
}
