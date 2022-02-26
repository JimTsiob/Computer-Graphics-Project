#version 330
layout(location = 0) out vec4 out_color;

in vec2 f_texcoord;

uniform sampler2D uniform_texture;
uniform sampler2D uniform_shadow_map;

uniform sampler2D uniform_tex_pos;
uniform sampler2D uniform_tex_normal;
uniform sampler2D uniform_tex_albedo;
uniform sampler2D uniform_tex_mask;
uniform sampler2D uniform_tex_depth;

// for perspective projection only
float linearize(const in float depth)
{
	float near = 0.1;
	float far = 30.0;
    float z = depth * 2.0 - 1.0; // [0, 1] -> [-1, 1]
    return ((2.0 * near * far) / (far + near - z * (far - near))) / far;
}

//#define PREVIEW_SHADOW_MAP
//#define CROSS_HAIR

void main(void)
{
	out_color.xyz = texture2D(uniform_texture, f_texcoord).rgb;

#ifdef PREVIEW_SHADOW_MAP
	out_color = vec4(vec3(linearize(texture2D(uniform_shadow_map, f_texcoord).r)), 1.0);
#elif defined CROSS_HAIR
	ivec2 halfRes = ivec2(textureSize(uniform_texture, 0) / 2);
	ivec2 pixel = ivec2(gl_FragCoord.xy);

	if((pixel.x == halfRes.x) ||
	   (pixel.y == halfRes.y))
	{
		out_color = vec4(1.0, 0.0, 0.0, 1.0);
	}
#endif
}