#version 330 core
layout(location = 0) out vec4 out_color;

in vec2 f_texcoord;

#define _PI_ 3.14159

uniform vec3 uniform_camera_pos;
uniform vec3 uniform_camera_dir;

uniform sampler2D uniform_tex_pos;
uniform sampler2D uniform_tex_normal;
uniform sampler2D uniform_tex_albedo;
uniform sampler2D uniform_tex_mask;
uniform sampler2D uniform_tex_depth;

uniform vec3 uniform_light_color;
uniform vec3 uniform_light_pos;
uniform vec3 uniform_light_dir;

uniform float uniform_light_umbra;
uniform float uniform_light_penumbra;

uniform mat4 uniform_light_projection_view;
uniform int uniform_cast_shadows;
uniform float uniform_constant_bias = 0.0002;

uniform sampler2D uniform_shadow_map;

float compute_spotlight(const in vec3 pSurfToLight)
{
	float cos_umbra = cos(radians(0.5 * uniform_light_umbra));
	float cos_penumbra = cos(radians(0.5 * uniform_light_penumbra));
	float spoteffect = 1;
	float angle_vertex_spot_dir = dot(-pSurfToLight, uniform_light_dir);

	if (angle_vertex_spot_dir > cos_umbra) 
	{
		spoteffect = 1;
	}
	else if(angle_vertex_spot_dir >= cos_penumbra) 
	{
		spoteffect = smoothstep(cos_penumbra, cos_umbra, angle_vertex_spot_dir);
	}
	else
	{
		spoteffect = 0;
	}
	
	return spoteffect;
}

float shadow_pcf2x2_weighted(vec3 light_space_xyz)
{
	ivec2 shadow_map_size = textureSize(uniform_shadow_map, 0);
	float xOffset = 1.0 / shadow_map_size.x;
    float yOffset = 1.0 / shadow_map_size.y;

	// compute the weights of the neighboring pixels
	vec2 uv = light_space_xyz.xy - vec2(xOffset, yOffset);
	float u_ratio = mod(uv.x, xOffset) / xOffset;
	float v_ratio = mod(uv.y, yOffset) / yOffset;
	float u_opposite = 1 - u_ratio;
	float v_opposite = 1 - v_ratio;

	// compute the distance with a small bias
	float z = light_space_xyz.z - uniform_constant_bias;

	// compute the shadow percentage
	float bottomLeft = (texture(uniform_shadow_map, uv).r > z) ? u_opposite : 0.0;
	float bottomRight = (texture(uniform_shadow_map, uv + vec2(xOffset, 0)).r > z) ? u_ratio : 0.0; 
	float topLeft = (texture(uniform_shadow_map, uv + vec2(0, yOffset), 0).r > z) ? u_opposite : 0.0;
	float topRight = texture(uniform_shadow_map, uv + vec2(xOffset, yOffset)).r > z ? u_ratio : 0.0;
	float factor = (bottomLeft + bottomRight) * v_opposite + (topLeft + topRight) * v_ratio;

    return factor;
}

float shadow_pcf2x2_mean(vec3 light_space_xyz)
{
	ivec2 shadow_map_size = textureSize(uniform_shadow_map, 0);
	float xOffset = 1.0 / shadow_map_size.x;
    float yOffset = 1.0 / shadow_map_size.y;
	vec2 texelSize = vec2(xOffset, yOffset);

	vec2 uv_bl = light_space_xyz.xy + vec2(-1.0, -1.0) * texelSize;
	vec2 uv_br = light_space_xyz.xy + vec2(0, -1.0) * texelSize;
	vec2 uv_tl = light_space_xyz.xy + vec2(-1.0, 0) * texelSize;
	vec2 uv_tr = light_space_xyz.xy + vec2(0, 0) * texelSize;

	float z = light_space_xyz.z - uniform_constant_bias;

	// compute the shadow percentage
	float bottomLeft = (texture(uniform_shadow_map, uv_bl).r > z) ? 1.0 : 0.0;
	float bottomRight = (texture(uniform_shadow_map, uv_br).r > z) ? 1.0 : 0.0;
	float topLeft = (texture(uniform_shadow_map, uv_tl).r > z) ? 1.0 : 0.0;
	float topRight = texture(uniform_shadow_map, uv_tr).r > z ? 1.0 : 0.0;
	float factor = (bottomLeft + bottomRight + topLeft + topRight) / 4.0;

    return factor;
}

float shadow_nearest(vec3 light_space_xyz)
{
	// sample shadow map
	float shadow_map_z = texture2D(uniform_shadow_map, light_space_xyz.xy).r;

	// + shaded -> 0.0 
	// - lit -> 1.0
	// < closer to the light
	return ((light_space_xyz.z - uniform_constant_bias) < shadow_map_z) ? 1.0 : 0.0;
}

// 1 sample per pixel
float shadow(vec3 pwcs)
{
	// project the pwcs to the light source point of view
	vec4 plcs = uniform_light_projection_view * vec4(pwcs, 1.0);
	// perspective division
	plcs /= plcs.w;
	// convert from [-1 1] to [0 1]
	plcs.xy = (plcs.xy + 1) * 0.5;

	// check that we are inside light clipping frustum
	if (plcs.x < 0.0) return 0.0;
	if (plcs.y < 0.0) return 0.0;
	if (plcs.x > 1.0) return 0.0;
	if (plcs.y > 1.0) return 0.0;

	// set scale of light space z vaule to [0, 1]
	plcs.z = 0.5 * plcs.z + 0.5;

	// sample shadow map
	//return shadow_nearest(plcs.xyz);
	//return shadow_pcf2x2_weighted(plcs.xyz);
	return shadow_pcf2x2_mean(plcs.xyz);
}

vec3 fresnel(
	const in vec3 diffColor,
	const in vec3 reflectance,
	const in float VdotH,
	const in float metallic)
{
	vec3 f0 = mix(reflectance, diffColor, metallic);
	float u = 1.0 - VdotH;
	float u5 = (u * u) * (u * u) * u;
	return min(vec3(1.0), f0  + (vec3(1.0) - f0) * u5);
}

float distribution(float NdotH, float a)
{
	float NdotH2 = NdotH * NdotH;
	float a2 = a * a;
	float denom = (NdotH2 * (a2 - 1) + 1);
	denom = _PI_ * denom * denom;
	float D = a2 / max(denom, 0.001);
	return D;
}

float geometric(float NH, float NO, float HO, float NI)
{
	float G = 2.0 * NH * min(NI, NO) / (0.001 + HO);
	return min(1.0, G);
}

vec3 blinn_phong(
	const in vec3 pSurfToEye,
	const in vec3 pSurfToLight,
	const in vec3 pPos,
	const in vec3 pNormal,
	const in vec3 pAlbedo,
	const in vec4 pMask)
{
	vec3 halfVector = normalize(pSurfToEye + pSurfToLight);

	float NdotL = max(dot(pNormal, pSurfToLight), 0.0);
	float NdotH = max(dot(pNormal, halfVector), 0.0);
	float metallic = pMask.r;
	float ao = pMask.g;
	vec3 reflectance = vec3(0.05);//vec3(pMask.b);
	vec3 F = fresnel(pAlbedo, reflectance, max(dot(halfVector, pSurfToEye), 0.0), metallic);

	vec3 kd = ((ao * pAlbedo) / _PI_) * (1.0 - F) * (1.0 - metallic);
	float alpha = pMask.a * 127.0;

	float fn =
		((alpha + 2) * (alpha + 4)) /
		(8 * _PI_ * (alpha + 1.0 / pow(2, alpha / 2.0)));

	vec3 ks = NdotL > 0.0 ? reflectance * fn * pow(NdotH, alpha) : vec3(0.0);
	ks *= F;

	float dist = distance(uniform_light_pos, pPos);

	return (kd + ks) * (uniform_light_color / pow(dist, 2)) * NdotL;
}

vec3 cook_torrance(
	const in vec3 pSurfToEye,
	const in vec3 pSurfToLight,
	const in vec3 pPos,
	const in vec3 pNormal,
	const in vec3 pAlbedo,
	const in vec4 pMask)
{
	vec3 halfVector = normalize(pSurfToEye + pSurfToLight);

	float NdotL = clamp(dot(pNormal, pSurfToLight), 0.0, 1.0);
	float NdotV = clamp(dot(pNormal, pSurfToEye), 0.0, 1.0);
	float NdotH = clamp(dot(pNormal, halfVector), 0.0, 1.0);
	float HdotV = clamp(dot(halfVector, pSurfToEye), 0.0, 1.0);
	float HdotL = clamp(dot(halfVector, pSurfToLight), 0.0, 1.0);

	float metallic = pMask.r;
	float ao = pMask.g;
	vec3 reflectance = vec3(0.05);//vec3(pMask.b);
	float alpha = pMask.a;

	vec3 F = fresnel(pAlbedo, reflectance, max(dot(halfVector, pSurfToEye), 0.0), metallic);
	float D = distribution(NdotH, alpha);
	float G = geometric(NdotH, NdotV, HdotV, NdotL);
	vec3 ks = (F * G * D) / max((4.0 * NdotL * NdotV), 0.0001);
	vec3 kd = ((ao * pAlbedo) / _PI_) * (1.0 - F) * (1.0 - metallic);
	float dist = distance(uniform_light_pos, pPos);

	return (kd + ks) * (uniform_light_color / pow(dist, 2)) * NdotL;
}

void main(void)
{
    float d = texture(uniform_tex_depth, f_texcoord).r;

	if(d == 1.0) discard;

	vec4 pos_wcs = texture(uniform_tex_pos, f_texcoord);
	vec4 normal_wcs = texture(uniform_tex_normal, f_texcoord);
	vec4 albedo = texture(uniform_tex_albedo, f_texcoord);
	vec4 mask = texture(uniform_tex_mask, f_texcoord);

	vec3 surfToEye = normalize(uniform_camera_pos - pos_wcs.xyz);
	vec3 surfToLight = normalize(uniform_light_pos - pos_wcs.xyz);

	// check if we have shadows
	float shadow_value = (uniform_cast_shadows == 1) ? shadow(pos_wcs.xyz) : 1.0;

//#if 0
	vec3 brdf = blinn_phong(surfToEye, surfToLight, pos_wcs.xyz,
		normal_wcs.xyz,
		albedo.xyz, mask);

	float spotEffect = 1.0;//compute_spotlight(surfToLight);

	out_color = vec4(shadow_value * brdf * spotEffect, 1.0);
/*#else
	out_color = vec4(cook_torrance(surfToEye, surfToLight, pos_wcs.xyz,
		normal_wcs.xyz,
		albedo.xyz, mask), 1.0);
#endif*/
}