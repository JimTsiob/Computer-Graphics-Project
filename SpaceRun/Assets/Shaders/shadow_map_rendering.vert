#version 330 core
layout(location = 0) in vec3 coord3d;

uniform mat4 uniform_projection_matrix;

void main(void) 
{
	gl_Position = uniform_projection_matrix * vec4(coord3d, 1.0);
}
