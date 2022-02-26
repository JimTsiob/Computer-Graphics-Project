#version 330 core
layout(triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform int uniform_prim_id;
uniform float uniform_time;

in vec2 v_texcoord[];
in vec3 v_position_wcs[];
in mat3 v_TBN[];

out vec2 f_texcoord;
out vec3 f_position_wcs;
out mat3 f_TBN;


void main(void)
{
	if(uniform_prim_id == int(gl_PrimitiveIDIn)) return;

	gl_Position = gl_in[0].gl_Position;
	f_texcoord = v_texcoord[0];
	f_position_wcs = v_position_wcs[0];
	f_TBN = v_TBN[0];
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	f_texcoord = v_texcoord[1];
	f_position_wcs = v_position_wcs[1];
	f_TBN = v_TBN[1];
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	f_texcoord = v_texcoord[2];
	f_position_wcs = v_position_wcs[2];
	f_TBN = v_TBN[2];
	EmitVertex();

	EndPrimitive();
}
