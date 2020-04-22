#pragma once

#ifndef GLTOOLS_H
#define GLTOOLS_H

#include <iostream>
#include <string>
#include <exception>
#include <sstream>
#include <map>

#include <GL/glew.h>
#include <GL/glu.h>

namespace cg
{
	/* Method 1: glDebugMessageCallback in OpenGL context */
	/*
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(cg::glErrorVerboseCallback, nullptr);
    glDebugMessageControl(..);
	*/
	inline void APIENTRY glErrorVerboseCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam) {
		static std::map<GLenum, std::string> severityString =
		{ { GL_DEBUG_SEVERITY_LOW,  "LOW" },
		{ GL_DEBUG_SEVERITY_MEDIUM, "MEDIUM" },
		{ GL_DEBUG_SEVERITY_HIGH,   "HIGH" }
		};

		static std::map<GLenum, std::string> errorString =
		{ { GL_DEBUG_TYPE_ERROR,             "ERROR" },
		{ GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "DEPRECATED_BEHAVIOR" },
		{ GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,  "UNDEFINED_BEHAVIOR" },
		{ GL_DEBUG_TYPE_PORTABILITY,         "PORTABILITY" },
		{ GL_DEBUG_TYPE_PERFORMANCE,         "PERFORMANCE" },
		{ GL_DEBUG_TYPE_OTHER,               "OTHER" }
		};

		// id is GLError-code
		std::cout << errorString[type] << " (" << severityString[severity] << " priority): " << message << std::endl;
		std::cout << std::endl;
	}

/* Method 2: Function called after OpenGL command sequence. */
/*
 Error output to stream. To be called after OpenGL command sequence!
 */
inline void glErrorVerbose (std::ostream& os, unsigned linenum, bool stop=false) {
  GLenum errCode  = glGetError();
  if    (errCode != GL_NO_ERROR) {
     os << "[" << linenum << "] " << gluErrorString(errCode) << std::endl;
     if (stop) {
        exit(-1);
     }
  }
}

/* Method 3: GLException out of macro GLCODE. */
/* 
 GLException
 */
class GLException : public std::runtime_error
{
public:
	GLException(GLenum code, unsigned linenum) 
		: std::runtime_error(createString(code, linenum)),
		  m_code(code), m_line(linenum) {
	}

	static void throwGLException(unsigned linenum) {
		GLenum errCode = glGetError();
		if (errCode != GL_NO_ERROR) {
			throw new GLException(errCode, linenum); 
		}
	}
	static std::string createString (GLenum code, unsigned linenum) {
		if (code != GL_NO_ERROR) {
			std::ostringstream sstr;
			sstr << "[" << linenum << "] " << gluErrorString(code);
			return sstr.str();
		}
		return std::string();
	}

	GLenum   getErrorCode  () const {
		return m_code;
	}
	unsigned getLineNumber () const {
		return m_line;
	}

private:
	GLenum   m_code;
	unsigned m_line;
};
#define GLCODE(call) call;                 \
  cg::GLException::throwGLException(__LINE__)

  
};

#endif