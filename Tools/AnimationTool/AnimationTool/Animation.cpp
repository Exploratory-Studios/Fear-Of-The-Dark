#include "Animation.h"

#include <string>

#include <iostream>

#include "Presets.h"

namespace AnimationModule {

	Animation::Animation() {

	}

	void Animation::draw(::GLEngine::SpriteBatch& sb, glm::vec4& destRect, float& depth, float& angle) {
		m_uv.x = (float)(m_currentFrame * m_frameWidth) / (float)(m_width);

		glm::vec2 centre(0.5f, 0.5f);

		sb.draw(destRect, m_uv, m_textureID, depth, GLEngine::ColourRGBA8(255, 255, 255, 255), angle, centre);
	}

	void Animation::update() {
		if(!isFinished()) {
			m_currentFrame++;
		} else if(m_loops) {
			m_currentFrame = 0;
		}
	}

	bool Animation::isFinished() {
		/// Returns true if this animation's m_currentFrame is the last frame.
		return m_currentFrame == ((m_width / m_frameWidth) - 1);
	}

	void Animation::restart() {
		/// Resets current frame to 0
		m_currentFrame = 0;
	}

	SkeletalAnimation::SkeletalAnimation() {

	}

	void SkeletalAnimation::updateLimb(Limb* limb) {
		// Interpolate betweem this keypoint and next point. One can assume that this is called every frame,
		// so a constant value (an integral from one angle/place to the next) can be added each frame.

		if(m_currentFrame < m_angles.size() - 1) {
			unsigned int elementI = m_currentFrame * m_numLimbs + limb->getIndex();

			glm::vec2 diffPos = m_offsets[elementI + 1] - m_offsets[elementI];
			float diffAngle = glm::radians(m_angles[elementI + 1] - m_angles[elementI]);

			glm::vec2 integralPos = diffPos / glm::vec2(FRAME_RATE / TICK_RATE);
			float integralAngle = diffAngle / (FRAME_RATE / TICK_RATE);

			glm::vec2 newOffset = limb->getOffset() + integralPos;
			float newAngle = limb->getAngle() + integralAngle;

			limb->setOffset(newOffset);
			limb->setAngle(newAngle);

			std::cout << m_currentFrame << std::endl;

		}
	}

	void SkeletalAnimation::update() {
		if(!isFinished()) {
			m_currentFrame++;
		} else if(m_repeats) {
			m_currentFrame = 0;
		}
	}

	bool SkeletalAnimation::isFinished() {
		return (m_currentFrame >= m_angles.size() - 1);
	}

	void SkeletalAnimation::restart() {
		m_currentFrame = 0;
	}

	Limb::Limb() {

	}

	void Limb::activateSkeletalAnimation(SkeletalAnimation anim) {
		// Sets current m_activeAnimation and sets m_animationTime to 0.0f
		m_activeAnimation = anim;
		m_isAnimated = true;

		m_offset = anim.getOffset(m_index);
		m_angle = anim.getAngle(m_index);
	}

	void Limb::tick() {
		// Sets the idle animation to the next state. If active, continues the skeletal animation too. If the skeletal animation finishes, set m_animationTime to -1.0f;
		m_idleAnimation.update();
	}

	void Limb::update() {
		if(m_isAnimated) {
			m_activeAnimation.update();
		}
	}

	void Limb::draw(GLEngine::SpriteBatch& sb, glm::vec4 destRect, float& depth) {
		// Draws based on the owner.
		destRect.x += m_offset.x;
		destRect.y += m_offset.y;
		float angle = m_angle;//glm::radians(m_angle);
		m_idleAnimation.draw(sb, destRect, depth, angle);

		if(isAnimationActive()) m_activeAnimation.updateLimb(this);
	}

	bool Limb::isAnimationActive() {
		if(m_isAnimated) {
			return !m_activeAnimation.isFinished();
		}
		return false;
	};

	unsigned int Limb::getIndex() {
		return m_index;
	}

	float Limb::getAngle() {
		return m_angle;
	}

	glm::vec2 Limb::getOffset() {
		return m_offset;
	}

	void Limb::setAngle(float& angle) {
		m_angle = angle;
	}

	void Limb::setOffset(glm::vec2& offset) {
		m_offset = offset;
	}



}
