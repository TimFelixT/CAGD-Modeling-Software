#pragma once

#ifndef GLSLPROGRAM_H
#define GLSLPROGRAM_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

namespace cg
{
	namespace GLSLShader
	{
		enum GLSLShaderType
		{
			VERTEX = GL_VERTEX_SHADER,
			FRAGMENT = GL_FRAGMENT_SHADER,
			GEOMETRY = GL_GEOMETRY_SHADER,
			TESS_CONTROL = GL_TESS_CONTROL_SHADER,
			TESS_EVALUATION = GL_TESS_EVALUATION_SHADER
		};
		extern std::map<GLSLShader::GLSLShaderType, std::string> GLSLShaderTypeString;
	};

	/*
	 Based on https://github.com/daw42/glslcookbook.
     PROTOCOL:
     COMPILATION
	 this->compileShaderFromFile, this->compileShaderFromString // for vertex shader
	 this->compileShaderFromFile, this->compileShaderFromString // for fragment shader
	 this->compileShaderFromFile, this->compileShaderFromString // for geometry shader (optional)
	 this->compileShaderFromFile, this->compileShaderFromString // for tesselation shader (optional)
	 this->compileShaderFromFile, this->compileShaderFromString // for compute shader (optional)
	 this->setUniform // uniforms of all shaders
	 this->bindAttribLocation, this->bindFragDataLocation // in of vertex shader (Attrib), out of fragment shader (FragData)
                                                          // connecting in/out pair (Varying) is automatic
     LINKING
	 this->link

	 USING
	 this->use                                            // activate program (all bound shaders)
	*/
	class GLSLProgram
	{
	private:
	    GLuint handle;  // id/handle of program object
		std::string logString;       // compile log
		std::vector<GLuint> shaders; // ids/handles of shaders
		
		bool linked;                 // not-linked (still compiling) or linked
		bool verbose;                // simple error handling: output to console

	public:
		GLSLProgram(bool verbose = true); // simple error handling: output to console
		~GLSLProgram(void);

		bool compileShaderFromFile  (const char* filename, GLSLShader::GLSLShaderType type);
		bool compileShaderFromString(const std::string& source, GLSLShader::GLSLShaderType type);
		bool link(void);
		void use (void);
		
		std::string log(void) const; // error log
		int  getHandle(void) const;  // program id/handle
		bool isLinked(void) const;   // still COMPILATION or already LINKED 

		void bindAttribLocation(GLuint location, const char* name);   // location -> attrib in
		void bindFragDataLocation(GLuint location, const char* name); //             fragData out -> location
		void setUniform(const char* name, float x, float y, float z);
		void setUniform(const char* name, const glm::vec3& v);
		void setUniform(const char* name, const glm::vec4& v);
		void setUniform(const char* name, const glm::mat3& m);
		void setUniform(const char* name, const glm::mat4& m);
		void setUniform(const char* name, float value);
		void setUniform(const char* name, int value);
		void setUniform(const char* name, bool value);
		void setUniform(const char* name, int size, const glm::mat4* value);
		void printActiveUniforms(void);  // Get OpenGL state: uniform
		void printActiveAttribs (void);  // Get OpenGL state: attrib

		int  getUniformLocation (const char* name); // location of uniform by name

	private:
		bool checkAndCreateProgram(void);             // sets this->handle
		bool fileExists(const std::string& filename); // internal for this->compileShaderFromFile
	};
};

#endif
