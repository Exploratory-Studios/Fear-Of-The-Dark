#pragma once

#include <GUI.h>
#include <Camera2D.h>

#include "EntityManager.h"
#include "AudioManager.h"
#include "World.h"

class Singletons
{
	public:
		static EntityManager* getEntityManager()
		{
			if(m_entityManager) return m_entityManager;
			m_entityManager = new EntityManager();
			return m_entityManager;
		}

		static AudioManager* getAudioManager()
		{
			if(m_audioManager) return m_audioManager;
			m_audioManager = new AudioManager();
			return m_audioManager;
		}

		static GLEngine::GUI* getGUI()
		{
			if(m_gui) return m_gui;
			m_gui = new GLEngine::GUI();
			return m_gui;
		}

		static GLEngine::Camera2D* getGameCamera()
		{
			if(m_gameCamera) return m_gameCamera;
			m_gameCamera = new GLEngine::Camera2D();
			return m_gameCamera;
		}

		static World* getWorld()
		{
			if(m_world) return m_world;
			m_world = new World(true);
			return m_world;
		}

		static void createWorld()
		{
			if(m_world) delete m_world;
			m_world = new World(true);
		}

		static void setWorld(World* w)
		{
			if(m_world) delete m_world;
			m_world = w;
		}

	private:
		static EntityManager* m_entityManager;
		static AudioManager* m_audioManager;
		static GLEngine::GUI* m_gui;
		static GLEngine::Camera2D* m_gameCamera;
		static World* m_world;
};
