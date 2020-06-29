#version 130

in vec3 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

out vec4 colour;

uniform sampler2D textureSampler;
uniform sampler2D depthMap;
uniform sampler2D sunlightMap;

const int MAX_LIGHTS = 30; // This is the max amnt of lights a scene can manage (MAX_LIGHTS_RENDERED)

uniform vec3 lights[MAX_LIGHTS]; // First three components are XYZ, 4th is intensity

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

float downBlur(sampler2D sampler) {
	float res = 0.0;

	res = texture(sampler, fragmentUV.xy).r;
	for(float i = 0; i < 0.05; i += 0.01) {
		res += texture(sampler, fragmentUV.xy + vec2(0.0, i)).r;
	}

	return res / 6.0;
}

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

float getLightLevel() {
	float lightLevel = 0.0f;
	for(int i = 0; i < MAX_LIGHTS; i++) {
		float intensity = lights[i].z / pow(distance(fragmentPosition.xy, lights[i].xy), 2.0); // relative intensity

		if(intensity > 0.01) {

		    lightLevel += intensity;
		}
	}
	return clamp(lightLevel, 0.0, 1.4);
}

float getSunlight() {
	return texture(sunlightMap, fragmentUV.xy).r;
}

void main() 
{ 
	float depth = texture(depthMap, fragmentUV).r; // Get depth (It's set to the exact values that SpriteBatch wrote to the buffer with.) (0 is closest to camera, 1 is farthest)
	float depthDiff = abs(playerDepth - depth); // Difference from player, for a focused effect when the player is on the same layer. (Just for blur)

	// Final
	colour = blur(16.0, 3.0, 0.008 * depthDiff, textureSampler, fragmentUV);
	float c = 1.0 / map(depth-0.1, 0.0, 0.9, 1.0, 4.0);
	colour.rgb *= c;
	colour.rgb *= getLightLevel() + getSunlight();
	if(fragmentUV.x > 0.5) {
		colour.rgb = vec3(texture(sunlightMap, fragmentUV.xy).r);
	}
}
