#pragma once

#include <vector>



class ShaderProgram {
	public:
	enum shaderType { Vertex, Fragment };

	ShaderProgram();

	void createShader(shaderType type, const char* shaderSource);
	void linkShaders();
	unsigned int getProgramID();
	void useProgram();



	private:
	unsigned int mCount;
	std::vector<unsigned int> mShadersList;
	unsigned int mProgram;
};