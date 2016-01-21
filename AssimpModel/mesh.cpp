#include "mesh.h"

#include "myShader.h"

Mesh::Mesh(
		std::vector<Vertex> vertices, 
		std::vector<GLuint> indices, 
		std::vector<Texture> textures
		)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
		&vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
		&indices[0], GL_STATIC_DRAW);

	// set vertex location pointer
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	// set normal pointer
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

	// set texcoord pointer
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}

void Mesh::Draw(myShader shader)
{
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	for(GLuint i = 0; i < textures.size(); i++){
		glActiveTexture(GL_TEXTURE0 + i); //active proper texture unit before bind it
		std::stringstream ss;
		std::string number;
		std::string name = textures[i].type;
		if(name == "texture_diffuse")
			ss << diffuseNr++;
		else if(name == "texture_specular")
			ss << specularNr++;
		number = ss.str();

		glUniform1f(glGetUniformLocation(shader.getHandle(), ("material."+name+number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	//draw the mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}