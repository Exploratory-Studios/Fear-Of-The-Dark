#version 330
//The vertex shader operates on each vertex

//input data from the VBO. Each vertex is 3 floats
in vec3 vertexPosition;
in vec4 vertexColour;
in vec2 vertexUV;

out vec3 fragmentPosition;
out vec4 fragmentColour;
out vec2 fragmentUV;

uniform mat4 projectionMatrix = mat4(1.0); // "Perspective"

void main() {
    //Set the x,y,z position on the screen
    gl_Position.xy = (projectionMatrix * vec4(vertexPosition, 1.0)).xy;
    gl_Position.z = vertexPosition.z;

    //Indicate that the coordinates are normalized
    gl_Position.w = 1.0;

    fragmentPosition = vertexPosition;
    fragmentColour = vertexColour;
    fragmentUV = vec2(vertexUV.x, vertexUV.y); // Doesn't even invert Y.
}
