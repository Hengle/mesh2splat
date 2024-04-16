#pragma once
#include <string>
#include <vector>
#include <glm.hpp>

struct Material {
    glm::vec3 ambient;       // Ka
    glm::vec3 diffuse;       // Kd
    glm::vec3 specular;      // Ks
    float specularExponent;  // Ns
    float transparency;      // d or Tr
    float opticalDensity;    // Ni
    std::string diffuseMap;  // map_Kd, texture map

    Material() : ambient(0.0f), diffuse(0.0f), specular(0.0f), specularExponent(0.0f), transparency(1.0f), opticalDensity(1.0f) {}
};


struct Gaussian3D {
    Gaussian3D(glm::vec3 position, glm::vec3 normal, glm::vec3 scale, glm::vec4 rotation, glm::vec3 RGB, float opacity, Material material)
        : position(position), normal(normal), scale(scale), rotation(rotation), sh0(RGB), opacity(opacity), material(material) {};
    Gaussian3D() : position(NULL), normal(NULL), scale(NULL), rotation(NULL), sh0(NULL), opacity(NULL), material({}) {};
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 scale;
    glm::vec4 rotation;
    glm::vec3 sh0;
    float opacity;
    Material material;
};

struct Face {
    std::vector<int> vertexIndices;
    std::vector<int> uvIndices;
    std::vector<int> normalIndices;
    std::string materialName;  // Added to store material name for each face

    Face(const std::vector<int>& verts, const std::vector<int>& uvs, const std::vector<int>& norms, const std::string& matName)
        : vertexIndices(verts), uvIndices(uvs), normalIndices(norms), materialName(matName) {}
};

// Class to represent a mesh
class Mesh {
public:
    std::string name;
    std::vector<Face> faces; // Tuple of vertex indices, uv indices and normalIndices

    Mesh(const std::string& name = "Unnamed") : name(name) {}
};

