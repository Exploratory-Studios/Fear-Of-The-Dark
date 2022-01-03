#pragma once

#include <BARECEGUI.hpp>
#include <Camera2D.hpp>

#include "EntityManager.h"
#include "GameAudioManager.h"
#include "World.h"

class Singletons {
  public:
	static EntityManager* getEntityManager() {
		if(m_entityManager)
			return m_entityManager;
		m_entityManager = new EntityManager();
		return m_entityManager;
	}

	static void deleteEntityManager() {
		if(m_entityManager)
			delete m_entityManager;
		m_entityManager = nullptr;
	}

	static GameAudioManager* getAudioManager() {
		if(m_audioManager)
			return m_audioManager;
		m_audioManager = new GameAudioManager();
		return m_audioManager;
	}

	static BARE2D::BARECEGUI* getGUI() {
		if(m_gui)
			return m_gui;
		m_gui = BARE2D::BARECEGUI::getInstance();
		return m_gui;
	}

	static void destroyGUI() {
		m_gui = nullptr;
	}

	static void setGameCamera(BARE2D::Camera2D* cam) {
		m_gameCamera = cam;
	}

	static BARE2D::Camera2D* getGameCamera() {
		return m_gameCamera;
	}

	static World* getWorld() {
		if(m_world)
			return m_world;
		m_world = new World(true);
		return m_world;
	}

	static void createWorld() {
		if(m_world)
			delete m_world;
		m_world = new World(true);
	}

	static void setWorld(World* w) {
		if(m_world)
			delete m_world;
		m_world = w;
	}

  private:
	static EntityManager*	  m_entityManager;
	static GameAudioManager*  m_audioManager;
	static BARE2D::BARECEGUI* m_gui;
	static BARE2D::Camera2D*  m_gameCamera;
	static World*			  m_world;
};
