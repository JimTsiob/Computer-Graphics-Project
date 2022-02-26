#pragma once

#include "SDL2/SDL.h"
#include "GLEW\glew.h"
#include <string>
#include <vector>
#include "GeometricMesh.h"

// Singleton Class of Texture Manager
class AssetManager
{
protected:
	struct AssetContainer
	{
		unsigned int m_vao;
		unsigned int m_vbo_positions;
		unsigned int m_vbo_normals;
		unsigned int m_vbo_tangents;
		unsigned int m_vbo_bitangents;
		unsigned int m_vbo_texcoords;
		std::string name;
	};

	std::vector<AssetContainer> assets;

	unsigned int findAsset(const std::string& assetName);

public:
	
	static AssetManager& GetInstance()
	{
		static AssetManager manager;
		return manager;
	}

	~AssetManager();

	void Clear();

	unsigned int RequestAsset(const std::string & assetName, GeometricMesh* mesh=nullptr);

protected:
	AssetManager();
	void operator=(AssetManager const&);
};
