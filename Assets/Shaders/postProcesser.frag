#version 330

in vec3 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

out vec4 colour0;

uniform sampler2D colourTexture0; // Regular colour
uniform sampler2D colourTexture1; // Normal maps
uniform sampler2D colourTexture2; // sunlight
uniform sampler2D colourTexture3; // fluids
//uniform sampler2D depthTexture;

const int MAX_LIGHTS = 30; // This is the max amnt of lights a scene can manage (MAX_LIGHTS_RENDERED)

uniform vec3 lights[MAX_LIGHTS]; // First two components are XY, 3th is intensity

uniform float playerDepth;
uniform float playerSanity;

uniform float time;

uniform vec2 screenSizeU;

const float Pi = 6.28318530718; // Pi*2

vec4 blur(float directions, float quality, float size, sampler2D textureS, vec2 uv) {
	vec2 radius = vec2(size);
	
	// Pixel colour
	vec4 pixColour = texture(textureS, uv);

	// Blur calculations
	for( float d=0.0; d<Pi; d+=Pi/directions)
	{
		for(float i=1.0/quality; i<=1.0; i+=1.0/quality)
		{
			pixColour += texture(textureS, uv+vec2(cos(d),sin(d))*radius*i);		
		}
	}

	// Output to screen
	pixColour /= quality * directions;
	return pixColour;
}

float downBlur(sampler2D sampler) {
	float res = 0.0;

	res = texture(sampler, fragmentUV.xy).r;
	for(float i = 0; i < 0.05; i += 0.01) {
		res += texture(sampler, fragmentUV.xy + vec2(0.0, i)).r;
	}

	return res / 6.0;
}

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

float getIntensity(vec3 lightPos, vec3 surfaceNormal) {
	// Get unit vector from fragment to light
	vec3 toLight = normalize(lightPos - vec3(fragmentPosition.xy, 0.0));
	
	vec3 normal = normalize(vec3(toLight.xy, (sqrt(pow(toLight.x, 2) + pow(toLight.y, 2))) / -toLight.z));
	
	// Dot the fragment to light with the fragment normal. If they face the same direction, intensity multiplier will be 1.0.
	float intensityMult = clamp(dot(surfaceNormal, normal * vec3(1.0, 1.0, -1.0)), 0, 1); // We must orient the Z coordinate so they match directions a little better.
	
	return intensityMult;
}

float getLight(float sun, vec3 fragmentNormal) { // first 3 are XYZ of average light (normalized), 4th is "average" intensity.
	float lightLevel = 0.0f;
	for(int i = 0; i < MAX_LIGHTS; i++) {
		float intensity = lights[i].z / pow(distance(fragmentPosition.xy, lights[i].xy), 2.0); // relative intensity

		if(intensity > 0.01) {
			float intensityMult = getIntensity(lights[i], fragmentNormal);
			
		    lightLevel += intensity * intensityMult;
		}
	}
	
	float intensityMult = getIntensity(vec3(0.0, 1.0, sun) + vec3(fragmentPosition.xy, 0.0), fragmentNormal);
	
	lightLevel += sun * intensityMult;
			
			
	return lightLevel;
}

float getSunlight() {
	return texture(colourTexture2, fragmentUV.xy).r;
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float vignette_alpha(vec2 uv, float threshold) {
	float dist = distance(uv, vec2(0.5));
	if(dist < threshold) {
		return 1.0;
	}
	float inv_sqrt_2 = 1.0/sqrt(2.0);
	float max_length = 0.8; // If we put inv_sqrt_2 here, we get that the corners of the screen are always black and that looks bad.
	float inv_neg_threshold = 1.0/(max_length - threshold);
	return 1.0 - inv_neg_threshold * (dist - threshold);
}

void main() {
	// Basic colour
	colour0 = texture(colourTexture0, fragmentUV.xy);
	
	// Depth
	// Get the difference between the displayed depth and this depth
	float fragDepth = fragmentPosition.z;
	float depthDifference = abs(fragDepth - playerDepth);
	// Blur based on depth
	if(depthDifference > 0.1 && colour0.a > 0.05) {
		colour0 = blur(8.0, 2.0, 0.008 * depthDifference, colourTexture0, fragmentUV);
	} // else do nothing, we've already read the colour.
	
	// Fluids
	vec4 fluid = texture(colourTexture3, fragmentUV.xy);
	colour0 = colour0 * (1.0 - fluid.a) + fluid * fluid.a;
	
	// Normal mapping + Lighting
	vec3 components = normalize(texture(colourTexture1, fragmentUV.xy).rgb * 2.0 - 1.0);
	colour0.rgb *= getLight(getSunlight(), components);
	
	// Vignette
	float sanity = abs(playerSanity * cos(time/360.0/5.0));
    float vignette = pow(vignette_alpha(fragmentUV.xy, sanity*1.0/sqrt(2.0)), 2.0);
    vec2 norm = normalize(vec2(0.5) - fragmentUV.xy);
    if(sanity < 0.3) {
        vignette += rand(vec2(time) * norm) * (0.35 - sanity) / 2.0;
        vignette -= rand(norm/1000.0 * time) * (0.3 - sanity);
    }
    colour0.rgb = colour0.rgb * (vignette) + vec3(0.0) * (1.0 - vignette);
    colour0.a = max(colour0.a, (1.0 - vignette));
}
