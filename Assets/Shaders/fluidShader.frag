#version 330
//The fragment shader operates on each pixel in a given polygon

in vec3 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

uniform float time;

//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 colour0;
out vec4 colour1; // normal

uniform sampler2D textureSampler;

void main() {
	vec4 fluid = texture(textureSampler, fragmentUV.xy);
	
	
	
	vec3 shimmer = vec3(sin(time/50.0 + fragmentUV.x*20.0 - fragmentUV.y*5.0) * 0.1 + 0.95);
	
	colour0 = fluid.r * fragmentColour;
	colour0.rgb *= shimmer;
	
	// Just set to completely neutral, light-wise. TODO
	colour1 = vec4(0.5, 0.5, 0.5, 1.0);
}
