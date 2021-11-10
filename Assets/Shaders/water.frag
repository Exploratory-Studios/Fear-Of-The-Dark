#version 130
//The fragment shader operates on each pixel in a given polygon

in vec3 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

uniform float time;

//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 color;

uniform sampler2D textureSampler;

void main() {
	color = texture(textureSampler, fragmentUV.xy) * vec4(vec3(sin(time/50+fragmentUV.x*20-fragmentUV.y*5) * 0.1 + 0.95).xyz, 1.0);
}
