#include "GeometryNode.h"
#include "GeometricMesh.h"
#include <glm/gtc/type_ptr.hpp>
#include "TextureManager.h"

GeometryNode::GeometryNode() { /* Empty */ }

GeometryNode::~GeometryNode() { /* Empty */ }

void GeometryNode::Init(const std::string & name, GeometricMesh* mesh)
{
	this->m_vao = AssetManager::GetInstance().RequestAsset(name, mesh);

	for (int i = 0; i < mesh->objects.size(); i++)
	{
		Objects part;
		part.start_offset = mesh->objects[i].start;
		part.count = mesh->objects[i].end - mesh->objects[i].start;
		auto material = mesh->materials[mesh->objects[i].material_id];
		
		part.diffuse = glm::vec4(material.diffuse[0], material.diffuse[1], material.diffuse[2], 1.0);
		part.specular = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);
		part.ambient = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]);

		part.shininess = material.shininess;
		part.diffuse_textureID = (material.textureDiffuse.empty())? 0 : TextureManager::GetInstance().RequestTexture(material.textureDiffuse.c_str());
		part.mask_textureID = (material.textureSpecular.empty()) ? 0 : TextureManager::GetInstance().RequestTexture(material.textureSpecular.c_str());
		part.emissive_textureID = (material.textureAmbient.empty()) ? 0 : TextureManager::GetInstance().RequestTexture(material.textureAmbient.c_str());
		part.normal_textureID = (material.textureNormal.empty())? 0 : TextureManager::GetInstance().RequestTexture(material.textureNormal.c_str());
		part.bump_textureID = (material.textureBump.empty()) ? 0 : TextureManager::GetInstance().RequestTexture(material.textureBump.c_str());

		parts.push_back(part);
	}

	this->m_aabb.min = glm::vec3(std::numeric_limits<float_t>::max());
	this->m_aabb.max = glm::vec3(-std::numeric_limits<float_t>::max());

	for (auto& v : mesh->vertices)
	{
		this->m_aabb.min = glm::min(this->m_aabb.min, v);
		this->m_aabb.max = glm::max(this->m_aabb.max, v);
	}

	this->m_aabb.center = (this->m_aabb.min + this->m_aabb.max) * 0.5f;
}