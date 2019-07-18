#version 130
//The vertex shader operates on each vertex

//input data from the VBO. Each vertex is 2 floats
in vec2 vertexPosition;
in vec4 vertexColour;
in vec2 vertexUV;
in vec3 vertexLighting; // 0-1 is normal just colour-changing light, after 1 everything starts to turn the colour of the light

out vec2 fragmentPosition;
out vec4 fragmentColour;
out vec2 fragmentUV;
out vec2 inverseScreenSize;

uniform mat4 P;

uniform vec2 inverseScreenSizeU;

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

    fragmentColour = clamp(fragmentColour, 0, 255);

    fragmentUV = vec2(vertexUV.x, 1.0 - vertexUV.y);

	inverseScreenSize = inverseScreenSizeU;
}
