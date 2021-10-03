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
	float opacity = texture(textureSampler, fragmentUV.xy).r;
	
	if(opacity <= 0.0001f) discard;
	
	float alpha = pow(opacity, (1.0/3.0));

	color = vec4(1.0, 1.0, 1.0, alpha) * fragmentColour;
}
