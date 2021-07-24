#version 130
//The fragment shader operates on each pixel in a given polygon

in vec3 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 color;

uniform sampler2D textureSampler;

float map(float value, float low1, float high1, float low2, float high2) {
	return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

void main() {
	float opacity = texture(textureSampler, fragmentUV.xy).r;
	
	if(opacity <= 0.1f) discard;
	
	float alpha = map(opacity * (-opacity + 2), 0.0, 1.0, 0.4, 1.0);

	color = vec4(1.0, 1.0, 1.0, alpha) * fragmentColour;
}
