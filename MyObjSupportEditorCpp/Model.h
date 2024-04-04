#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

template <typename T>
struct Vector3
{
	T x, y, z;
};

template <typename T>
struct Triangle
{
	Vector3<T> normal;
	Vector3<T> vertices[3];
};

struct Vertex
{
	GLfloat x, y, z;
	GLuint index;
};

class Model
{
private:
	Vector3<double> position;
	Vector3<double> rotation;
	std::vector<GLfloat> trianglesVertices;
	std::vector<GLuint> trianglesFaces;
	GLuint vbo, ebo,vao,normalsVbo;
	GLuint vertexShader, fragmentShader, shaderProgram;
	unsigned int trianglesCount,vertexCounter;
	float modelColor[3];
	bool isSupport;
	int nameId;
	

public:
	Model(bool _isSupport);
	Model(std::string path,bool _isSupport);
	Model(std::string,double,double,double, bool _isSupport = false);
	Model(bool, bool _isSupport);
	~Model();

	static int supportNameId;

	void LoadModel(std::string path);
	void LoadModelWithoutEbo(std::string);

	void LoadModelObj(std::string path);
	void Draw();
	void DrawWithoutEbo();
	void showData();
};


