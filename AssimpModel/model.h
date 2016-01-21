#ifndef MODEL_H_
#define MODEL_H_

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "GL/glew.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "SOIL.h"

#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

#include "mesh.h"

class myShader;

class Model
{
public:
	Model(GLchar* path)
	{		loadModel(path);
	}
	void Draw(myShader shader);
private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

GLint TextureFromFile(const char* path, std::string directory);

#endif