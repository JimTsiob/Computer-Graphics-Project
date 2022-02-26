#pragma once

#include "AssetManager.hpp"

AssetManager::AssetManager()
{

}

AssetManager::~AssetManager()
{
	this->Clear();
}

void AssetManager::Clear()
{
	for (int i = 0; i < assets.size(); i++)
	{
		glDeleteVertexArrays(1, &assets[i].m_vao);
		glDeleteBuffers(1, &assets[i].m_vbo_positions);
		glDeleteBuffers(1, &assets[i].m_vbo_normals);
		glDeleteBuffers(1, &assets[i].m_vbo_tangents);
		glDeleteBuffers(1, &assets[i].m_vbo_bitangents);
		glDeleteBuffers(1, &assets[i].m_vbo_texcoords);
	}

	assets.clear();
}

unsigned int AssetManager::findAsset(const std::string& assetName)
{
	for (int i = 0; i < assets.size(); i++)
	{
		if (assets[i].name.compare(assetName) == 0)
		{
			return assets[i].m_vao;
		}
	}
	return 0;
}

GLuint AssetManager::RequestAsset(const std::string& assetName, GeometricMesh* mesh)
{
	GLuint asset_vao = this->findAsset(assetName);

	if(!mesh || asset_vao != 0)
	{
		return asset_vao;
	}

	AssetContainer asset;
	asset.name = assetName;

	glGenVertexArrays(1, &asset.m_vao);
	glBindVertexArray(asset.m_vao);

	glGenBuffers(1, &asset.m_vbo_positions);
	glBindBuffer(GL_ARRAY_BUFFER, asset.m_vbo_positions);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(glm::vec3), &mesh->vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,				// attribute index
		3,              // number of elements per vertex, here (x,y,z)
		GL_FLOAT,       // the type of each element
		GL_FALSE,       // take our values as-is
		0,		         // no extra data between each position
		0				// pointer to the C array or an offset to our buffer
	);

	glGenBuffers(1, &asset.m_vbo_normals);
	glBindBuffer(GL_ARRAY_BUFFER, asset.m_vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, mesh->normals.size() * sizeof(glm::vec3), &mesh->normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,				// attribute index
		3,              // number of elements per vertex, here (x,y,z)
		GL_FLOAT,		// the type of each element
		GL_FALSE,       // take our values as-is
		0,		         // no extra data between each position
		0				// pointer to the C array or an offset to our buffer
	);

	if (!mesh->textureCoord.empty())
	{
		glGenBuffers(1, &asset.m_vbo_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, asset.m_vbo_texcoords);
		glBufferData(GL_ARRAY_BUFFER, mesh->textureCoord.size() * sizeof(glm::vec2), &mesh->textureCoord[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2,				// attribute index
			2,              // number of elements per vertex, here (x,y,z)
			GL_FLOAT,		// the type of each element
			GL_FALSE,       // take our values as-is
			0,		         // no extra data between each position
			0				// pointer to the C array or an offset to our buffer
		);
	}

	if (!mesh->tangents.empty())
	{
		glGenBuffers(1, &asset.m_vbo_tangents);
		glBindBuffer(GL_ARRAY_BUFFER, asset.m_vbo_tangents);
		glBufferData(GL_ARRAY_BUFFER, mesh->tangents.size() * sizeof(glm::vec3), &mesh->tangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(
			3,				// attribute index
			3,              // number of elements per vertex, here (x,y,z)
			GL_FLOAT,		// the type of each element
			GL_FALSE,       // take our values as-is
			0,		         // no extra data between each position
			0				// pointer to the C array or an offset to our buffer
		);

		glGenBuffers(1, &asset.m_vbo_tangents);
		glBindBuffer(GL_ARRAY_BUFFER, asset.m_vbo_tangents);
		glBufferData(GL_ARRAY_BUFFER, mesh->tangents.size() * sizeof(glm::vec3), &mesh->tangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(
			3,				// attribute index
			3,              // number of elements per vertex, here (x,y,z)
			GL_FLOAT,		// the type of each element
			GL_FALSE,       // take our values as-is
			0,		         // no extra data between each position
			0				// pointer to the C array or an offset to our buffer
		);

		glGenBuffers(1, &asset.m_vbo_bitangents);
		glBindBuffer(GL_ARRAY_BUFFER, asset.m_vbo_bitangents);
		glBufferData(GL_ARRAY_BUFFER, mesh->bitangents.size() * sizeof(glm::vec3), &mesh->bitangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(
			4,				// attribute index
			3,              // number of elements per vertex, here (x,y,z)
			GL_FLOAT,		// the type of each element
			GL_FALSE,       // take our values as-is
			0,		         // no extra data between each position
			0				// pointer to the C array or an offset to our buffer
		);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	this->assets.push_back(asset);
	return asset.m_vao;
}