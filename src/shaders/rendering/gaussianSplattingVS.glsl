#version 460 core

#define ALPHA_CUTOFF (1.0f/256.0f)
#define GAUSSIAN_CUTOFF_SCALE (sqrt(log(1.0f / ALPHA_CUTOFF)))
#define MIN_PIXEL_RADIUS 1.0f

// Static quad vertex position per-vertex
layout(location = 0) in vec3 vertexPos;

// Per-instance (quad) attributes
layout(location = 1) in vec4 gaussianPosition_ms; //model space  
layout(location = 2) in vec4 gaussianColor;
layout(location = 3) in vec4 gaussianPackedScale;
layout(location = 4) in vec4 gaussianNormal;
layout(location = 5) in vec4 gaussianQuaternion;
layout(location = 6) in vec4 gaussianPbr;

// You can add more per-instance attributes at locations 3, 4, 5 as needed.
out vec3 out_color;
out vec2 out_uv;
out float out_opacity;
out vec3 out_conic;
out vec2 out_coordxy;

uniform mat4 u_MVP;
uniform mat4 u_worldToView;	
uniform mat4 u_viewToClip;	
uniform mat4 u_objectToWorld; 
uniform vec2 u_resolution;
uniform vec3 u_hfov_focal;

//TODO: Should define some common imports, this func should be used also in other shaders
void castQuatToMat3(vec4 quat, out mat3 rotMatrix)
{
	vec3 firstRow = vec3(
		1.f - 2.f * (quat.z * quat.z + quat.w * quat.w),
		2.f * (quat.y * quat.z - quat.x * quat.w),      
		2.f * (quat.y * quat.w + quat.x * quat.z)       
	);

	vec3 secondRow = vec3(
		2.f * (quat.y * quat.z + quat.x * quat.w),       
		1.f - 2.f * (quat.y * quat.y + quat.w * quat.w), 
		2.f * (quat.z * quat.w - quat.x * quat.y)        
	);

	vec3 thirdRow = vec3(
		2.f * (quat.y * quat.w - quat.x * quat.z),       
		2.f * (quat.z * quat.w + quat.x * quat.y),     
		1.f - 2.f * (quat.y * quat.y + quat.z * quat.z) 
	);

	rotMatrix = mat3(
		firstRow,
		secondRow,
		thirdRow
	);
}
/*
vec3 computeConic(mat4 objectToWorld, mat4 worldToView, mat4 viewToClip, vec2 resolution, inout float alphaScale)
{
	// just reading 3 floats
	vec3 wsCenterPos	= (vec4(0, 0, 0, 1) * objectToWorld).xyz;
	vec4 vsCenterPosHom =  worldToView * vec4(wsCenterPos, 1);
	vec3 vsCenterPos	= vsCenterPosHom.xyz;

	// covariance Matrix in world space, symmetric, can be optimized
	mat3 wsBigSigma = transpose(mat3(objectToWorld)) * mat3(objectToWorld);
	//float3x3 wsBigSigma = mul((float3x3) objectToWorld, transpose((float3x3) objectToWorld));

	// covariance Matrix in view space, symmetric, can be optimized
	mat3 W = mat3(worldToView);

	mat3 vsBigSigma = transpose(W) * wsBigSigma * W;

	// https://stackoverflow.com/questions/46075606/getting-focal-length-and-focal-point-from-a-projection-matrix#:~:text=The%20focal%20length%20is%20merely,to%20obtain%20the%20focal%20point.
	// The focal length is merely the first element in the matrix (m11).
	//float2 focalLength = float2(viewToClip._m11, viewToClip._m11);

	vec2 focalLength = vec2(viewToClip[1][1], viewToClip[1][1]);

	// the following lines are copied from https://github.com/aras-p/UnityGaussianSplatting/blob/main/Assets/GaussianSplatting/Shaders/GaussianSplatting.hlsl
	float aspect = viewToClip[0][0] / viewToClip[1][1];
	
	float tanFovX = 1 / viewToClip[0][0];
	float tanFovY = 1 / viewToClip[1][1];
	float limX = 1.3 * tanFovX;
	float limY = 1.3 * tanFovY;
	vsCenterPos.x = clamp(vsCenterPos.x / vsCenterPos.z, -limX, limX) * vsCenterPos.z;
	vsCenterPos.y = clamp(vsCenterPos.y / vsCenterPos.z, -limY, limY) * vsCenterPos.z;


	mat3 J = mat3(
		focalLength.x / vsCenterPos.z,				0,										-(focalLength.x * vsCenterPos.x) / (vsCenterPos.z * vsCenterPos.z),
		0,											focalLength.y / vsCenterPos.z,			-(focalLength.y * vsCenterPos.y) / (vsCenterPos.z * vsCenterPos.z),
		0,											0,										0);
	
	mat2 csBigSigma = mat2(transpose(J) * vsBigSigma * J);
	csBigSigma[0][0] += MIN_PIXEL_RADIUS * MIN_PIXEL_RADIUS / (resolution.y * resolution.y);
	csBigSigma[1][1] += MIN_PIXEL_RADIUS * MIN_PIXEL_RADIUS / (resolution.y * resolution.y);

	mat2 invCov = inverse(mat2(csBigSigma));

	return vec3(invCov[0][0], 2 * invCov[1][0], invCov[1][1]);
}


vec2 computeCorner(vec2 xy, vec3 conic, vec2 resolution)
{
	float a = conic.x;
	float b = 0.5f * conic.y;
	float c = conic.z;

	float baseHalf = (a + c) * 0.5f;
	float rootHalf = 0.5f * sqrt((a - c) * (a - c) + 4.0f * b * b);
	float rx = inversesqrt(baseHalf + rootHalf);
	float ry = inversesqrt(baseHalf - rootHalf);

	vec2 k = vec2(a - c, 2.0f * b);

	// if splat gets thinner than a pixel, keep it pixel size for antialiasing, this works with aspectRatio
	rx = max(rx, 1.0f / resolution.y);
	ry = max(ry, 1.0f / resolution.x);

	// half vector, should be faster than atan()
	vec2 axis0 = normalize(k + vec2(length(k), 0));

	vec2 axis1 = vec2(axis0.y, -axis0.x);

	return (axis0 * (rx * xy.x) - axis1 * (ry * xy.y)) * vec2(resolution.y / resolution.x, 1.0f);
}
*/

mat4 calcMatrixFromRotationScaleTranslation(vec3 translation, vec4 rot, vec3 scale)
{
	mat3 scaleMatrix = mat3(
		vec3(scale.x, 0, 0),
		vec3(0, scale.y, 0),
		vec3(0, 0, scale.z)
	);

	mat3 rotMatrix;
	castQuatToMat3(rot, rotMatrix);

	mat3 wsRotMulScale = scaleMatrix * rotMatrix;

	mat4 rot4x4 = mat4(
		vec4(wsRotMulScale[0], 0.0),
		vec4(wsRotMulScale[1], 0.0),
		vec4(wsRotMulScale[2], 0.0),
		vec4(0.0, 0.0, 0.0, 1.0)
	);


	mat4 tr = mat4(
		vec4(1, 0, 0, 0),
		vec4(0, 1, 0, 0),
		vec4(0, 0, 1, 0),
		vec4(translation, 1)
	);

	return tr * rot4x4;
}


void computeCov3D(vec4 quat, vec3 scales, out mat3 sigma3d) {

	mat3 scaleMatrix = mat3(
		scales.x , 0, 0, 
		0, scales.y , 0,
		0, 0, scales.z 
	);

	mat3 rotMatrix;
	castQuatToMat3(quat, rotMatrix);

	mat3 mMatrix = scaleMatrix * rotMatrix;

	// sigma3D = R*S*S.T*R.T 
	sigma3d = transpose(mMatrix) * mMatrix;
};

void main() {
	//layout(location = 1) in vec4 gaussianPosition_ms; //model space  
	//layout(location = 2) in vec4 gaussianColor;
	//layout(location = 3) in vec4 gaussianPackedScale;
	//layout(location = 4) in vec4 gaussianNormal;
	//layout(location = 5) in vec4 gaussianQuaternion;
	//layout(location = 6) in vec4 gaussianPbr;

	//uniform mat4 u_MVP;
	//uniform mat4 u_worldToView;	
	//uniform mat4 u_viewToClip;	
	//uniform mat4 u_objectToWorld; 
	//uniform vec2 u_resolution;
	//uniform vec3 u_hfov_focal;

	//out vec3 out_color;
	//out vec2 out_uv;
	//out float out_opacity;
	//out vec3 out_conic;

	mat3 cov3D;

	//TODO: I am technically having to log and then exp the same scale values along the pipeline, but whatever
	computeCov3D(gaussianQuaternion, vec3(exp(gaussianPackedScale.xy), exp(max(gaussianPackedScale.x, gaussianPackedScale.y))), cov3D);
	vec4 gaussian_vs = u_worldToView * vec4(gaussianPosition_ms.xyz, 1);

	vec4 pos2dHom = u_viewToClip * gaussian_vs;
	
	//persective divide
	pos2dHom.xyz = pos2dHom.xyz / pos2dHom.w;
	pos2dHom.w = 1.f;

	vec2 wh = 2 * u_hfov_focal.xy * u_hfov_focal.z;
	float limx = 1.3 * u_hfov_focal.x;
	float limy = 1.3 * u_hfov_focal.y;

	//To NDC with persp div
	float txtz = gaussian_vs.x / gaussian_vs.z;
	float tytz = gaussian_vs.y / gaussian_vs.z;

	float tx = min(limx, max(-limx, txtz)) * gaussian_vs.z;
	float ty = min(limy, max(-limy, tytz)) * gaussian_vs.z; 

	if (any(greaterThan(abs(pos2dHom.xyz), vec3(1.3)))) {
		gl_Position = vec4(-100, -100, -100, 1);
		return;	
	}

	//Jacobian of affine approximation of projective transformation
	mat3 J = mat3(
	  u_hfov_focal.z / gaussian_vs.z, 0., -(u_hfov_focal.z * tx) / (gaussian_vs.z * gaussian_vs.z),
	  0., u_hfov_focal.z / gaussian_vs.z, -(u_hfov_focal.z * ty) / (gaussian_vs.z * gaussian_vs.z),
	  0., 0., 0.
	);

	//Apply jacobian
	mat3 T = transpose(mat3(u_worldToView)) * J;

	// J*W*Sigma3D*W.T*J.T
	mat3 cov2d = transpose(T) * transpose(cov3D) * T;

	//Just care about applying low pass filter to 2x2 upper matrix
	cov2d[0][0] += 0.3f;
	cov2d[1][1] += 0.3f; 

	float det = determinant(cov2d);
	if (det == 0.0f)
		gl_Position = vec4(0.f, 0.f, 0.f, 0.f);

	float det_inv = 1.f / det;


	vec2 quadwh_scr = vec2(3.f * sqrt(cov2d[0][0]), 3.f * sqrt(cov2d[1][1]));
	vec2 quadwh_ndc = quadwh_scr / wh * 2; //HMM
	pos2dHom.xy = pos2dHom.xy + vertexPos.xy * quadwh_ndc;

	gl_Position = pos2dHom;
	//save space using vec3 as its a 2x2 symmetric

	out_conic = vec3(cov2d[1][1] * det_inv, -cov2d[0][1] * det_inv, cov2d[0][0] * det_inv);
	out_color = gaussianColor.rgb;
	out_opacity = gaussianColor.a;
	out_uv = vertexPos.xy;

}
