#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.hpp>

#include "rendering/model.hpp"
#include "helper/log.hpp"

#include <vector>
#include <map>

using namespace tinyobj;
using namespace std;
using namespace glm;

#define USE_DIFFUSE_TEXTURE (1<<0)
#define USE_SPECULAR_TEXTURE (1<<1)
#define USE_ALPHA_TEXTURE (1<<2)

#define POSITION_OFFSET 0
#define NORMAL_OFFSET 1
#define UV_OFFSET 2
#define COLOR_OFFSET 3

#define GET_POSITION(base) (base[POSITION_OFFSET])
#define GET_NORMAL(base) (base[NORMAL_OFFSET])
#define GET_UV(base) (base[UV_OFFSET])
#define GET_COLOR(base) (base[COLOR_OFFSET])

Model::Model(const string& path, const string& base) {
    info("Loading model: %s\n", path.c_str());

    this->vao = new BufferArray();

    // Adapted from https://github.com/tinyobjloader/tinyobjloader#example-code-new-object-oriented-api
    // Loads a model and material file
    ObjReaderConfig config;
    config.mtl_search_path = base;
    config.triangulate = true;
    ObjReader reader;

    if (!reader.ParseFromFile(path, config)) {
        if (!reader.Error().empty()) {
            critical("TinyObjReader: %s", reader.Error());
        }
        critical("No such file: %s\n", path.c_str());
    }

    if (!reader.Warning().empty()) {
        info("TinyObjReader: %s", reader.Warning());
    }

    // Gets reader data
    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    vector<float> positions;
    vector<float> normals;
    vector<float> uvs;
    vector<float> colors;
    vector<int> mat_index;

    bool hasUVs = attrib.texcoords.size() != 0;
    bool hasColors = attrib.colors.size() != 0;

    // Unpack vertex data
    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces
        size_t index = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over verticies in face
            for (size_t v = 0; v < fv; v++) {
                index_t idx = shapes[s].mesh.indices[index + v];

                // Unpack position data
                positions.push_back(attrib.vertices[3*idx.vertex_index+0]);
                positions.push_back(attrib.vertices[3*idx.vertex_index+1]);
                positions.push_back(attrib.vertices[3*idx.vertex_index+2]);

                // Unpack normal data
                normals.push_back(attrib.normals[3*idx.normal_index+0]);
                normals.push_back(attrib.normals[3*idx.normal_index+1]);
                normals.push_back(attrib.normals[3*idx.normal_index+2]);

                // Unpack UV data
                if (hasUVs) {
                    uvs.push_back(attrib.texcoords[2*idx.texcoord_index+0]);
                    uvs.push_back(attrib.texcoords[2*idx.texcoord_index+1]);
                } else {
                    uvs.push_back(0.0f);
                    uvs.push_back(0.0f);
                }

                // Unpack color data
                if (hasColors) {
                    colors.push_back(attrib.colors[3*idx.vertex_index+0]);
                    colors.push_back(attrib.colors[3*idx.vertex_index+1]);
                    colors.push_back(attrib.colors[3*idx.vertex_index+2]);
                } else {
                    colors.push_back(0.8f);
                    colors.push_back(0.8f);
                    colors.push_back(0.8f);
                }

                int mat = shapes[s].mesh.material_ids[f];
                if (mat < 0) {
                    info("Material index: %i\n", mat);
                }
                mat_index.push_back(shapes[s].mesh.material_ids[f]);
            }
            index += fv;
        }
    }

    // Get the number of verticies
    this->count = positions.size() / 3;

    // Upload model data
    this->vao->bind();

    // Position data
    this->buffers[0] = new BufferData(sizeof(float) * positions.size(), positions.data());
    this->vao->setAttribute(0, 3, GL_FLOAT, 0, 0);

    // Normal data
    this->buffers[1] = new BufferData(sizeof(float) * normals.size(), normals.data());
    this->vao->setAttribute(1, 3, GL_FLOAT, 0, 0);

    // UV data
    this->buffers[2] = new BufferData(sizeof(float) * uvs.size(), uvs.data());
    this->vao->setAttribute(4, 2, GL_FLOAT, 0, 0);

    // Color data
    this->buffers[3] = new BufferData(sizeof(float) * colors.size(), colors.data());
    this->vao->setAttribute(5, 3, GL_FLOAT, 0, 0);
    
    // Upload material index data
    this->materialIndex = new BufferData(sizeof(unsigned int) * mat_index.size(), mat_index.data());
    this->vao->setAttributeI(6, 1, GL_INT, 0, 0);

    string diffuseTexture;
    string specularTexture;
    string alphaTexture;

    unsigned int mapMask = 0;
    
    // Unpack and process material data
    info("Material count: %i\n", materials.size());
    for (size_t m = 0; m < materials.size(); m++) {
        unsigned int mask = 0;

        // Unpack texture data
        diffuseTexture = materials[m].diffuse_texname;
        specularTexture = materials[m].specular_texname;
        alphaTexture = materials[m].alpha_texname;

        // See if the textures exists and if it needs to be
        // uploaded to the GPU

        // Texture exists and needs to be uploaded

        // Diffuse
        if (!diffuseTexture.empty()) {
            this->textureIndexes.push_back(this->textures.size());
            this->textures.push_back(new Texture(base + diffuseTexture));
            mask |= USE_DIFFUSE_TEXTURE;
        } else {
            this->textureIndexes.push_back(0);
        }

        // Specular
        if (!specularTexture.empty()) {
            this->textureIndexes.push_back(this->textures.size());
            this->textures.push_back(new Texture(base + specularTexture));
            mask |= USE_SPECULAR_TEXTURE;
        } else {
            this->textureIndexes.push_back(0);
        }

        // Alpha
        if (!alphaTexture.empty()) {
            this->textureIndexes.push_back(this->textures.size());
            this->textures.push_back(new Texture(base + alphaTexture));
            mask |= USE_ALPHA_TEXTURE;
        } else {
            this->textureIndexes.push_back(0);
        }

        // Unpack color values
        this->diffuseColors.push_back(materials[m].diffuse[0]);
        this->diffuseColors.push_back(materials[m].diffuse[1]);
        this->diffuseColors.push_back(materials[m].diffuse[2]);

        // Unpack specular color values
        this->specularColors.push_back(materials[m].specular[0]);
        this->specularColors.push_back(materials[m].specular[1]);
        this->specularColors.push_back(materials[m].specular[2]);

        // Unpack alpha values
        this->alphaValues.push_back(materials[m].dissolve);

        // Set maps mask
        mapsMask.push_back(mask);
    }
}

Model::~Model() {
    // Delete textures, buffers, and vertex attribute object
    for (auto& tex : this->textures) {
        delete tex;
    }

    delete this->materialIndex;

    for (auto& buffer : this->buffers) {
        delete buffer;
    }

    delete this->vao;
}

void Model::draw(Shader* shader) {
    shader->bind();

    // Calculates model matrix
    mat4 M = this->transform.getMatrix();
    shader->setUniformMatrix("M", M);

    this->vao->bind();

    // Upload texture data
    int* textureList = new int[textures.size()];
    for (GLuint i = 0; i < textures.size(); i++) {
        if (this->textures[i] != nullptr) {
            this->textures[i]->bind(i);
            textureList[i] = i;
        } else {
            textureList[i] = 0;
        }
    }
    
    // Upload material textures
    shader->setUniformIntv("matTextures", textureList, textures.size());
    delete[] textureList;

    shader->setUniformUIntv("textureIndexes", this->textureIndexes.data(), this->textureIndexes.size());

    // Upload material maps and count
    shader->setUniformUIntv("mapsMask", this->mapsMask.data(), this->mapsMask.size());
    shader->setUniformUInt("matCount", this->mapsMask.size());
    
    GLint uniform;

    // Upload material colors
    uniform = shader->getUniformLocation("matDiffuse");
    if (uniform != -1) {
        glUniform3fv(uniform, this->diffuseColors.size()/3, this->diffuseColors.data());
    }

    uniform = shader->getUniformLocation("matSpecular");
    if (uniform != -1) {
        glUniform3fv(uniform, this->specularColors.size()/3, this->specularColors.data());
    }

    shader->setUniformFloatv("matAlpha", this->alphaValues.data(), this->alphaValues.size());

    // Draw
    glDrawArrays(GL_TRIANGLES, 0, this->count);

    this->vao->unbind();
}
