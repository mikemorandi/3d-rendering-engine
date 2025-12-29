#version 410

layout (location = 0) out vec4 FragColor;

in vec3 NormalWs;

//uniform mat3 envRotMat;
uniform int	numShBands;
uniform float exposure;
uniform vec3 shLightCoeffs[9];

//Albedo
const vec3 rho = vec3(1,1,1);
const float pi = 3.141592654;

int numShTerms = numShBands * numShBands;

/* Zonal harmonics coefficients for unshadowed diffuse transfer, 
 * computed for normal unitZ. Theses coefficients still have to be 
 * multiplied by the albedo factor rho */
const vec3 zhCoeffs = vec3(1/(2*sqrt(pi)), 1/sqrt(3*pi), 1/8*sqrt(5/pi));

void evalSHfast(in vec3 v,inout float pOut[9] )
{
	float fC0,fS0,fC1,fS1,fPa,fPb,fPc;
	float vz2 = v.z*v.z;
	pOut[0] = 0.282094791774;
	pOut[2] =  0.488602511903*v.z;
	pOut[6] = ( 0.946174695758*vz2)+ -0.315391565253;
	fC0 = v.x;
	fS0 = v.y;
	fPa =  -0.488602511903;
	pOut[3] = fPa*fC0;
	pOut[1] = fPa*fS0;
	fPb =  -1.09254843059*v.z;
	pOut[7] = fPb*fC0;
	pOut[5] = fPb*fS0;
	fC1 = (v.x*fC0)-(v.y*fS0);
	fS1 = (v.x*fS0)+(v.y*fC0);
	fPc =  0.546274215296;
	pOut[8] = fPc*fC1;
	pOut[4] = fPc*fS1;
}

void computeDiffuseTransferCoeffs(in vec3 n, inout float tfC[9])
{
	evalSHfast(n,tfC);

	for( int l = 0; l < 3; l++ ) 
	{
		for( int m = -l; m <= l; m++ ) 	
		{
			int index = l * (l+1) + m;
			tfC[index] *= sqrt(4*pi/(2*l+1)) * zhCoeffs[l];
		}
	}	
}

void main() {
    
	ivec2 fragCoords = ivec2(gl_FragCoord.xy);

	//mat3 envRotMat = mat3(
	//   0,0,1, // first column (not row!)
	//   0,1,0, // second column
	//   -1,0,0  // third column
	//);
	//Rotate coordinate frame to be oriented with the cubemap
	vec3 rotatedWsNormal = /*envRotMat **/  NormalWs;
	//rotatedWsNormal.z *= -1;

	float transferCoeff[9];
	computeDiffuseTransferCoeffs(rotatedWsNormal,transferCoeff);

	//For each color channel
	vec3 diffuseShade = vec3(0.0);
	for(int c = 0; c < 3; c++)
	{
		//For each sh lighting coeff
		for(int sh=0; sh < numShTerms; sh++)
		{			
			diffuseShade[c] +=  shLightCoeffs[sh][c] * transferCoeff[sh] * rho[c];
		}
	}

	FragColor = vec4(vec3(1.0) - exp(-diffuseShade * exposure),1.0);
	
	

}
