#pragma once

#include <string>
#include <BasicRenderer.hpp>
#include <LuaScript.hpp>

class Entity;

class Buff {
  public:
	Buff(Entity* owner, unsigned int id);
	void init();

	void tick();
	void draw(BARE2D::BasicRenderer* renderer, glm::vec4& destRect, float& depth);

	const std::string  getName() const;
	const std::string  getDescription() const;
	const bool		   isActive() const;
	const unsigned int getTime() const;

  protected:
	unsigned int m_duration;
	unsigned int m_id;
	std::string	 m_name;
	std::string	 m_description;
	BARE2D::LuaScript m_tickScript;
	GLuint		 m_textureId;
	Entity*		 m_owner = nullptr;
};
