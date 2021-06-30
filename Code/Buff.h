#pragma once

#include <string>
#include <SpriteBatch.h>

class Entity;

class Buff {
  public:
	Buff(Entity* owner, unsigned int id);
	void init();

	void tick();
	void draw(GLEngine::SpriteBatch& sb, glm::vec4& destRect, float& depth);

	const std::string  getName() const;
	const std::string  getDescription() const;
	const bool		   isActive() const;
	const unsigned int getTime() const;

  protected:
	unsigned int m_duration;
	unsigned int m_id;
	std::string	 m_name;
	std::string	 m_description;
	unsigned int m_tickScript;
	GLuint		 m_textureId;
	Entity*		 m_owner = nullptr;
};
