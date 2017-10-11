#pragma once
#include "OGLRenderer.h"

enum MeshBuffer {
	VERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER, NORMAL_BUFFER, INDEX_BUFFER, TANGENT_BUFFER, MAX_BUFFER
};

#define normalIndex 3

class Mesh
{
public:
	Mesh(void);
	~Mesh(void);

	virtual void Draw();
	static Mesh* GenerateTriangle();
	static Mesh * GenerateSquare();
	static Mesh * GenerateQuad();

	


	void GenerateNormals();

	void GenerateTangents();
	Vector3 GenerateTangent(const Vector3 & a, const Vector3 & b, const Vector3 & c, const Vector2 & ta, const Vector2 & tb, const Vector2 & tc);

	void SetBumpMap(GLuint tex) { bumpTexture = tex; }
	GLuint GetBumpMap() { return bumpTexture; };



	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() { return texture; }

	

protected:
	void BufferData();
	void RebufferData();
	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint type;

	GLuint texture;
	Vector2* textureCoords;

	Vector3* vertices;
	Vector4* colours;

	GLuint numIndices;
	unsigned int* indices;

	Vector3* tangents;
	GLuint bumpTexture;

	

	Vector3* normals;
	GLuint normalBuffer;
};





