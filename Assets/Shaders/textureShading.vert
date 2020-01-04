
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
out float fragmentLight;

uniform mat4 P;

float linearInterpolate(float min, float max, float percentage) {
	return (min*(1-percentage)+max*percentage);
}

float cosineInterpolate(float min, float max, float percentage) { // cosine interpolation
	float mu2;

   	mu2 = (1.0 - sin(mu * 3.141592)) / 2.0;
   	return (min * (1.0 - mu2) + max * mu2);
}

float interpolateCornersforCoords(vec2 coords, vec4 corners) {
	float topX = linearInterpolate(corners.x, corners.y, coords.x);
	float bottomX = linearInterpolate(corners.z, corners.w, coords.x);

	return cosineInterpolate(bottomX, topX, coords.y);// * (0.25 - clamp(distance(coords, vec2(0.5, 0.5)), 0.0, 0.25)));
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
	fragmentColour.xyz *= vertexLighting.xyz;

    fragmentUV = vec2(vertexUV.x, 1.0 - vertexUV.y);

	fragmentLight = interpolateCornersforCoords(fragmentPosition, vertexLight);
}
