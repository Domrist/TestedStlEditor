#include "Model.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <stdlib.h>
#include <map>
#include <array>
#include <algorithm>

using namespace std;

int Model::supportNameId;


Model::Model(bool _isSupport = false) {
	this->position = { 0,0,0 };
	this->rotation = { 0,0,0 };

	this->ebo = 0;
	this->vbo = 0;
	this->vao = 0;

	isSupport = _isSupport;

}

void Model::showData() {
}

Model::Model(string path, bool _isSupport = false){
	std::cout << "Model loaded tt\n";
	this->position = { 0,0,0 };
	this->rotation = { 0,0,0 };

	this->ebo = 0;
	this->vbo = 0;
	this->vao = 0;
	isSupport = _isSupport;
	
	if (isSupport) {
		modelColor[0] = 1;
		modelColor[1] = 1;
		modelColor[2] = 1;
		supportNameId++;
		nameId = supportNameId;
	}
	else {
		nameId = 100;//for test
	}
	LoadModelWithoutEbo(path);
}



Model::Model(std::string path, double _x, double _y, double _z, bool _isSupport) {
	std::cout << "Model loaded tt\n";
	this->position = { _x,_y,_z };
	this->rotation = { 0,0,0 };
	this->ebo = 0;
	this->vbo = 0;
	this->vao = 0;
	modelColor[0] = 0;
	modelColor[1] = 0.145f;
	modelColor[2] = 0.121f;
	isSupport = _isSupport;
	if (isSupport) {
		modelColor[0] = 1;
		modelColor[1] = 1;
		modelColor[2] = 1;
		supportNameId++;
		nameId = supportNameId;
	}
	else {
		nameId = 100;//for test
	}
	LoadModelWithoutEbo(path);
}

Model::~Model() {
	
}

void Model::LoadModel(std::string path) {
	fstream file(path, ios::in | ios::binary);
	trianglesCount = 0;
	if (file) {
		file.seekg(80);
		
		trianglesCount = 0;
		Triangle<float> p;

		std::vector<Triangle<float>> triangles;

		std::cout << "Sizse of my triangle = " << sizeof(p) << endl;
		file.read((char*)&trianglesCount,sizeof(unsigned int));
		std::cout << "TrianglesCounter = " << (trianglesCount) << std::endl;
		
		for (int i = 0; i < trianglesCount; i++) {
			Triangle<GLfloat> trix;
			file.read((char*)&trix,sizeof(Triangle<float>));
			triangles.push_back(trix);
			file.seekg(2,ios::cur); //между треугольниками отступ в 2 байта, поэтому перешагиваем между ними
		}
		file.close();

		std::cout << "Triangles readed" << std::endl;


		std::vector<Vertex> vertices;
		std::map<std::array<float, 3>, int> unique_vertices;

		for (const auto& tris : triangles) {
			for (int i = 0; i < 3; i++) {
				const auto& vertex = tris.vertices[i]; //vertex type = Vector3<T> {x,y,z}

				const std::array<float, 3> v = {vertex.x,vertex.y,vertex.z};

				if (unique_vertices.count(v) == 0) {
					int index = vertices.size();
					Vertex tmp = { vertex.x,vertex.y,vertex.z,index };
					vertices.push_back(tmp);
					unique_vertices[v] = index;
				}
			}
		}
		std::cout << "Count of triangles = " << triangles.size() << endl;
		std::cout << "Count of unique vertices = " << vertices.size() << endl;

		std::vector<unsigned int> indices;
		int currenttris = 1;
		for (const auto& tris : triangles) {
			std::cout << "Parsed " << currenttris << " from " << triangles.size()<<  std::endl;
			currenttris++;
			for (int i = 0; i < 3; i++) {
				const auto& vertex = tris.vertices[i]; //vertex type = Vector3<T> {x,y,z}
				const std::array<float, 3> v = { vertex.x,vertex.y,vertex.z };
				auto it = unique_vertices.find(v);
				if (it != unique_vertices.end()) {
					indices.push_back(it->second);
				}
			}
		}

		int verticesPercent = vertices.size() / 100;

		std::vector<float> verticesDestination;
		auto it = vertices.begin();
		for (long double i = 0; i < vertices.size();i++) {
			verticesDestination.push_back(vertices[i].x); verticesDestination.push_back(vertices[i].y); verticesDestination.push_back(vertices[i].z);
			std::cout << "CurrentPercent = " << i * verticesPercent << endl;

		}
	
		trianglesCount = indices.size();

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesDestination.size(), verticesDestination.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*indices.size(),indices.data(),GL_STATIC_DRAW);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);


		const GLchar* vertexShaderSource = "#version 330 core\n"
			"in vec3 position;\n"
			"void main()\n"
			"{\n"
			"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
			"}\0";
		const GLchar* fragmentShaderSource = "#version 330 core\n"
			"out vec4 color;\n"
			"void main()\n"
			"{\n"
			"color = vec4(0.0f, 1.0f, 0.25f, 1.0f);\n"
			"}\n\0";

		this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(this->vertexShader,1,&vertexShaderSource,NULL);
		glCompileShader(this->vertexShader);

		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(this->vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(this->vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED FOR MODEL\n" << infoLog << std::endl;
		}
		else {
			std::cout << "COMPILING VERTEX SHADER FOR MODEL DONE\n";
		}

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		// Check for compile time errors
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED FOR MODEL\n" << infoLog << std::endl;
		}
		else {
			std::cout << "COMPILING FRAGMENT SHADER FOR MODEL DONE\n";
		}
		// Link shaders
		GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		// Check for linking errors
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED FOR MODEL\n" << infoLog << std::endl;
		}
		else {
			std::cout << "COMPILING PROGRAM FOR MODEL DONE\n";
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

	}
	else {
		std::cout << "Couldnt open file\n";
		file.close();
		return;
	}
	file.close();
	
	return;

}

void Model::LoadModelWithoutEbo(std::string path) {
	fstream file(path, ios::in | ios::binary);
	trianglesCount = 0;
	if (file) {
		file.seekg(80);

		trianglesCount = 0;
		Triangle<float> p;

		std::vector<Triangle<float>> triangles;

		std::cout << "Sizse of my triangle = " << sizeof(p) << endl;
		file.read((char*)&trianglesCount, sizeof(unsigned int));
		std::cout << "TrianglesCounter = " << (trianglesCount) << std::endl;

		std::vector<float> vertices(trianglesCount*9);
		std::vector<float> normals(trianglesCount*9);

		vertexCounter = trianglesCount * 3;

		
		unsigned long int c = 1;
		int step = 9;
		for (int i = 0; i < trianglesCount; i++) {
			file.read((char*)&normals[i*step], sizeof(float));
			file.read((char*)&normals[(i * step)+1], sizeof(float));
			file.read((char*)&normals[(i * step)+2], sizeof(float));
			normals[(i * step) + 3] = normals[i * step];
			normals[(i * step) + 4] = normals[(i * step) + 1];
			normals[(i * step) + 5] = normals[(i * step) + 2];
			normals[(i * step) + 6] = normals[i * step];
			normals[(i * step) + 7] = normals[(i * step) + 1];
			normals[(i * step) + 8] = normals[(i * step) + 2];
			//std::cout << normals[i * 3] << "\t" << normals[i * 3 + 1] << "\t" << normals[i * 3 + 2] << std::endl;
			file.read((char*)&vertices[i * step],sizeof(float));
			file.read((char*)&vertices[(i * step)+1], sizeof(float));
			file.read((char*)&vertices[(i * step) + 2], sizeof(float));
			file.read((char*)&vertices[(i * step) + 3], sizeof(float));
			file.read((char*)&vertices[(i * step) + 4], sizeof(float));
			file.read((char*)&vertices[(i * step) + 5], sizeof(float));
			file.read((char*)&vertices[(i * step) + 6], sizeof(float));
			file.read((char*)&vertices[(i * step) + 7], sizeof(float));
			file.read((char*)&vertices[(i * step) + 8], sizeof(float));
			file.seekg(2, ios::cur); //между треугольниками отступ в 2 байта, поэтому перешагиваем между ними
		}
		file.close();
		
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &normalsVbo);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(GLfloat),(GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, normalsVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), normals.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		const GLchar* vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 position;\n"
			"void main()\n"
			"{\n"
			"//gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
			"}\0";
		const GLchar* fragmentShaderSource = "#version 330 core\n"
			"out vec4 color;\n"
			"void main()\n"
			"{\n"
			"//color = vec4(1.0f, 0.0f, 1.0f, 1.0f);\n"
			"}\n\0";

		this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(this->vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(this->vertexShader);

		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(this->vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(this->vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED FOR MODEL\n" << infoLog << std::endl;
		}
		else {
			std::cout << "COMPILING VERTEX SHADER FOR MODEL DONE\n";
		}

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		// Check for compile time errors
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED FOR MODEL\n" << infoLog << std::endl;
		}
		else {
			std::cout << "COMPILING FRAGMENT SHADER FOR MODEL DONE\n";
		}
		// Link shaders
		GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		// Check for linking errors
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED FOR MODEL\n" << infoLog << std::endl;
		}
		else {
			std::cout << "COMPILING PROGRAM FOR MODEL DONE\n";
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	else {
		std::cout << "Couldnt open file\n";
		file.close();
		return;
	}
	file.close();

	return;

}


//unfinished method,maybe do later
void Model::LoadModelObj(std::string path) { 
	//fstream fileStream(path);
	//string s;

	//bool isAddingVertces = false;
	//bool isAddingFaces = false;

	//std::vector<GLfloat> tmpCountVerticesVector;
	//std::vector<GLuint> tmpCountFacesVector;
	//while (fileStream.good()) {
	//	getline(fileStream, s);
	//	istringstream is(s);
	//	while (is) {
	//		string tmp;
	//		is >> tmp;
	//		if (tmp == "v")
	//		{
	//			isAddingVertces = true;
	//			continue;
	//		}
	//		else if (isAddingVertces && is)
	//		{
	//			tmpCountVerticesVector.push_back(atof(tmp.c_str()));
	//		}

	//		if (tmp == "f")
	//			isAddingFaces = true;
	//		else if (isAddingFaces && is) {
	//			tmpCountFacesVector.push_back(atoi(tmp.c_str()));
	//		}
	//	}
	//	if (isAddingVertces) {
	//		isAddingVertces = false;
	//		tmpCountVerticesVector.size() > 3 ? this->quadsVertices.insert(this->quadsVertices.end(), tmpCountVerticesVector.begin(), tmpCountVerticesVector.end()) : this->trianglesVertices.insert(this->trianglesVertices.end(), tmpCountVerticesVector.begin(), tmpCountVerticesVector.end());
	//		tmpCountVerticesVector.clear();
	//	}
	//	if (isAddingFaces) {
	//		isAddingFaces = false;
	//		tmpCountFacesVector.size() > 3 ? this->quadsFaces.insert(this->quadsFaces.end(), tmpCountFacesVector.begin(), tmpCountFacesVector.end()) : this->trianglesFaces.insert(this->trianglesFaces.end(), tmpCountFacesVector.begin(), tmpCountFacesVector.end());
	//		tmpCountFacesVector.clear();
	//	}
	//}

	//fileStream.close();
	//
	////for (auto i : facesVector) //хуйня нужна так как obj файлы начинают считать с единицы
	////	i--;

	//glGenBuffers(1,&vbo);
	//glGenBuffers(1, &ebo);

	//glBindBuffer(GL_ARRAY_BUFFER,vbo);
	//glBufferData(GL_ARRAY_BUFFER, verticesVector.size() * sizeof(GLfloat), verticesVector.data(), GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER,0);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, facesVector.size()*sizeof(GLuint), facesVector.data(), GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//Model::Model(bool testLoad) {
//	this->position = { 0,0,0 };
//	this->rotation = { 0,0,0 };
//
//	this->countOfFaces = 0;
//	this->ebo = 0;
//	this->vbo = 0;
//
//	GLfloat center = 0;
//	GLfloat side = 0.5f;
//	GLfloat top = 0.5f;
//	GLfloat forward = 0.5f;
//	
//
//	verticesVector.push_back(-side); verticesVector.push_back(-top); verticesVector.push_back(-forward);
//	verticesVector.push_back(-side); verticesVector.push_back(top); verticesVector.push_back(-forward);
//	verticesVector.push_back(side); verticesVector.push_back(top); verticesVector.push_back(-forward);
//	verticesVector.push_back(side); verticesVector.push_back(-top); verticesVector.push_back(-forward);
//
//	verticesVector.push_back(-side); verticesVector.push_back(-top); verticesVector.push_back(forward);
//	verticesVector.push_back(-side); verticesVector.push_back(top); verticesVector.push_back(forward);
//	verticesVector.push_back(side); verticesVector.push_back(top); verticesVector.push_back(forward);
//	verticesVector.push_back(side); verticesVector.push_back(-top); verticesVector.push_back(forward);
//
//	//facesVector.push_back(2); facesVector.push_back(1); facesVector.push_back(0); facesVector.push_back(3);// - right version,lol
//	//facesVector.push_back(0); facesVector.push_back(1); facesVector.push_back(2); facesVector.push_back(3);
//	facesVector.push_back(0); facesVector.push_back(1); facesVector.push_back(4); facesVector.push_back(5);// - right version,lol
//
//	glGenBuffers(1, &vbo);
//	glGenBuffers(1, &ebo);
//
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glBufferData(GL_ARRAY_BUFFER, verticesVector.size() * sizeof(GLfloat), verticesVector.data(), GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, facesVector.size() * sizeof(GLuint), facesVector.data(), GL_STATIC_DRAW);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//}

void Model::Draw() {
	glUseProgram(this->shaderProgram);
	glBindVertexArray(this->vao);
	glDrawElements(GL_TRIANGLES, trianglesCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Model::DrawWithoutEbo() {
	glPushName(this->nameId); //тест, может быть должен быть другой порядок вызова данной функции
	glPushMatrix();

	glTranslatef(this->position.x, this->position.y, this->position.z);
	//GLint lightPosLoc = glGetUniformLocation(shaderProgram,"lightPos");
	//glUniform3f(lightPosLoc,0,0,1);
	//glUseProgram(this->shaderProgram);
	glColor3f(modelColor[0], modelColor[1], modelColor[2]); //окей, можем задавать цвет модели не через шейдер. Почему? Да хер знает, облажался с биндом шейдеров
	glBindVertexArray(this->vao);
	//glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(2);
	glDrawArrays(GL_TRIANGLES, 0, vertexCounter);
	glBindVertexArray(0);
	//glUseProgram(0);
	glPopMatrix();
	glPopName(); //также, мб нужен другой тип порядок для отображения
}