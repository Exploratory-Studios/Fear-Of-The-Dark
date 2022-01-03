#include "Singletons.h"

EntityManager*	   Singletons::m_entityManager = nullptr;
GameAudioManager*  Singletons::m_audioManager  = nullptr;
BARE2D::BARECEGUI* Singletons::m_gui		   = nullptr;
BARE2D::Camera2D*  Singletons::m_gameCamera	   = nullptr;
World*			   Singletons::m_world		   = nullptr;
