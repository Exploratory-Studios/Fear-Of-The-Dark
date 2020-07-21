#pragma once

#include <GUI.h>

namespace QuestModule {
	class QuestManager;
}

namespace DialogueModule {

	class ResponseFrame {
		public:
			ResponseFrame(CEGUI::FrameWindow* parent, unsigned int responseID, unsigned int index);
			ResponseFrame(CEGUI::FrameWindow* parent, std::string text, unsigned int index);
			~ResponseFrame();

		private:
			void init(CEGUI::FrameWindow* parent, std::string text, unsigned int index);

			CEGUI::PushButton* m_button = nullptr;
			//CEGUI::DefaultWindow* m_text = nullptr;
	};

	class DialogueFrame {
			/// For GUI
		public:
			DialogueFrame(QuestModule::QuestManager* qm);
			~DialogueFrame();

			void show();
			void hide();

			void init(unsigned int questionID);

		private:
			void deleteResponses();
			void generateResponses(unsigned int questionID);

			QuestModule::QuestManager* m_qm = nullptr;
			CEGUI::FrameWindow* m_frame = nullptr;
			CEGUI::DefaultWindow* m_text = nullptr;
			std::vector<ResponseFrame*> m_responses;
	};

	class DialogueManager {
			/**
				Responsibilities of class "DialogueManager":
				 - Provide functions to start dialogue
				 - Run scripts that go along with dialogue
				 - Run dialogue (Modifying UI, flags, making sure flags are required, etc.)
				 - Hold current dialogue for getters
				 - Hold current state (in dialogue, out of dialogue)
			**/
		public:
			DialogueManager(QuestModule::QuestManager* qm); // Constructor
			~DialogueManager(); // Destructor

			void activateDialogue(unsigned int questionID);

		private:
			DialogueFrame* m_dialogueGUI = nullptr;

			QuestModule::QuestManager* m_qm = nullptr;

			bool m_inDialogue = false; // Set by activateDialogue();
			unsigned int m_currentQuestion = (unsigned int)(-1);  // Set by selecting a response. This is the ID according to XML stuff.
	};

}
