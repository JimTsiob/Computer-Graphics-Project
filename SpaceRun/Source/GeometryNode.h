#ifndef GEOMETRY_NODE_H
#define GEOMETRY_NODE_H

#include <vector>
#include "GLEW\glew.h"
#include <unordered_map>
#include "glm\gtx\hash.hpp"
#include "AssetManager.hpp"

class GeometryNode
{
public:
	GeometryNode();
	virtual ~GeometryNode();

	virtual void Init(const std::string & name, class GeometricMesh* mesh);

	struct Objects
	{
		unsigned int start_offset;
		unsigned int count;

		glm::vec3 diffuse;
		glm::vec3 ambient;
		glm::vec3 specular;

		float shininess;
		GLuint diffuse_textureID;
		GLuint normal_textureID;
		GLuint bump_textureID;
		GLuint emissive_textureID;
		GLuint mask_textureID;
	};

	struct aabb
	{
		glm::vec3 min;
		glm::vec3 max;
		glm::vec3 center;
	};

	std::vector<Objects> parts;

	glm::mat4 model_matrix;
	glm::mat4 app_model_matrix;
	aabb m_aabb;
	GLuint m_vao;
};

#endif