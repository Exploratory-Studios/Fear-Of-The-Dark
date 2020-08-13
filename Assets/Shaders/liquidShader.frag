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

	float opacity = texture(textureSampler, fragmentUV.xy).r;
	if(opacity > 0.08f) {
		opacity = 1.0f;
	} else if(opacity > 0.065f) {
		opacity = 0.5f;
	} else if(opacity > 0.05f) {
		opacity = 0.25f;
	} else {
		discard;
	}
	color = fragmentColour * vec4(1.0, 1.0, 1.0, opacity);
}
