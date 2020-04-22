#include "include/GLSLProgram.h"

using namespace cg;

std::map<GLSLShader::GLSLShaderType, std::string> GLSLShader::GLSLShaderTypeString = {
	{ GLSLShader::VERTEX,       "GL_VERTEX_SHADER" },
	{ GLSLShader::FRAGMENT,		"GL_FRAGMENT_SHADER" },
	{ GLSLShader::GEOMETRY,		"GL_GEOMETRY_SHADER" },
	{ GLSLShader::TESS_CONTROL,	"GL_TESS_CONTROL_SHADER" },
	{ GLSLShader::TESS_EVALUATION, "GL_TESS_EVALUATION_SHADER" }
};

GLSLProgram::GLSLProgram(bool verbose)
: handle(0)
, linked(false)
, logString("")
, verbose(verbose)
{
}

GLSLProgram::~GLSLProgram(void)
{
	// Delete shader
	for (GLuint shader : shaders)
	{
		glDeleteShader(shader);
	}

	shaders.clear();

	// Delete program
	glDeleteProgram(handle);
	handle = 0;
}

bool GLSLProgram::checkAndCreateProgram(void)
{
	if (handle < 1)
	{
		handle = glCreateProgram();

		if (handle < 1)
		{
			logString = "could not create program";
			return false;
		}
	}

	return true;
}

bool GLSLProgram::compileShaderFromFile(const char* filename, GLSLShader::GLSLShaderType type)
{
	std::fstream file(filename);

	if (!file.good())
	{
		logString = "could not open file \"" + std::string(filename) + "\"";
		return false;
	}
	
	if (!checkAndCreateProgram())
	{
		return false;
	}

	std::string line;
	std::string content;

	while (!file.eof())
	{
		getline(file, line);
		content += line + "\n";
	}

	file.close();
	if (verbose) {
		std::cerr << "[" << GLSLShader::GLSLShaderTypeString[type] << "]" << std::endl 
			<< content << std::endl;
	}

	return compileShaderFromString(content, type);
}

bool GLSLProgram::compileShaderFromString(const std::string& source, GLSLShader::GLSLShaderType type)
{
	if (!checkAndCreateProgram())
	{
		return false;
	}
	
	GLuint shader = glCreateShader(type);

	if (shader == 0)
	{
		logString = "could not create shader";
		return false;
	}

	shaders.push_back(shader);

	const GLchar* codeArray[]= {source.c_str()};
	glShaderSource (shader, 1, codeArray, nullptr);
	glCompileShader(shader);

	GLint result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		GLint logLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		
		if (logLen > 0)
		{
			char* log = new char[logLen];

			GLsizei written;
			glGetShaderInfoLog(shader, logLen, &written, log);
			logString = std::string(log);
			delete [] log;
		}

		return false;
	}

	glAttachShader(handle, shader);

	return true;
}

bool GLSLProgram::link(void)
{
	if (linked)
	{
		return true;
	}

	if (handle < 1)
	{
		return false;
	}

	glLinkProgram(handle);

	GLint result;
	glGetProgramiv(handle, GL_LINK_STATUS, &result);

	if (result == GL_FALSE)
	{
		GLint logLen;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLen);
		
		if (logLen > 0)
		{
			char* log = new char[logLen];

			GLsizei written;
			glGetShaderInfoLog(handle, logLen, &written, log);
			logString = std::string(log);
			delete [] log;
		}

		return false;
	}

	linked = true;

	return true;
}

void GLSLProgram::use(void)
{
	if (handle < 1 || !linked)
	{
		return;
	}

	glUseProgram(handle);
}

std::string GLSLProgram::log(void) const
{
	return logString;
}

int GLSLProgram::getHandle(void) const
{
	return handle;
}

bool GLSLProgram::isLinked(void) const
{
	return linked;
}

void GLSLProgram::bindAttribLocation(GLuint location, const char* name)
{
	glBindAttribLocation(handle, location, name);
}

void GLSLProgram::bindFragDataLocation(GLuint location, const char* name)
{
	glBindFragDataLocation(handle, location, name);
}

void GLSLProgram::setUniform(const char* name, float x, float y, float z)
{
	int location = getUniformLocation(name);

	if (location < 0)
	{
		if (verbose)
			std::cout << "Uniform \"" << name << "\" not found" << std::endl;
	}
	else
	{
		glUniform3f(location, x, y, z);
	}
}

void GLSLProgram::setUniform(const char* name, const glm::vec3& v)
{
	setUniform(name, v.x, v.y, v.z);
}

void GLSLProgram::setUniform(const char* name, const glm::vec4& v)
{
	int location = getUniformLocation(name);

	if (location < 0)
	{
		if (verbose)
			std::cout << "Uniform \"" << name << "\" not found" << std::endl;
	}
	else
	{
		glUniform4f(location, v.x, v.y, v.z, v.w);
	}
}

void GLSLProgram::setUniform(const char* name, const glm::mat3& m)
{
	int location = getUniformLocation(name);

	if (location < 0)
	{
		if (verbose)
			std::cout << "Uniform \"" << name << "\" not found" << std::endl;
	}
	else
	{
		glUniformMatrix3fv(location, 1, GL_FALSE, &m[0][0]);
	}
}

void GLSLProgram::setUniform(const char* name, const glm::mat4& m)
{
	int location = getUniformLocation(name);

	if (location < 0)
	{
		if (verbose)
			std::cout << "Uniform \"" << name << "\" not found" << std::endl;
	}
	else
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, &m[0][0]);
	}
}

void GLSLProgram::setUniform(const char* name, float value)
{
	int location = getUniformLocation(name);

	if (location < 0)
	{
		if (verbose)
			std::cout << "Uniform \"" << name << "\" not found" << std::endl;
	}
	else
	{
		glUniform1f(location, value);
	}
}

void GLSLProgram::setUniform(const char* name, int value)
{
	int location = getUniformLocation(name);

	if (location < 0)
	{
		if (verbose)
			std::cout << "Uniform \"" << name << "\" not found" << std::endl;
	}
	else
	{
		glUniform1i(location, value);
	}
}

void GLSLProgram::setUniform(const char* name, bool value)
{
	setUniform(name, (int) value);
}

void GLSLProgram::setUniform(const char* name, int size, const glm::mat4* value)
{
	int location = getUniformLocation(name);

	if (location < 0)
	{
		if (verbose)
			std::cout << "Uniform \"" << name << "\" not found" << std::endl;
	}
	else
	{
		glUniformMatrix4fv(location, size, GL_FALSE, glm::value_ptr(value[0]));
	}
}

void GLSLProgram::printActiveUniforms(void)
{
}

void GLSLProgram::printActiveAttribs(void)
{
}

int GLSLProgram::getUniformLocation(const char* name)
{
	if (handle < 1 || !linked)
	{
		return -1;
	}

	return glGetUniformLocation(handle, name);
}

bool GLSLProgram::fileExists(const std::string& filename)
{
	std::fstream file(filename);

	bool result = !file;

	if (result)
	{
		file.close();
	}

	return result;
}
