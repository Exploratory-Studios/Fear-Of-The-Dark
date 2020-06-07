#version 130
//The fragment shader operates on each pixel in a given polygon

in vec2 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 color;

uniform sampler2D textureSampler;

uniform vec2 screenSizeU;
uniform float daylight;
uniform float playerXPos; // Player's depth into the chunk (in the x direction) (from 0-1)
uniform float parallaxZoom;

float dist(vec2 a, vec2 b) {
	return sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y));
}

vec4 softLightBlend(vec4 a, vec4 b) {
	// uses pegtop formula	
	return (vec4(1.0) - vec4(2.0)*b)*pow(a, vec4(2)) + vec4(2.0)*b*a;
}

vec4 overlayBlend(vec4 a, vec4 b) {
	vec3 rgb = vec3(1) - vec3(2)*(vec3(1)-a.rgb)*(vec3(1)-b.rgb);
	float alpha = 1.0 - (1.0 - a.a) * (1.0 - b.a);
	return vec4(rgb.rgb, alpha);
}

void main() {
	vec2 uv = ((fragmentPosition.xy - screenSizeU.xy * vec2(0.5)) * vec2(parallaxZoom) + screenSizeU.xy * vec2(0.5)) / screenSizeU.xy + vec2((playerXPos-0.5)*(1-parallaxZoom),0);

	float playerDepth = clamp((-abs(playerXPos - 0.5) * 1.1 + 0.5) * 2.0, 0.0, 1.0);
	vec4 textureColour = texture(textureSampler, vec2(uv.x, 1.0-uv.y)) * daylight * pow(playerDepth, 1.0 / 3.0);

	color = textureColour * daylight;
}

/* Standalone sun effect:

uniform float time;


	vec2 sunPos = vec2(0.5, -cos(daylight) * 1.5 + 1.7)  * screenSizeU;

	float gradient = 0.6;
	float skyMult = 25.0;
	float sunMult = 0.8;

	float EL = 1.0 / pow(dist(fragmentPosition.xy,sunPos) * sunMult, gradient) * skyMult;
	vec4 sunEffect = vec4(EL * 0.3 * daylight, EL * 0.4 * daylight, EL * daylight, 1.0-textureColour.a);

	color = sunEffect;
*/


//vec2 sunPos = vec2(0.5, -cos(daylight) * 2.0 + 1.75)  * screenSizeU;

//float EL = 1.0 / pow(dist(uv,sunPos),1); // Good one for stars
