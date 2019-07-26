#version 130
//The fragment shader operates on each pixel in a given polygon

in vec2 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 color;

uniform vec2 screenSizeU;
uniform float time;
uniform float daylight;

float dist(vec2 a, vec2 b) {

	return sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y));
}

void main() {
	vec2 uv = fragmentPosition.xy;// / screenSizeU.xy;

	//vec2 sunPos = vec2((1.0 - abs(0.5 - uv.x)), (1.0 - abs(0.5 - (uv.y + cos(daylight) * 2.5 - 1.5))));

	vec2 sunPos = vec2(0.5, -cos(daylight) * 1.5 + 1.7)  * screenSizeU;

	float gradient = 0.6;
	float skyMult = 25.0;
	float sunMult = 0.8;

	float EL = 1.0 / pow(dist(uv,sunPos) * sunMult, gradient) * skyMult;

	color = vec4(EL * 0.3 * daylight, EL * 0.4 * daylight, EL * daylight, 1.0f);

	if(daylight > 0.3 && daylight < 0.7) {
        color.r += abs(0.2 - abs(0.5 - daylight)) * 2.0 * time / time;
        color.b += abs(0.2 - abs(0.5 - daylight)) * 1.0;
	}
}



//vec2 sunPos = vec2(0.5, -cos(daylight) * 2.0 + 1.75)  * screenSizeU;

//float EL = 1.0 / pow(dist(uv,sunPos),1); // Good one for stars