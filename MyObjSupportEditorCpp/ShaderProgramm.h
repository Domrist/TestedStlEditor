#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>

class ShaderProgramm
{

private:
	GLuint CreateShader(GLint shaderType,std::string shaderProgramm) {
		
		const GLchar* shaderStr[]{ shaderProgramm.c_str()};

		int shaderId = glCreateShader(shaderType);
		glShaderSource(shaderId, 1, shaderStr,NULL);
		glCompileShader(shaderId);

		GLint shaderCompiled;

		glGetShaderiv(shaderId,GL_COMPILE_STATUS,&shaderCompiled);

		if (shaderCompiled != GL_TRUE) {
			std::cout << "Cannot compile shader\t" << shaderProgramm << std::endl;
			return -1;
		}
		return shaderId;
	}

	void DeleteShader(){ //calls after compiling shaders
		glDetachShader(programm,vertexShader);
		glDetachShader(programm, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	GLint vertexShader, fragmentShader,programm;


public:

	ShaderProgramm(std::string vertexProgramm,std::string fragmentProgramm) {
		vertexShader = CreateShader(GL_VERTEX_SHADER, vertexProgramm);
		fragmentShader = CreateShader(GL_FRAGMENT_SHADER,fragmentProgramm);
		
		programm = glCreateProgram();
		glAttachShader(programm, vertexShader);
		glAttachShader(programm, fragmentShader);
		glLinkProgram(programm);
		GLint programmCompiled;

		glGetProgramiv(programm,GL_LINK_STATUS,&programmCompiled);
		if (programmCompiled != GL_TRUE) {
			std::cout << "Cannot compile programm\n";
		}
		DeleteShader();
	}

	void ActivateProgramm() {
		glUseProgram(programm);
	}

	void DeactivateProgramm() {
		glUseProgram(0);
	}

	~ShaderProgramm() {
		glDeleteProgram(programm);
	}
};

