#include "AugAnimation.h"

namespace AnimationModule {

	AugAnimation::AugAnimation(GLEngine::GLTexture tex, float y, float frameWidth, float frameHeight) : Animation() {
		m_textureID = tex.id;
		m_width = tex.width;
		float height = tex.height;
		m_frameWidth = frameWidth;
		m_frameHeight = frameHeight;

		float framesVert = height / frameHeight;

		m_uv.x = 0.0f;
		m_uv.y = (((framesVert - 1 - y) * frameHeight) / height);
		m_uv.z = (float)(m_frameWidth) / (float)(m_width);
		m_uv.w = (float)(frameHeight) / (float)(height);
	}

	AugLimb::AugLimb(unsigned int index) : Limb() {
		m_index = index;

		m_offset = glm::vec2(0.0f);
	}

}
