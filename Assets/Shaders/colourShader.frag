#version 130
//The fragment shader operates on each pixel in a given polygon

in vec3 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

//This is the 4 component float vector that gets outputted to the screen
//for each pixel.
out vec4 color;

// Adapted for ONLY SUNLIGHT!!!!

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main() {
	float TL = 0.0f, TR = 0.0f, BL = 0.0f, BR = 0.0f;
	int bits = int(fragmentColour.g * 255.0f);

	color = vec4(1.0);

	if(bool(bits & 1)) { // Top corners
		// This is right
		TL = fragmentColour.r;
	}
	if(bool((bits & 2) >> 1)) {
		// This is left
		TR = fragmentColour.r;
	}

	if(bool((bits & 4) >> 2)) { // Bottom corners
		// This is right
		BL = fragmentColour.r;
	}
	if(bool((bits & 8) >> 3)) {
		// This is left
		BR = fragmentColour.r;
	}

	float highPass = map(fragmentUV.x, 0.0, 1.0, TL, TR);
	float lowPass = map(fragmentUV.x, 0.0, 1.0, BL, BR);
	float midPass = map(fragmentUV.y, 0.0, 1.0, lowPass, highPass);

	color.r = midPass; // Actual light
}
