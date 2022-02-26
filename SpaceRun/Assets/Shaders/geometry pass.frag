#version 330 core
layout(location = 0) out vec4 out_pos;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec4 out_albedo;
layout(location = 3) out vec4 out_mask;

in vec2 f_texcoord;
in vec3 f_position_wcs;
in mat3 f_TBN;

#define _PI_ 3.14159

uniform vec3 uniform_diffuse;
uniform vec3 uniform_specular;
uniform vec3 uniform_ambient;
uniform float uniform_shininess;

uniform int uniform_has_tex_diffuse;
uniform int uniform_has_tex_mask;
uniform int uniform_has_tex_normal;
uniform int uniform_has_tex_emissive;

uniform sampler2D uniform_tex_diffuse;
uniform sampler2D uniform_tex_mask;
uniform sampler2D uniform_tex_normal;
uniform sampler2D uniform_tex_emissive;

void main(void)
{
	vec3 normal = f_TBN[2];

	if(uniform_has_tex_normal == 1)
	{
		vec3 nmap = texture(uniform_tex_normal, f_texcoord).rgb;
		nmap = nmap * 2.0 - 1.0;
		normal = normalize(f_TBN * nmap);
	}

	vec3 albedo = uniform_has_tex_diffuse == 1 ?
		texture(uniform_tex_diffuse, f_texcoord).rgb : uniform_diffuse;

	vec3 emission = uniform_has_tex_emissive == 1 ?
		texture(uniform_tex_emissive, f_texcoord).rgb : uniform_ambient;

	float reflectance = (uniform_specular.x + uniform_specular.y + uniform_specular.z) / 3;
	float gloss = uniform_shininess;
	float metallic = 0.0;
	float ao = 0.0;

	if(uniform_has_tex_mask == 1)
	{
		vec4 mask = texture(uniform_tex_mask, f_texcoord);
		metallic = mask.r;
		ao = mask.g;
		reflectance = mask.b;
		gloss = mask.a;
	}

	out_pos = vec4(f_position_wcs, emission.x);
	out_normal = vec4(normal, emission.y);
	out_albedo = vec4(albedo, emission.z);
	out_mask = vec4(metallic, ao, reflectance, gloss);
}