
#version 130
//The vertex shader operates on each vertex

//input data from the VBO. Each vertex is 2 floats
in vec2 vertexPosition;
in vec4 vertexColour;
in vec2 vertexUV;
in vec4 vertexLight; // 0th is top left, moves clockwise

out vec2 fragmentPosition;
out vec4 fragmentColour;
out vec2 fragmentUV;
out vec4 fragmentLight;

out vec3 lightSource; // normalized, pointing TO light source

uniform mat4 P;

vec3 vectorize(vec3 initial, vec3 terminal) {
	return vec3(terminal.x - initial.x, terminal.y - initial.y, terminal.z - initial.z);
}

void main() {
    //Set the x,y position on the screen
    gl_Position.xy = (P * vec4(vertexPosition, 0.0, 1.0)).xy;
    //the z position is zero since we are in 2D
    gl_Position.z = 0.0;

    //Indicate that the coordinates are normalized
    gl_Position.w = 1.0;

    fragmentPosition = vertexPosition;

    fragmentColour = vertexColour;

    fragmentUV = vec2(vertexUV.x, 1.0 - vertexUV.y);

	fragmentLight = vertexLight;

	// Find normal of plane, where x and y are the corners', and the z is their light value
	// set both vectors for the plane (crossing each other)
	vec3 A = vectorize(vec3(1.0, 1.0, vertexLight.y), vec3(0.0, 1.0, vertexLight.x)); // i2 - i1 = vector
	vec3 B = vectorize(vec3(0.0, 0.0, vertexLight.w), vec3(0.0, 1.0, vertexLight.x)); // i4 - ii = vector
	
	// Find cross product to get a vector orthogonal to both (the normal)
	vec3 normal = cross(A, B);
	lightSource = normalize(normal);
}
