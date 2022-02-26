#version 330 core
layout(location = 0) in vec3 coord3d;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec3 v_tangent;
layout(location = 4) in vec3 v_bitangent;

out vec2 v_texcoord;
out vec3 v_position_wcs;
out mat3 v_TBN;

uniform mat4 uniform_projection_matrix;
uniform mat4 uniform_normal_matrix;
uniform mat4 uniform_world_matrix;

void main(void)
{
	v_TBN = mat3(
		normalize(vec3(uniform_normal_matrix * vec4(v_tangent, 0.0))),
		normalize(vec3(uniform_normal_matrix * vec4(v_bitangent, 0.0))),
		normalize(vec3(uniform_normal_matrix * vec4(v_normal, 0.0))));

	v_texcoord = texcoord;
	v_position_wcs = vec3(uniform_world_matrix * vec4(coord3d, 1.0));
	gl_Position = uniform_projection_matrix * vec4(coord3d, 1.0);
}
