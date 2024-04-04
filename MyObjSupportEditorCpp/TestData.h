#pragma once
#define GLEW_STATIC
#include <GL/glew.h>

GLfloat cubeVertices[] =
{
	1, 0 ,0,
	 0, -1, 0,
	 0 ,0, 0,
	 1 ,-1 ,0,
	 1 ,-1 ,1,
	 0 ,-1, 1,
	 1 ,0 ,1,
	 0 ,0, 1
};

GLuint cubeFaces[] =
{
	 1 - 1, 2 - 1, 3 - 1
	,2 - 1, 1 - 1, 4 - 1
	,5 - 1, 2 - 1, 4 - 1
	,2 - 1, 5 - 1, 6 - 1
	,1 - 1, 5 - 1, 4 - 1
	,5 - 1, 1 - 1, 7 - 1
	,5 - 1, 8 - 1, 6 - 1
	,8 - 1, 5 - 1, 7 - 1
	,8 - 1, 2 - 1, 6 - 1
	,2 - 1, 8 - 1, 3 - 1
	,8 - 1, 1 - 1, 3 - 1
	,1 - 1, 8 - 1, 7 - 1
};

GLfloat simpleVertexes[] = {
		-0.5f,-0.5f,0,
		0.5f,-0.5f,0,
		-0.5f,0.5f,0,
		0.5f,0.5f,0,

};

GLuint simpleFaces[] = {
		0,1,2,
		0,2,3
};