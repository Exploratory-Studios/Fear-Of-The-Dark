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
	float light = map(fragmentUV.y, 0.0, 1.0, fragmentColour.g, fragmentColour.b);

	color.r = light; // Actual light
	color.gba = vec3(1.0);
}
