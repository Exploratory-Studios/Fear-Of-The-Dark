#include "Animation.h"

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
		m_width = tex.width;
		float height = tex.height;
		float y = d.y;
		m_frameWidth = d.width;
		float frameHeight = d.height;

		float framesVert = height / frameHeight;

		m_uv.x = 0.0f;
		m_uv.y = (((framesVert - 1 - y) * frameHeight) / height);
		m_uv.z = (float)(m_frameWidth) / (float)(m_width);
		m_uv.w = (float)(frameHeight) / (float)(height);
		//m_uv.y = (float)(y * frameHeight) / (float)(height);
		//m_uv.z = (float)(m_frameWidth) / (float)(m_width);
		//m_uv.z = (float)(frameHeight) / (float)(height);
	}

	void Animation::draw(::GLEngine::SpriteBatch& sb, glm::vec4& destRect, float& depth, float& angle) {
		m_uv.x = (float)(m_currentFrame * m_frameWidth) / (float)(m_width);

		sb.draw(destRect, m_uv, m_textureID, depth, GLEngine::ColourRGBA8(255, 255, 255, 255), angle);
	}

	void Animation::update() {
		if(!isFinished()) {
			m_currentFrame++;
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

		m_angles = d.angles;
		m_offsets = d.offsets;
	}

	void SkeletalAnimation::updateLimb(Limb* limb) {
		// Interpolate betweem this keypoint and next point. One can assume that this is called every frame,
		// so a constant value (an integral from one angle/place to the next) can be added each frame.

		unsigned int elementI = m_currentFrame * m_numLimbs + limb->getIndex();

		glm::vec2 diffPos = m_offsets[elementI + 1] - m_offsets[elementI];
		float diffAngle = m_angles[elementI + 1] - m_angles[elementI];

		glm::vec2 integralPos = diffPos / glm::vec2(FRAME_RATE / TICK_RATE);
		float integralAngle = diffAngle / (FRAME_RATE / TICK_RATE);

		limb->setPosition(limb->getPosition() + integralPos);
		limb->setAngle(limb->getAngle() + integralAngle);
	}

	void SkeletalAnimation::update() {
		if(!isFinished()) {
			m_currentFrame++;
		}
	}

	bool SkeletalAnimation::isFinished() {
		return (m_currentFrame >= m_angles.size());
	}

	void SkeletalAnimation::restart() {
		m_currentFrame = 0;
	}

	Limb::Limb() {

	}

	Limb::Limb(Entity* owner, Animation idleAnimation, unsigned int index); // The idleAnimation is the animation that constantly runs. Most of the time, this is just a single-textured sprite which supplies the texture

	void Limb::init(Entity* owner, Animation idleAnimation, unsigned int index) {
		m_owner = owner;
		m_idleAnimation = idleAnimation;
		m_index = index;
	}

	void Limb::activateSkeletalAnimation(SkeletalAnimation anim) {
		// Sets current m_activeAnimation and sets m_animationTime to 0.0f
		m_activeAnimation = anim;
		m_isAnimated = true;
	}

	void Limb::tick() {
		// Sets the idle animation to the next state. If active, continues the skeletal animation too. If the skeletal animation finishes, set m_animationTime to -1.0f;
		m_idleAnimation.update();
		if(m_isAnimated) {
			m_activeAnimation.update();
		}
	}

	void Limb::draw(GLEngine::SpriteBatch& sb) {
		// Draws based on the owner.
		glm::vec4 destRect(m_owner->getPosition() + m_offset, m_owner->getSize());
		m_idleAnimation.draw(sb, destRect, m_owner->getDepth(), m_angle);

		m_activeAnimation.updateLimb(this);
	}

	bool Limb::isAnimationActive() {
		if(m_activeAnimation) {
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
