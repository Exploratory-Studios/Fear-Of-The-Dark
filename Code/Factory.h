#pragma once

#include "EntityManager.h"
#include "AudioManager.h"

class Factory {
	public:
		static EntityManager* getEntityManager() {
			if(m_entityManager) return m_entityManager;
			m_entityManager = new EntityManager();
			return m_entityManager;
		}

		static AudioManager* getAudioManager() {
			if(m_audioManager) return m_audioManager;
			m_audioManager = new AudioManager();
			return m_audioManager;
		}

	private:
		static EntityManager* m_entityManager;
		static AudioManager* m_audioManager;
};
