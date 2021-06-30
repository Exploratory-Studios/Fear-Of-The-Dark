#include "Buff.h"

#include <ResourceManager.h>

#include "ScriptQueue.h"
#include "XMLDataTypes.h"
#include "XMLData.h"

#include "Entity.h"

Buff::Buff(Entity* owner, unsigned int id) {
	m_owner = owner;

	m_id = id;

	init();
}

void Buff::init() {
	XMLModule::BuffData d = XMLModule::XMLData::getBuffData(m_id);

	m_textureId	  = GLEngine::ResourceManager::getTexture(d.texture).id;
	m_duration	  = d.duration;
	m_name		  = d.name;
	m_description = d.description;
	m_tickScript  = d.tickScript.getID();
}

void Buff::tick() {
	if(isActive()) {
		m_duration--;
		ScriptingModule::ScriptQueue::activateScript(m_tickScript, m_owner->generateLuaValues());
	}
}

void Buff::draw(GLEngine::SpriteBatch& sb, glm::vec4& destRect, float& depth) {
	sb.draw(destRect, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_textureId, depth, GLEngine::ColourRGBA8(255, 255, 255, 255));
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
