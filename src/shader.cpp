#include "shader.hpp"
#include "log.hpp"

#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

Shader::Shader() {
    this->program = glCreateProgram();
}

Shader::~Shader() {
    glDeleteProgram(this->program);
}

void Shader::bind() {
    glUseProgram(this->program);
}

void Shader::unbind() {
    glUseProgram(0);
}

GLint Shader::getUniformLocation(const string& name) {
    GLint uniform;

    // See if we have the uniform stored in uniformLocations
    if (this->uniformLocations.find(name) == this->uniformLocations.end()) {
        // Lookup uniform location from the shader
        uniform = glGetUniformLocation(this->program, name.c_str());

        if (uniform == -1) {
            info("Unknown uniform: %s\n", name.c_str());
        }

        this->uniformLocations[name] = uniform;
    } else {
        // Lookup uniform location from uniformLocations
        uniform = this->uniformLocations[name];
    }

    return uniform;
}

void Shader::setUniformTexture(const string& name, GLuint value) {
    GLint uniform = this->getUniformLocation(name);

    if (uniform != -1) {
        glUniform1i(uniform, value);
    }
}

void Shader::setUniformIntv(const string& name, int* pointer, size_t count) {
    GLint uniform = this->getUniformLocation(name);

    if (uniform != -1) {
        glUniform1iv(uniform, count, pointer);
    }
}

void Shader::setUniformUIntv(const string& name, unsigned int* pointer, size_t count) {
    GLint uniform = this->getUniformLocation(name);

    if (uniform != -1) {
        glUniform1uiv(uniform, count, pointer);
    }
}

void Shader::setUniformInt(const string& name, int value) {
    GLint uniform = this->getUniformLocation(name);

    if (uniform != -1) {
        glUniform1i(uniform, value);
    }
}

void Shader::setUniformUInt(const string& name, unsigned int value) {
    GLint uniform = this->getUniformLocation(name);

    if (uniform != -1) {
        glUniform1ui(uniform, value);
    }
}

void Shader::setUniformMatrix(const string& name, glm::mat4 matrix) {
    GLint uniform = this->getUniformLocation(name);

    if (uniform != -1) {
        glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(matrix));
    }
}

void Shader::setUniformFloatv(const string& name, float* pointer, size_t count) {
    GLint uniform = this->getUniformLocation(name);

    if (uniform != -1) {
        glUniform1fv(uniform, count, pointer);
    }
}

void Shader::setUniformFloat(const string& name, float value) {
    GLint uniform = this->getUniformLocation(name);

    if (uniform != -1) {
        glUniform1f(uniform, value);
    }
}

void Shader::setUniformVec3(const string& name, const glm::vec3 value) {
    GLint uniform = this->getUniformLocation(name);

    if (uniform != -1) {
        glUniform3f(uniform, value.r, value.g, value.b);
    }
}

void Shader::setUniformVec4(const string& name, const glm::vec4 value) {
    GLint uniform = this->getUniformLocation(name);

    if (uniform != -1) {
        glUniform4f(uniform, value.r, value.g, value.b, value.a);
    }
}

const string Shader::readFile(const string& path) {
    string code;
    ifstream file;

    file.exceptions(ifstream::failbit | ifstream::badbit);

    try {
        // Opens, reads the file, and then close it
        file.open(path);
        stringstream stream;

        stream << file.rdbuf();

        file.close();

        code = stream.str();
    } catch (ifstream::failure e) {
        critical("Cannot read file: %s\n", path.c_str());
    }

    return code;
}

void Shader::attachSource(const string& path, GLenum type) {
    // Reads the file
    string code = this->readFile(path);
    const char* shaderCode;

    GLint success;
    GLuint shader;

    // Process includes
    size_t index = code.find("#include");
    size_t startIndex;
    size_t endIndex;
    string includePath, includeCode;
    while (index != string::npos) {
        // Replace #include ... with included file's contents
        startIndex = code.find("<", index);

        if (startIndex == string::npos) {
            goto doError;
        }

        endIndex = code.find(">", startIndex);

        if (index == string::npos) {
            goto doError;
        }

        includePath = code.substr(startIndex+1, endIndex-startIndex-1);
        includeCode = this->readFile(includePath);
        code.replace(index, endIndex-index+1, includeCode);
        
        index = code.find("#include");
    }

    // Upload file contents
    shaderCode = code.c_str();

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);

    // Check the status of the shader compilation
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        // Get the error log
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        logLength++;
        char* infoLog = new char[logLength];

        glGetShaderInfoLog(shader, logLength, NULL, infoLog);
        critical("Shader error(%s)\n", infoLog);
    }

    // Attach it to the shader program
    glAttachShader(this->program, shader);
    glDeleteShader(shader);

    return;

doError:
    critical("Malformed shader: %s\n", path.c_str());
}

bool Shader::build() {
    GLint success;

    // Link shader
    glLinkProgram(this->program);

    // Check link status
    glGetProgramiv(this->program, GL_LINK_STATUS, &success);
    if (!success) {
        // Get error log
        GLint logLength;
        glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &logLength);
        logLength++;
        char* infoLog = new char[logLength];

        glGetProgramInfoLog(this->program, logLength, nullptr, infoLog);
        critical("Shader link error: %s\n", infoLog);

        delete[] infoLog;
        return false;
    }

    return true;
}

DefaultShader::DefaultShader(const string& vertex, const string& fragment) {
    this->vertex = vertex;
    this->fragment = fragment;

    // Attach vertex and fragment shaders, and build
    this->attachSource(this->vertex, GL_VERTEX_SHADER);
    this->attachSource(this->fragment, GL_FRAGMENT_SHADER);
    if (!this->build()) {
        critical("Could not build shader (%s, %s)\n", this->vertex.c_str(), this->fragment.c_str());
    }
}
