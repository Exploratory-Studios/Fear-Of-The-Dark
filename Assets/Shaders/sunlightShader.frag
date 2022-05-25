#version 330
//The fragment shader operates on each pixel in a given polygon

in vec3 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

out vec4 colour2; // For the post processor input.

uniform sampler2D textureSampler;

void main() {
	colour2 = texture(textureSampler, fragmentUV.xy);
}
