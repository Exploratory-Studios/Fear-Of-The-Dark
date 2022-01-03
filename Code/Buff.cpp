#include "Buff.h"

#include <ResourceManager.hpp>
#include <LuaScriptQueue.hpp>
#include <XMLDataManager.hpp>

#include "CustomXMLTypes.h"

#include "Entity.h"

Buff::Buff(Entity* owner, unsigned int id) {
	m_owner = owner;

	m_id = id;

	init();
}

void Buff::init() {
	XMLModule::BuffData d = getBuffData(m_id);

	m_textureId	  = d.texture.id;
	m_duration	  = d.duration;
	m_name		  = d.name;
	m_description = d.description;
	m_tickScript  = d.tickScript;
}

void Buff::tick() {
	if(isActive()) {
		m_duration--;
		BARE2D::LuaScriptQueue::getInstance()->addLuaScript(m_tickScript);
	}
}

void Buff::draw(BARE2D::BasicRenderer* renderer, glm::vec4& destRect, float& depth) {
	renderer->draw(destRect, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_textureId, depth, BARE2D::Colour(255, 255, 255, 255));
}

const std::string Buff::getName() const {
	return m_name;
}

const std::string Buff::getDescription() const {
	return m_description;
}

const bool Buff::isActive() const {
	return m_duration > 0;
}

const unsigned int Buff::getTime() const {
	return m_duration;
}
