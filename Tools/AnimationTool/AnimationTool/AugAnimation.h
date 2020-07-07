#pragma once

#include "Animation.h"

namespace AnimationModule {

	class AugAnimation : public Animation {
		public:
			AugAnimation(GLEngine::GLTexture tex, float y, float frameWidth, float frameHeight);
			~AugAnimation() {}

	};

	class AugLimb : public Limb {
		public:
			AugLimb(unsigned int index);
			~AugLimb() {}

			void setSkin(Animation skin) {
				m_idleAnimation = skin;
			}
			void setIndex(unsigned int index) {
				m_index = index;
			}
			Animation getAnimation() {
				return m_idleAnimation;
			}
	};

}
