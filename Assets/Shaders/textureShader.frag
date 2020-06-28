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
	gl_FragDepth = fragmentPosition.z;

	color = texture(textureSampler, fragmentUV.xy);
	if(color.a <= 0.0) {
		discard;
	}
}
