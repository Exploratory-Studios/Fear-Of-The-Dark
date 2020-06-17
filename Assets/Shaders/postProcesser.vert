#version 130

in vec3 vertexPosition;
in vec4 vertexColour;
in vec2 vertexUV;

out vec3 fragmentPosition;
out vec4 fragmentColour;
out vec2 fragmentUV;

uniform mat4 P; // Our "camera"

void main() {
	gl_Position.xyz = (P * vec4(vertexPosition.xyz, 1.0)).xyz;
	gl_Position.w = 1.0; // Need to reset this

	fragmentPosition = vertexPosition;
	fragmentColour = vertexColour;
	fragmentUV = vertexUV;
}
