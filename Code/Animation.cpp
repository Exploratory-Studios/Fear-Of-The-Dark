#include "Animation.h"

#include "Entity.h"

#include <string>

#include <iostream>

namespace AnimationModule {

	Animation::Animation() {

	}

	Animation::Animation(unsigned int id) {
		init(id);
	}

	void Animation::init(unsigned int id) {
		XMLModule::AnimationData d = XMLModule::XMLData::getAnimationData(id);

		GLEngine::GLTexture tex = GLEngine::ResourceManager::getTexture(d.texture);

		m_textureID = tex.id;
		float height = tex.height;
		float y = d.y;
		m_frameWidth = d.width;
		m_frameHeight = d.height;
		m_width = d.frames * m_frameWidth;

		float framesVert = height / m_frameHeight;

		m_uv.x = 0.0f;
		m_uv.y = (((framesVert - 1 - y) * m_frameHeight) / height);
		m_uv.z = (float)(m_frameWidth) / (float)(m_width);
		m_uv.w = (float)(m_frameHeight) / (float)(height);
		//m_uv.y = (float)(y * frameHeight) / (float)(height);
		//m_uv.z = (float)(m_frameWidth) / (float)(m_width);
		//m_uv.z = (float)(frameHeight) / (float)(height);
	}

	void Animation::draw(::GLEngine::SpriteBatch& sb, GLEngine::ColourRGBA8 colour, glm::vec4& destRect, float& depth, float& angle, glm::vec2& COR) {
		m_uv.x = (float)(m_currentFrame * m_frameWidth) / (float)(m_width);

		sb.draw(destRect, m_uv, m_textureID, depth, colour, angle, COR);
	}

	void Animation::draw(::GLEngine::SpriteBatch& sb, GLEngine::ColourRGBA8 colour, glm::vec4& destRect, float& depth, glm::vec2 direction) {
		m_uv.x = (float)(m_currentFrame * m_frameWidth) / (float)(m_width);

		sb.draw(destRect, m_uv, m_textureID, depth, colour, direction);
	}

	void Animation::tick() {
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

	SkeletalAnimation::SkeletalAnimation(unsigned int id) {
		init(id);
	}

	void SkeletalAnimation::init(unsigned int id) {
		XMLModule::SkeletalAnimationData d = XMLModule::XMLData::getSkeletalAnimationData(id);

		m_currentFrame = 0;

		m_angles = d.angles;
		m_offsets = d.offsets;
		m_centresOfRotation = d.centresOfRotation;
		m_limbIndices = d.limbIndices;
		m_repeats = d.repeats;
	}

	void SkeletalAnimation::updateLimb(Limb* limb) {
		// Interpolate betweem this keypoint and next point. One can assume that this is called every frame,
		// so a constant value (an integral from one angle/place to the next) can be added each frame.

		{
			bool found = false;
			for(unsigned int i = 0; i < m_limbIndices.size(); i++) {
				if(m_limbIndices[i] == limb->getIndex()) {
					found = true;
				}
			}
			if(!found) return;
		}

		if(m_currentFrame < m_angles.size() - 1) {
			unsigned int elementI = m_currentFrame * m_limbIndices.size() + limb->getIndex();
			unsigned int nextElementI = (((m_currentFrame + 1) * m_limbIndices.size()) % m_angles.size()) + limb->getIndex();

			if(m_lastFrame == m_currentFrame) { // Continue interpolation
				glm::vec2 diffPos = m_offsets[nextElementI] - m_offsets[elementI];
				float diffAngle = m_angles[nextElementI] - m_angles[elementI];
				glm::vec2 diffCentre = m_centresOfRotation[nextElementI] - m_centresOfRotation[elementI];

				glm::vec2 integralPos = diffPos / glm::vec2(FRAME_RATE / TICK_RATE);
				float integralAngle = diffAngle / (FRAME_RATE / TICK_RATE);
				glm::vec2 integralCentre = diffCentre / glm::vec2(FRAME_RATE / TICK_RATE);

				glm::vec2 newOffset = limb->getOffset() + integralPos;
				float newAngle = limb->getAngle() + integralAngle;
				glm::vec2 newCentre = limb->getCentreOfRotation() + integralCentre;

				limb->setOffset(newOffset);
				limb->setAngle(newAngle);
				limb->setCentreOfRotation(newCentre);
			} else { // We're on a new frame. Make sure to set the angle, etc.
				limb->setOffset(m_offsets[elementI]);
				limb->setAngle(m_angles[elementI]);
				limb->setCentreOfRotation(m_centresOfRotation[elementI]);

				m_lastFrame = m_currentFrame;
			}
		}
	}

	void SkeletalAnimation::transitionLimb(Limb* limb) {
		// Move the limb from where it is to where the first frame of this animation is. This has to be done in some arbitrary amount of frames. This function is called every frame.
		const float maxMovement = 0.125f; // (.0625 tiles) How far anything can move in one frame MAXIMUM
		const float maxAngle = M_PI_4 / 16.0f / 2.0f; // (1/128 pi, 1.40625 degrees) How far anything can rotate in one frame MAXIMUM

		glm::vec2 diffPos = m_offsets[limb->getIndex()] - limb->getOffset();
		float diffAngle = m_angles[limb->getIndex()] - limb->getAngle();
		glm::vec2 diffCentre = m_centresOfRotation[limb->getIndex()] - limb->getCentreOfRotation();

		glm::vec2 integralPos = glm::clamp(diffPos, glm::vec2(-maxMovement), glm::vec2(maxMovement));
		float integralAngle = glm::clamp(diffAngle, -maxAngle, maxAngle);
		glm::vec2 integralCentre = glm::clamp(diffCentre, glm::vec2(-maxMovement), glm::vec2(maxMovement));

		glm::vec2 newOffset = limb->getOffset() + integralPos;
		float newAngle = limb->getAngle() + integralAngle;
		glm::vec2 newCentre = limb->getCentreOfRotation() + integralCentre;

		limb->setOffset(newOffset);
		limb->setAngle(newAngle);
		limb->setCentreOfRotation(newCentre);

		if(glm::distance(limb->getOffset(), m_offsets[limb->getIndex()]) <= 0.001f) { // Check if we're done
			if(std::abs(limb->getAngle() - m_angles[limb->getIndex()]) <= 0.001f) {
				if(glm::distance(limb->getCentreOfRotation(), m_centresOfRotation[limb->getIndex()]) <= 0.001f) {
					// Everything is within an unnoticable distance, so we can continue with regular updating.
					limb->activateSkeletalAnimation(this);
					m_changing = false;
				}
			}
		}
	}

	void SkeletalAnimation::tick() {
		if(!isFinished()) {
			m_currentFrame = (m_currentFrame + 1) % (m_offsets.size() / m_limbIndices.size());
		}
	}

	bool SkeletalAnimation::isFinished() {
		if(m_repeats) return false;
		if(m_angles.size() == 0) return true;
		return (m_currentFrame + 1 >= (m_angles.size() / m_limbIndices.size())); // This only returns if it doesn't repeat
	}

	void SkeletalAnimation::restart() {
		m_currentFrame = 0;
	}

	Limb::Limb() {

	}

	Limb::Limb(Animation idleAnimation, unsigned int index) {
		// The idleAnimation is the animation that constantly runs. Most of the time, this is just a single-textured sprite which supplies the texture
		init(idleAnimation, index);
	}

	void Limb::init(Animation idleAnimation, unsigned int index) {
		m_idleAnimation = idleAnimation;
		m_index = index;
		m_centreOfRotation = glm::vec2(0.5f);
	}

	void Limb::activateSkeletalAnimation(SkeletalAnimation* anim) {
		// Sets current m_activeAnimation and sets m_animationTime to 0.0f
		if(!anim->affectsLimb(m_index)) {
			return;
		}

		m_activeAnimation = anim;
		m_nextAnimation = nullptr; // Make sure that this animation is the one we're actually using now.

		m_offset = anim->getOffset(m_index);
		m_angle = anim->getAngle(m_index);
		m_centreOfRotation = anim->getCentreOfRotation(m_index);
	}

	void Limb::changeSkeletalAnimation(SkeletalAnimation* anim) {
		// Sets current m_activeAnimation and sets m_animationTime to 0.0f
		if(!anim->affectsLimb(m_index)) {
			return;
		}

		m_nextAnimation = anim;
	}

	void Limb::setAnimation(Animation& anim) {
		m_idleAnimation = anim;
	}

	void Limb::tick() {
		// Sets the idle animation to the next state. If active, continues the skeletal animation too. If the skeletal animation finishes, set m_animationTime to -1.0f;
		m_idleAnimation.tick();
	}

	void Limb::update() {
		if(m_nextAnimation) {
			m_nextAnimation->transitionLimb(this);
		} else if(isAnimationActive()) {
			m_activeAnimation->updateLimb(this);
		}
	}

	void Limb::draw(GLEngine::SpriteBatch& sb, GLEngine::ColourRGBA8 colour, glm::vec4 destRect, float& depth) {
		// Transform based on centre of rotation

		float xDist = (m_offset.x + destRect.z / 2.0f) - m_centreOfRotation.x * destRect.z;
		float yDist = (m_offset.y + destRect.w / 2.0f) - m_centreOfRotation.y * destRect.w;

		destRect.x += xDist * std::cos(m_angle) - yDist * std::sin(m_angle) + m_centreOfRotation.x * destRect.z;
		destRect.y += xDist * std::sin(m_angle) + yDist * std::cos(m_angle) + m_centreOfRotation.y * destRect.w;

		destRect.x -= destRect.z / 2.0f;
		destRect.y -= destRect.w / 2.0f;

		glm::vec2 c(0.5f, 0.5f);

		m_idleAnimation.draw(sb, colour, destRect, depth, m_angle, c);
	}

	bool Limb::isAnimationActive() {
		if(m_activeAnimation) {
			return !m_activeAnimation->isFinished();
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

	glm::vec2 Limb::getCentreOfRotation() {
		return m_centreOfRotation;
	}

	void Limb::setAngle(float& angle) {
		m_angle = angle;
	}

	void Limb::setOffset(glm::vec2& offset) {
		m_offset = offset;
	}

	void Limb::setCentreOfRotation(glm::vec2& centre) {
		m_centreOfRotation = centre;
	}



}
