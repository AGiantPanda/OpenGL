#ifndef MESH_H_
#define MESH_H_

//std includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

//gl includes
#include "GL/glew.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture
{
	GLuint id;
	std::string type;
	aiString path;
};

class myShader;

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	Mesh(
		std::vector<Vertex> vertices, 
		std::vector<GLuint> indices, 
		std::vector<Texture> textures
		);
	void Draw(myShader shader);

private:
	GLuint VAO, VBO, EBO;
	void setupMesh();
};

#endif