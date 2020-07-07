#pragma once

#include "Animation.h"

namespace AnimationModule {

	class AugAnimation : public Animation {
		public:
			AugAnimation(GLEngine::GLTexture tex, float y, float frameWidth, float frameHeight);
			~AugAnimation() {}

	};

	class AugSkeletalAnimation : public SkeletalAnimation {
		public:
			AugSkeletalAnimation() : SkeletalAnimation() {}

			void setOffsets(std::vector<glm::vec2>& offsets) {
				m_offsets = offsets;
			}

			void setAngles(std::vector<float>& angles) {
				m_angles = angles;
			}

			void setNumLimbs(unsigned int limbs) {
				m_numLimbs = limbs;
			}
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
			Animation& getAnimation() {
				return m_idleAnimation;
			}
			SkeletalAnimation& getSkeletalAnimation() {
				return m_activeAnimation;
			}
	};

}
