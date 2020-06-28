#version 130
//The fragment shader operates on each pixel in a given polygon

in vec3 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 color;

uniform sampler2D mySampler;
uniform sampler2D depthSampler;

uniform vec2 screenSizeU;

float normpdf(in float x, in float sigma)
{
	return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}


vec4 gaussianBlur(vec2 fragCoord, vec2 iResolution, sampler2D iChannel0)
{
	vec4 fragColor = vec4(0.0);

	vec3 c = texture(iChannel0, fragCoord.xy / iResolution.xy).rgb;

	//declare stuff
	const int mSize = 11;
	const int kSize = (mSize-1)/2;
	float kernel[mSize];
	vec3 final_colour = vec3(0.0);
	
	//create the 1-D kernel
	float sigma = 7.0;
	float Z = 0.0;
	for (int j = 0; j <= kSize; ++j)
	{
		kernel[kSize+j] = kernel[kSize-j] = normpdf(float(j), sigma); // Creates a 1D array, which is read speedily!
	}
	
	//get the normalization factor (as the gaussian has been clamped)
	for (int j = 0; j < mSize; ++j)
	{
		Z += kernel[j];
	}
	
	//read out the texels
	for (int i=-kSize; i <= kSize; ++i)
	{
		for (int j=-kSize; j <= kSize; ++j)
		{
			final_colour += kernel[kSize+j]*kernel[kSize+i]*texture(iChannel0, (fragCoord.xy+vec2(float(i),float(j))) / iResolution.xy).rgb;

		}
	}
	
	
	fragColor = vec4(final_colour/(Z*Z), 1.0);

	return vec4(fragColor.rgb, 1.0);
}

void main() {
	float depth = texture(depthSampler, fragmentUV.xy).r;//gl_FragCoord.z;

	if(depth > 1.0) {
		color = gaussianBlur(fragmentPosition.xy, screenSizeU, mySampler);
	} else {
		color = vec4(vec3(depth) * texture(mySampler, fragmentUV.xy).rgb, 0.75);
		color -= vec4(vec3(depth) * texture(mySampler, fragmentUV.xy).rgb, 0.75);
		color += texture(depthSampler, fragmentUV.xy);
	}
}

