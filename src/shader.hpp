#pragma once

#include "opengl.hpp"
#include "glm/glm.hpp"

#include <string>
#include <map>

// A class to create a shader object and handle
// shader data
class Shader {
public:
    // Creates shader object
    Shader();
    ~Shader();

    // Binds this as the current shader
    void bind();
    // Unbinds the shader
    void unbind();

    /* Looks up a uniform location in the current shader
     * @param name The name of the uniform
     * @returns The location of the uniform. -1 is return is the uniform is not found
    */
    GLint getUniformLocation(const std::string& name);

    /* Uploads an unsigned int as the texture unit to use
     * @param name The uniform name
     * @param value The texture unit
    */
    void setUniformTexture(const std::string& name, GLuint value);
    /* Uploads an array of ints
     * @param name The uniform name
     * @param pointer A pointer to an array of ints
     * @param count The number of ints in the array
    */
    void setUniformIntv(const std::string& name, int* pointer, size_t count);
    /* Uploads an array of unsigned ints
     * @param name The uniform name
     * @param pointer A pointer to an array of unsigned ints
     * @param count The number of unsigned ints in the array
    */
    void setUniformUIntv(const std::string& name, unsigned int* pointer, size_t count);
    /* Uploads an int vaule
     * @param name The uniform name
     * @param value The int to upload
    */
    void setUniformInt(const std::string& name, int value);
    /* Uploads an unsigned int vaule
     * @param name The uniform name
     * @param value The unsigned int to upload
    */
    void setUniformUInt(const std::string& name, unsigned int value);
    /* Uploads a matrix
     * @param name The uniform name
     * @param value The matrix to upload
    */
    void setUniformMatrix(const std::string& name, glm::mat4 matrix);
    /* Uploads an array of floats
     * @param name The uniform name
     * @param pointer A pointer to an array of floats
     * @param count The number of floats in the array
    */
    void setUniformFloatv(const std::string& name, float* pointer, size_t count);
    /* Uploads a float
     * @param name The uniform name
     * @param value The float to upload
    */
    void setUniformFloat(const std::string& name, float value);
    /* Uploads a vec3
     * @param name The uniform name
     * @param value The vec3 to upload
    */
    void setUniformVec3(const std::string& name, const glm::vec3 value);
    /* Uploads a vec4
     * @param name The uniform name
     * @param value The vec4 to upload
    */
    void setUniformVec4(const std::string& name, const glm::vec4 value);

protected:
    /* A helper function that reads files
     * @param path The file path to read
     * @returns The contents of the file
    */
    const std::string readFile(const std::string& path);
    /* Uploads the contents of the given file as a shader
     * @param path The path of the file to upload
     * @param type The shader type to use the file as
    */
    void attachSource(const std::string& path, GLenum type);

    // Links the the shader
    // @returns Returns true if the shader links successfuly
    bool build();

    GLuint program;

private:
    std::map<std::string, GLint> uniformLocations;
};

// The shader class for shaders with only a vertex and fragment
// shader stages
class DefaultShader : public Shader {
public:
    /* Creates a shader with a vertex and fragment shader stage
     * @param vertex The file path for the vertex stage
     * @param fragment The file path for the fragment stage
    */
    DefaultShader(const std::string& vertex, const std::string& fragment);

private:
    std::string vertex;
    std::string fragment;
};
