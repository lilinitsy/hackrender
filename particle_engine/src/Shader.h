#ifndef SHADER_H
#define SHADER_H
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

class Shader
{
	public:
		int Id;

		Shader(const char *vertexPath, const char *fragmentPath);

		void use()
		{
			glUseProgram(Id);
		}

		void setBool(const string &name, bool val)
		{
			glUniform1i(glGetUniformLocation(Id, name.c_str()), (int) val);
		}

		void setInt(const string &name, int val)
		{
			glUniform1i(glGetUniformLocation(Id, name.c_str()), val);
		}

		void setFloat(const string &name, float val)
		{
			glUniform1i(glGetUniformLocation(Id, name.c_str()), val);
		}

		GLint getInt(const char *name)
		{
			return glGetAttribLocation(Id, name);
		}

		void setVec4(const std::string &name, const glm::vec4 &vec) const
		{
			glUniform4f(glGetUniformLocation(Id, name.c_str()), vec.x, vec.y, vec.z, vec.a);
		}

		void setVec3(const std::string &name, const glm::vec3 &vec) const
		{
			glUniform3f(glGetUniformLocation(Id, name.c_str()), vec.x, vec.y, vec.z);
		}

		void setVec2(const std::string &name, const glm::vec3 &vec) const
		{
			glUniform2f(glGetUniformLocation(Id, name.c_str()), vec.x, vec.y);
		}

		void setMat2(const std::string &name, const glm::mat2 &mat) const
		{
		   glUniformMatrix2fv(glGetUniformLocation(Id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		// ------------------------------------------------------------------------
		void setMat3(const std::string &name, const glm::mat3 &mat) const
		{
		   glUniformMatrix3fv(glGetUniformLocation(Id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		// ------------------------------------------------------------------------
		void setMat4(const std::string &name, const glm::mat4 &mat) const
		{
		   glUniformMatrix4fv(glGetUniformLocation(Id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

		void setUniform(const std::string &name, int n)
		{
			glUniform1i(glGetUniformLocation(Id, name.c_str()), n);
		}

	private:
		void compile(int shaderId, string shaderType);
};

#endif
