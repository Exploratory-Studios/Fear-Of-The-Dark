#version 130
//The fragment shader operates on each pixel in a given polygon

in vec2 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 color;

uniform vec2 screenSizeU;
uniform vec2 sourcePosition;

float dist(vec2 p1, vec2 p2) {
	return sqrt((p1*p1) + (p2*p2));
}

void main() {
	float d = dist(sourcePosition, fragmentPosition);

	color.r = 1.0/d;
	color.g = 1.0/d;
	color.b = 1.0/d;
}

