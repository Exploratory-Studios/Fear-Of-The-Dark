#version 130
//The fragment shader operates on each pixel in a given polygon

in vec2 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 color;

//uniform sampler2D mySampler;
uniform vec2 screenSizeU;
uniform float sanity; // How far in the vignette goes
uniform float time; 

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {

    vec2 uv = fragmentPosition.xy / screenSizeU.xy;
   
    uv *=  1.0 - uv.yx;
    
    float vig = uv.x*uv.y * 15.0;
    
    vig = pow(vig, 0.05 + pow((1.0 - sanity), 0.5) + 0.1 * (sanity < 0.3 ? (mod(rand(vec2(time)), 100)) : 1.0));
    
    color = vec4(vec3(0.0), 1.0-vig); 
}

