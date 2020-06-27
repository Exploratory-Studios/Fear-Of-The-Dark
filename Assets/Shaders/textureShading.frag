#version 330
//The fragment shader operates on each pixel in a given polygon

in vec3 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;
in vec4 fragmentLight;

in vec4 lightSource; // just a normalized vector pointing TO light source

//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 color;

uniform sampler2D textureSampler;
uniform sampler2D bumpSampler;

uniform vec2 screenSizeU;

const float Pi = 6.28318530718; // Pi*2

float linearInterpolate(float min, float max, float percentage) {
	return ((percentage)*(max-min)) + min;
}

float cosineInterpolate(float min, float max, float percentage) { // cosine interpolation
	float mu2;

   	mu2 = (1.0 - cos(percentage * 3.141592)) / 2.0;
   	return (min * (1.0 - mu2) + max * mu2);
}

float interpolateCornersforCoords(vec2 coords, vec4 corners) {
	float topX = cosineInterpolate(corners.x, corners.y, coords.x);
	float bottomX = cosineInterpolate(corners.w, corners.z, coords.x);

	return cosineInterpolate(bottomX, topX, coords.y);
}

void main() {
	vec4 textureColour = vec4(0.0);
	textureColour = texture(textureSampler, fragmentUV);

	if(textureColour.a <= 0.01f) {
		discard;
	}

	gl_FragDepth = fragmentPosition.z;


	float lightLevel = 0.0;

	vec4 source = lightSource;

	source += vec4(vec2(0.5, 5.0) - vec2(fragmentUV.x, 1.0 - fragmentUV.y), 1.0, interpolateCornersforCoords(vec2(fragmentUV.x, 1.0 - fragmentUV.y), fragmentLight));
	source.xyz /= vec3(2.0);
	source.xy = normalize(source.xy);

	vec3 normalMap = normalize(texture(bumpSampler, fragmentUV).rgb * 2.0 - 1.0);

	lightLevel = clamp(dot(normalMap, source.xyz), 0.0, 1.0);
	lightLevel = clamp(lightLevel * source.w, 0.0, 1.25);

	color = textureColour * fragmentColour * vec4(lightLevel, lightLevel, lightLevel, 1.0);
}

