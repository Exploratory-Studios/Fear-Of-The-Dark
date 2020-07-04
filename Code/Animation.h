#pragma once

#include "XMLData.h"

#include <glm/glm.hpp>
#include <SpriteBatch.h>
#include <GLTexture.h>

#include "Animation.h"

namespace AnimationModule {

	unsigned int m_textureID;
	unsigned int m_width, m_height, m_y;
	unsigned int m_frameWidth, m_frameHeight;

	class Animation {
		public:
			Animation();
			Animation(unsigned int id) {
				init(id);
			}

			void init(unsigned int id) {
				XMLModule::AnimationData d = XMLModule::XMLData::getAnimationData(id);

				GLEngine::GLTexture tex = GLEngine::ResourceManager::getTexture(d.texture);

				m_textureID = tex.id;
				m_width = tex.width;
				float height = tex.height;
				float y = d.y;
				m_frameWidth = d.width;
				float frameHeight = d.height;

				m_uv.x = 0.0f;
				m_uv.y = (float)(y * frameHeight) / (float)(height);
				m_uv.z = (float)(m_frameWidth) / (float)(m_width);
				m_uv.z = (float)(frameHeight) / (float)(height);
			}

			void draw(::GLEngine::SpriteBatch& sb, glm::vec4& destRect, float& depth) {
				m_uv.x = (float)(m_currentFrame * m_frameWidth) / (float)(m_width);

				sb.draw(destRect, m_uv, m_textureID, depth, GLEngine::ColourRGBA8(255, 255, 255, 255));
			}

			void update() {
				m_currentFrame++;
			}

		private:
			unsigned int m_currentFrame = 0;

			unsigned int m_textureID;
			unsigned int m_width;
			unsigned int m_frameWidth;

			glm::vec4 m_uv;
	};

}
