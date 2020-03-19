#version 130
//The fragment shader operates on each pixel in a given polygon

in vec2 fragmentPosition;
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

	float lightLevel = interpolateCornersforCoords(vec2(fragmentUV.x, 1.0 - fragmentUV.y), fragmentLight);

	//float rise1 = lightLevel - fragmentLight.x; // TL
	//float rise2 = lightLevel - fragmentLight.y; // TR
	//float rise3 = -(lightLevel - fragmentLight.z); // BR
	//float rise4 = -(lightLevel - fragmentLight.w); // BL
	//float rise = (rise1 + rise2 + rise3 + rise4) / 4.0;

	//float run1 = -(lightLevel - fragmentLight.x); // TL
	//float run2 = lightLevel - fragmentLight.y; // TR
	//float run3 = lightLevel - fragmentLight.z; // BR
	//float run4 = -(lightLevel - fragmentLight.w); // BL
	//float run = (run1 + run2 + run3 + run4) / 4.0;

	//vec3 lightSource = normalize(vec3(-run, -rise, 1.0));

	vec4 textureColour = texture(textureSampler, fragmentUV);

	vec3 normalMap = normalize(texture(bumpSampler, fragmentUV).rgb * 2.0 - 1.0);

	lightLevel *= clamp(dot(normalMap, lightSource * vec3(1.0, 1.0, -1.0)), 0.0, 1.0); // reverse z coordinate of lightsource to match direction of normal map's vector

	lightLevel /= clamp(dot(normalMap, lightSource * vec3(1.0, 1.0, -1.0)), 0.0, 1.0);

	color = textureColour * fragmentColour * vec4(lightLevel, lightLevel, lightLevel, 1.0);
}

