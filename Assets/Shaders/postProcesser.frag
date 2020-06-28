#version 130

in vec3 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

out vec4 colour;

uniform sampler2D textureSampler;
uniform sampler2D depthMap;

uniform float playerDepth;

const float Pi = 6.28318530718; // Pi*2

vec4 blur(float directions, float quality, float size, sampler2D textureS, vec2 uv) {
	vec2 radius = vec2(size);
	
	// Pixel colour
	vec4 pixColour = texture(textureS, uv);

	if(pixColour.a <= 0.01f) {
		discard;
	}
	// Blur calculations
	for( float d=0.0; d<Pi; d+=Pi/directions)
	{
		for(float i=1.0/quality; i<=1.0; i+=1.0/quality)
		{
			pixColour += texture(textureS, uv+vec2(cos(d),sin(d))*radius*i);		
		}
	}

	// Output to screen
	pixColour /= quality * directions;
	return pixColour;
}

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main() 
{ 
	float depth = texture(depthMap, fragmentUV).r; // Get depth (It's set to the exact values that SpriteBatch wrote to the buffer with.) (0 is closest to camera, 1 is farthest)
	float depthDiff = abs(playerDepth - depth); // Difference from player, for a focused effect when the player is on the same layer. (Just for blur)

	
	// Final
	colour = blur(16.0, 3.0, 0.008 * depthDiff, textureSampler, fragmentUV);
	float c = 1.0f / map(depth, 0.0, 1.0, 1.0, 4.0);
	colour.rgb *= c;
}
