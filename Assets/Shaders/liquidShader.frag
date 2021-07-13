#version 130
//The fragment shader operates on each pixel in a given polygon

in vec3 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 color;

uniform sampler2D textureSampler;

void main() {
	float water = texture(textureSampler, fragmentUV.xy).r;
	float alpha = water;
	
	//if(water < 0.8f) {
	//	alpha = 0.8f;
	//}
	//if(water < 0.5f) {
	//	alpha = 0.5f;
	//}
	//if(water < 0.2f) {
	//	alpha = 0.2f;
	//}
	//if(water == 0.0f) {
	//	alpha = 0.0f;
	//}

	color = vec4(1.0, 1.0, 1.0, alpha) * fragmentColour;
}
