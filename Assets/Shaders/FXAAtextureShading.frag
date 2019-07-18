#version 130
//The fragment shader operates on each pixel in a given polygon

in vec2 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;
in vec2 inverseScreenSize; // 1.0/width, 1.0/height

//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 color;

uniform sampler2D mySampler;

const float EDGE_THRESHOLD_MIN = 0.0312;
const float EDGE_THRESHOLD_MAX = 0.125;
const float SUBPIXEL_QUALITY = 0.75;
const int ITERATIONS = 12;

int QUALITY(int i) {
	return i > 5 ? i * 2 : 1;
}

float rgb2luma(vec3 rgb) { // Weighted grey levels of a number, returns between 1-0 and takes sensitivity of eyes into account
	return sqrt(dot(rgb, vec3(0.299, 0.587, 0.114)));
}

void main() {

    vec4 textureColour = texture(mySampler, fragmentUV);

	// Anti-Aliasing (FXAA) stuff
	vec4 rgbColour = textureColour * fragmentColour;
	
	float lumaC = rgb2luma(rgbColour.rgb);

	float lumaL = rgb2luma(textureOffset(mySampler, fragmentUV, ivec2(-1, 0)).rgb);
	float lumaR = rgb2luma(textureOffset(mySampler, fragmentUV, ivec2(1, 0)).rgb);;
	float lumaU = rgb2luma(textureOffset(mySampler, fragmentUV, ivec2(0, 1)).rgb);
	float lumaD = rgb2luma(textureOffset(mySampler, fragmentUV, ivec2(0, -1)).rgb);

	// Find the max and min luma
	float lumaMin = min(lumaC, min(min(lumaD, lumaU), min(lumaL, lumaR)));
	float lumaMax = max(lumaC, max(max(lumaD, lumaU), max(lumaL, lumaR)));

	// Delta
	float lumaRange = lumaMax - lumaMin;

	// If the luma variation is lower that a threshold (or if we are in a really dark area), we are not on an edge, don't perform any AA.
	if(lumaRange < max(EDGE_THRESHOLD_MIN,lumaMax*EDGE_THRESHOLD_MAX)){
		color = fragmentColour * textureColour;
		return;
	}

	// http://blog.simonrodriguez.fr/articles/30-07-2016_implementing_fxaa.html
	
	// FXAA is necessary, time to find direction
	// Query the 4 remaining corners lumas.
	float lumaDL = rgb2luma(textureOffset(mySampler, fragmentUV, ivec2(-1, -1)).rgb);;
	float lumaUR = rgb2luma(textureOffset(mySampler, fragmentUV, ivec2(1, 1)).rgb);
	float lumaUL = rgb2luma(textureOffset(mySampler, fragmentUV, ivec2(-1, 1)).rgb);
	float lumaDR = rgb2luma(textureOffset(mySampler, fragmentUV, ivec2(1, -1)).rgb);

	float lumaDU = lumaD + lumaU;
	float lumaLR = lumaL + lumaR;

	float lumaLCorners = lumaDL + lumaUL;
	float lumaRCorners = lumaDR + lumaUR;
	float lumaUCorners = lumaUR + lumaUL;
	float lumaDCorners = lumaDL + lumaDR;

	// Compute an estimation of the gradient along the horizontal and vertical axis.
	float edgeHorizontal = abs(-2.0 * lumaL + lumaLCorners) + abs(-2.0 * lumaC + lumaDU) * 2.0 + abs(-2.0 * lumaR + lumaRCorners);
	float edgeVertical   = abs(-2.0 * lumaU + lumaUCorners) + abs(-2.0 * lumaC + lumaLR) * 2.0 + abs(-2.0 * lumaD + lumaDCorners);

	// Is the local edge horizontal or vertical ?
	bool isHorizontal = (edgeHorizontal >= edgeVertical);

	// Select the two neighboring texels lumas in the opposite direction to the local edge.
	float luma1 = isHorizontal ? lumaD : lumaL;
	float luma2 = isHorizontal ? lumaU : lumaR;
	// Compute gradients in this direction.
	float gradient1 = luma1 - lumaC;
	float gradient2 = luma2 - lumaC;

	// Which direction is the steepest?
	bool is1Steepest = abs(gradient1) >= abs(gradient2);

	// Gradient in the corresponding direction, normalized.
	float gradientScaled = 0.25*max(abs(gradient1),abs(gradient2));


	// Choose the step size (one pixel) according to the edge direction.
	float stepLength = isHorizontal ? inverseScreenSize.y : inverseScreenSize.x;

	// Average luma in the correct direction.
	float lumaLocalAverage = 0.0;

	if(is1Steepest){
		// Switch the direction
		stepLength = - stepLength;
		lumaLocalAverage = 0.5*(luma1 + lumaC);
	} else {
		lumaLocalAverage = 0.5*(luma2 + lumaC);
	}

	// Shift UV in the correct direction by half a pixel.
	vec2 currentUv = fragmentUV;
	if(isHorizontal){
		currentUv.y += stepLength * 0.5;
	} else {
		currentUv.x += stepLength * 0.5;
	}


	// Compute offset (for each iteration step) in the right direction.
	vec2 offset = isHorizontal ? vec2(inverseScreenSize.x,0.0) : vec2(0.0,inverseScreenSize.y);
	// Compute UVs to explore on each side of the edge, orthogonally. The QUALITY allows us to step faster.
	vec2 uv1 = currentUv - offset;
	vec2 uv2 = currentUv + offset;

	// Read the lumas at both current extremities of the exploration segment, and compute the delta wrt to the local average luma.
	float lumaEnd1 = rgb2luma(texture(mySampler,uv1).rgb);
	float lumaEnd2 = rgb2luma(texture(mySampler,uv2).rgb);
	lumaEnd1 -= lumaLocalAverage;
	lumaEnd2 -= lumaLocalAverage;

	// If the luma deltas at the current extremities are larger than the local gradient, we have reached the side of the edge.
	bool reached1 = abs(lumaEnd1) >= gradientScaled;
	bool reached2 = abs(lumaEnd2) >= gradientScaled;
	bool reachedBoth = reached1 && reached2;

	// If the side is not reached, we continue to explore in this direction.
	if(!reached1){
		uv1 -= offset;
	}
	if(!reached2){
		uv2 += offset;
	} 

	// If both sides have not been reached, continue to explore.
	if(!reachedBoth){

		for(int i = 2; i < ITERATIONS; i++){
		    // If needed, read luma in 1st direction, compute delta.
		    if(!reached1){
		        lumaEnd1 = rgb2luma(texture(mySampler, uv1).rgb);
		        lumaEnd1 = lumaEnd1 - lumaLocalAverage;
		    }
		    // If needed, read luma in opposite direction, compute delta.
		    if(!reached2){
		        lumaEnd2 = rgb2luma(texture(mySampler, uv2).rgb);
		        lumaEnd2 = lumaEnd2 - lumaLocalAverage;
		    }
		    // If the luma deltas at the current extremities is larger than the local gradient, we have reached the side of the edge.
		    reached1 = abs(lumaEnd1) >= gradientScaled;
		    reached2 = abs(lumaEnd2) >= gradientScaled;
		    reachedBoth = reached1 && reached2;

		    // If the side is not reached, we continue to explore in this direction, with a variable quality.
		    if(!reached1){
		        uv1 -= offset * QUALITY(i);
		    }
		    if(!reached2){
		        uv2 += offset * QUALITY(i);
		    }

		    // If both sides have been reached, stop the exploration.
		    if(reachedBoth){ break;}
		}
	}

	// Compute the distances to each extremity of the edge.
	float distance1 = isHorizontal ? (fragmentUV.x - uv1.x) : (fragmentUV.y - uv1.y);
	float distance2 = isHorizontal ? (uv2.x - fragmentUV.x) : (uv2.y - fragmentUV.y);

	// In which direction is the extremity of the edge closer ?
	bool isDirection1 = distance1 < distance2;
	float distanceFinal = min(distance1, distance2);

	// Length of the edge.
	float edgeThickness = (distance1 + distance2);

	// UV offset: read in the direction of the closest side of the edge.
	float pixelOffset = - distanceFinal / edgeThickness + 0.5;

	// Is the luma at center smaller than the local average ?
	bool isLumaCenterSmaller = lumaC < lumaLocalAverage;

	// If the luma at center is smaller than at its neighbour, the delta luma at each end should be positive (same variation).
	// (in the direction of the closer side of the edge.)
	bool correctVariation = ((isDirection1 ? lumaEnd1 : lumaEnd2) < 0.0) != isLumaCenterSmaller;

	// If the luma variation is incorrect, do not offset.
	float finalOffset = correctVariation ? pixelOffset : 0.0;

	// Sub-pixel shifting
	// Full weighted average of the luma over the 3x3 neighborhood.
	float lumaAverage = (1.0/12.0) * (2.0 * (lumaDU + lumaLR) + lumaLCorners + lumaRCorners);
	// Ratio of the delta between the global average and the center luma, over the luma range in the 3x3 neighborhood.
	float subPixelOffset1 = clamp(abs(lumaAverage - lumaC)/lumaRange,0.0,1.0);
	float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;
	// Compute a sub-pixel offset based on this delta.
	float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * SUBPIXEL_QUALITY;

	// Pick the biggest of the two offsets.
	finalOffset = max(finalOffset,subPixelOffsetFinal);

	// Compute the final UV coordinates.
	vec2 finalUv = fragmentUV;
	if(isHorizontal){
		finalUv.y += finalOffset * stepLength;
	} else {
		finalUv.x += finalOffset * stepLength;
	}

	// Read the color at the new UV coordinates, and use it.
	vec3 finalColor = texture(mySampler,finalUv).rgb;
	color.rgb = finalColor * fragmentColour.rgb;
	color.a = fragmentColour.a * textureColour.a;

    //color = vec4(vec3(grayScaleV), fragmentColour.a * textureColour.a);
}
