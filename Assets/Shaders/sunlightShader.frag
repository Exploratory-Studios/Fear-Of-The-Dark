#version 410
//The fragment shader operates on each pixel in a given polygon

in vec3 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 colour2; // For the post processor input.

uniform sampler2D textureSampler;

void main() {
	colour2 = texture(textureSampler, fragmentUV.xy) * fragmentColour;
}
