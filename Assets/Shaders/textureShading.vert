
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

out vec4 lightSource; // normalized, pointing TO average direction of light.

uniform mat4 P;

#define MAX_NUM_OF_LIGHTS 30

uniform vec3 lights[MAX_NUM_OF_LIGHTS]; // x and y are positions, z is brightness.

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

	vec4 lightSources[MAX_NUM_OF_LIGHTS]; // an array of lights, where x, y, and z are the direction vector, and z is the effect on the vertex.

	int lightsAffecting = 0;
	vec4 averageSource = vec4(0.0);

	for(int i = 0; i < MAX_NUM_OF_LIGHTS; i++) {
		if(lights[i].z > 0.0) {
			float range = sqrt(lights[i].z) * 4.0;
			float dist = distance(fragmentPosition.xy, lights[i].xy);
			if(dist <= range && range > 0) {
				float intensity = (1.0f - dist/range) * lights[i].z;
				lightSources[i] = vec4(lights[i].xy - fragmentPosition, 1.0, intensity);

				averageSource.xyz += lightSources[i].xyz;
				averageSource.w += intensity;

				lightsAffecting++;
			}
		} else {
			break;
		}
	}

	/*float d0 = 1.0/sqrt(2.0); // center to corner
	
	float r0 = sqrt(vertexLight.x);
	float r1 = sqrt(vertexLight.y);
	float r2 = sqrt(vertexLight.z);
	float r3 = sqrt(vertexLight.w);
	
	float i0 = (1.0f - d0/r0) * vertexLight.x;
	float i1 = (1.0f - d0/r1) * vertexLight.y;
	float i2 = (1.0f - d0/r2) * vertexLight.z;
	float i3 = (1.0f - d0/r3) * vertexLight.w;

	if(i0 > 0.0) {
		averageSource.xyz += vec3(-.5,  .5, 1.0);
		//averageSource.w += i0;
		lightsAffecting++;
	}
	if(i1 > 0.0) {
		averageSource.xyz += vec3( .5,  .5, 1.0);
		//averageSource.w += i1;
		lightsAffecting++;
	}
	if(i2 > 0.0) {
		averageSource.xyz += vec3( .5, -.5, 1.0);
		//averageSource.w += i2;
		lightsAffecting++;
	}
	if(i3 > 0.0) {
		averageSource.xyz += vec3(-.5, -.5, 1.0);
		//averageSource.w += i3;
		lightsAffecting++;
	}*/


	if(lightsAffecting > 0) {
		lightSource.xyz = averageSource.xyz / vec3(lightsAffecting);

		lightSource.w = averageSource.w;
	} else {
		lightSource.xyz = vec3(0.0, 0.0, 1.0);
		lightSource.w = 0.0;
	}
}
