#include "model.h"

#include "myShader.h"

void Model::Draw(myShader shader)
{
	for (GLuint i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}
void Model::loadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(
		path, 
		aiProcess_Triangulate | 
		aiProcess_FlipUVs);
	bool tex = scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode;
	if (!tex){
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}
void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (GLuint i = 0; i < node->mNumChildren; i++){
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (GLuint i = 0; i < node->mNumChildren; i++){
		processNode(node->mChildren[i], scene);
	}
}
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	//Data to fill
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	
	//walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++){
		Vertex vertex;
		glm::vec3 vector;//we declare a place holder vector since assimp uses its own vector
		//positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		//Normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		//texture coord
		if (mesh->mTextureCoords[0]){
			glm::vec2 vec;
			//A vertex can contain up to 8 different texture coord. we thus make the assumption that we won't
			//use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);
	}

	//Now walk through each of the mesh's faces
	for (GLuint i = 0; i < mesh->mNumFaces; i++){
		aiFace face = mesh->mFaces[i];
		//retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++){
			indices.push_back(face.mIndices[j]);
		}
	}

	//process materials
	if (mesh->mMaterialIndex >= 0){
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// Diffuse: texture_diffuseN
		// Specular: texture_specularN
		// Normal: texture_normalN

		//1. Diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		//2. Specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++){
		aiString str;
		mat->GetTexture(type, i, &str);
		GLboolean skip = false;
		for (GLuint j = 0; j < textures_loaded.size(); j++){
			if (textures_loaded[j].path == str){
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip){
			//check if the texture was loaded before and if so, continue to next iteration
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

GLint TextureFromFile(const char* path, std::string directory)
{
	//Generate texture ID and load texture data
	std::string filename = std::string(path);
	filename = directory + "/" + filename;
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	//assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	//parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}