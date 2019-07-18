#version 130
//The fragment shader operates on each pixel in a given polygon

in vec2 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 color;

uniform sampler2D mySampler;
uniform vec2 screenSizeU;

void main() {

	vec4 textureColour = texture(mySampler, fragmentUV);

	float mod = (pow(abs(0.5 - fragmentUV.x), 2) + pow(abs(0.5 - fragmentUV.y), 2)) / 2.0;

	color = textureColour * fragmentColour * vec4(vec3(1.0-mod), 1);
}

