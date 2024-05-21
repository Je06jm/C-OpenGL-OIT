#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.hpp>

#include "helper/log.hpp"
#include "rendering/model.hpp"

#include <map>
#include <vector>
#include <unordered_map>

using namespace tinyobj;
using namespace std;
using namespace glm;

Model::Model(const string &path, const string &base) {
  info("Loading model: %s\n", path.c_str());

  // Adapted from
  // https://github.com/tinyobjloader/tinyobjloader#example-code-new-object-oriented-api
  // Loads a model and material file
  ObjReaderConfig config;
  config.mtl_search_path = base;
  config.triangulate = true;
  ObjReader reader;

  if (!reader.ParseFromFile(path, config)) {
    if (!reader.Error().empty()) {
      critical("TinyObjReader: %s\n", reader.Error());
    }
    critical("No such file: %s\n", path.c_str());
  }

  if (!reader.Warning().empty()) {
    info("TinyObjReader: %s\n", reader.Warning());
  }

  // Gets reader data
  auto &attrib = reader.GetAttrib();
  auto &shapes = reader.GetShapes();
  auto &materials = reader.GetMaterials();

  unordered_map<int, vector<float>> positions;
  unordered_map<int, vector<float>> normals;
  unordered_map<int, vector<float>> uvs;
  unordered_map<int, vector<float>> colors;
  unordered_map<int, Material> mats;

  unordered_map<string, shared_ptr<Texture>> diffuseTextures;
  unordered_map<string, shared_ptr<Texture>> specularTextures;
  unordered_map<string, shared_ptr<Texture>> alphaTextures;

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

        int mat = shapes[s].mesh.material_ids[f];

        auto &pos = positions[mat];
        auto &norm = normals[mat];
        auto &uv = uvs[mat];
        auto &color = colors[mat];

        // Unpack position data
        pos.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
        pos.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
        pos.push_back(attrib.vertices[3 * idx.vertex_index + 2]);

        // Unpack normal data
        norm.push_back(attrib.normals[3 * idx.normal_index + 0]);
        norm.push_back(attrib.normals[3 * idx.normal_index + 1]);
        norm.push_back(attrib.normals[3 * idx.normal_index + 2]);

        // Unpack UV data
        if (hasUVs) {
          uv.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
          uv.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
        } else {
          uv.push_back(0.0f);
          uv.push_back(0.0f);
        }

        // Unpack color data
        if (hasColors) {
          color.push_back(attrib.colors[3 * idx.vertex_index + 0]);
          color.push_back(attrib.colors[3 * idx.vertex_index + 1]);
          color.push_back(attrib.colors[3 * idx.vertex_index + 2]);
        } else {
          color.push_back(0.8f);
          color.push_back(0.8f);
          color.push_back(0.8f);
        }
      }
      index += fv;
    }
  }

  // Unpack and process material data
  info("Material count: %i\n", materials.size());
  for (size_t mat = 0; mat < materials.size(); mat++) {
    auto m = materials[mat];

    // Unpack texture data
    string diffuseTexture = m.diffuse_texname;
    string specularTexture = m.specular_texname;
    string alphaTexture = m.alpha_texname;

    // See if the textures exists and if it needs to be
    // uploaded to the GPU

    // Texture exists and needs to be uploaded

    mats[mat] = Material{};

    // Diffused
    if (!diffuseTexture.empty()) {
      if (diffuseTextures.find(diffuseTexture) == diffuseTextures.end()) {
        diffuseTextures[diffuseTexture] = Texture::create(base + diffuseTexture);
      }
      mats[mat].diffused = diffuseTextures[diffuseTexture];
      mats[mat].mask |= Material::MASK_USE_DIFFUSED;
    } else {
      mats[mat].diffusedColor.values[0] = m.diffuse[0];
      mats[mat].diffusedColor.values[1] = m.diffuse[1];
      mats[mat].diffusedColor.values[2] = m.diffuse[2];
    }

    // Specular
    if (!specularTexture.empty()) {
      if (specularTextures.find(specularTexture) == specularTextures.end()) {
        specularTextures[specularTexture] = Texture::create(base + specularTexture);
      }
      mats[mat].specular = specularTextures[specularTexture];
      mats[mat].mask |= Material::MASK_USE_SPECULAR;
    } else {
      mats[mat].specularColor.values[0] = m.specular[0];
      mats[mat].specularColor.values[1] = m.specular[1];
      mats[mat].specularColor.values[2] = m.specular[2];
    }

    // Alpha
    if (!alphaTexture.empty()) {
      if (alphaTextures.find(alphaTexture) == alphaTextures.end()) {
        alphaTextures[alphaTexture] = Texture::create(base + alphaTexture);
      }
      mats[mat].alpha = alphaTextures[alphaTexture];
      mats[mat].mask |= Material::MASK_USE_ALPHA;
    } else {
      mats[mat].alphaValue = m.dissolve;
    }
  }

  // Create meshes
  for (auto [i, m] : mats) {
    auto mesh = Mesh::create(positions[i], normals[i], uvs[i], colors[i]);

    this->meshes.push_back(mesh);
    this->materials.push_back(m);
  }
}

void Model::draw(std::shared_ptr<Shader> shader) {
  shader->bind();

  // Calculates model matrix
  mat4 M = this->transform.getMatrix();
  shader->setUniformMatrix("M", M);

  for (size_t i = 0; i < meshes.size(); i++) {
    int t = 0;
    auto &mesh = this->meshes[i];
    auto &mat = this->materials[i];

    if (mat.mask & Material::MASK_USE_DIFFUSED) {
      if (shader->hasUniform("matDiffuse")) {
        shader->setUniformInt("matDiffuse", t);
      }
      mat.diffused->bind(t++);
    } else {
      if (shader->hasUniform("matDiffuseColor")) {
        shader->setUniformVec3("matDiffuseColor", { mat.diffusedColor.r, mat.diffusedColor.g, mat.diffusedColor.b });
      }
    }

    if (mat.mask & Material::MASK_USE_SPECULAR) {
      if (shader->hasUniform("matSpecular")) {
        shader->setUniformInt("matSpecular", t);
      }
      mat.specular->bind(t++);
    } else {
      if (shader->hasUniform("matSpecularColor")) {
        shader->setUniformVec3("matSpecularColor", { mat.specularColor.r, mat.specularColor.g, mat.specularColor.b });
      }
    }

    if (mat.mask & Material::MASK_USE_ALPHA) {
      if (shader->hasUniform("matAlpha")) {
        shader->setUniformInt("matAlpha", t);
      }
      mat.alpha->bind(t++);
    } else {
      if (shader->hasUniform("matAlphaValue")) {
        shader->setUniformFloat("matAlphaValue", mat.alphaValue);
      }
    }

    if (shader->hasUniform("matMask")) {
      shader->setUniformUInt("matMask", mat.mask);
    }

    mesh->draw();
  }
}