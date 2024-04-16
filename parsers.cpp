#include "parsers.hpp"

std::tuple<std::vector<Mesh>, std::vector<glm::vec3>, std::vector<glm::vec2>, std::vector<glm::vec3>> parseObjFileToMeshes(const std::string& filename) {
    std::vector<glm::vec3> globalVertices;
    std::vector<glm::vec2> globalUvs;
    std::vector<glm::vec3> globalNormals;
    std::vector<Mesh> meshes;
    std::string currentMaterial;
    Mesh* currentMesh = nullptr;

    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        if (line.find('o') == 0 || line.find('g') == 0) {
            std::string meshName = line.substr(2);
            meshes.emplace_back(meshName);
            currentMesh = &meshes.back();
        }
        else if (line.find("usemtl") == 0) {
            currentMaterial = line.substr(7); // Capture material name after "usemtl"
        }
        else if (line.find('v') == 0 && line[1] == ' ') {
            glm::vec3 vertex;
            std::istringstream(line.substr(2)) >> vertex.x >> vertex.y >> vertex.z;
            globalVertices.push_back(vertex);
        }
        else if (line.find("vt") == 0) {
            glm::vec2 uv;
            std::istringstream(line.substr(3)) >> uv.x >> uv.y;
            globalUvs.push_back(uv);
        }
        else if (line.find("vn") == 0) {
            glm::vec3 normal;
            std::istringstream(line.substr(3)) >> normal.x >> normal.y >> normal.z;
            globalNormals.push_back(normal);
        }
        else if (line.find('f') == 0) {
            std::vector<int> vertexIndices, uvIndices, normalIndices;
            std::istringstream iss(line.substr(2));
            std::string part;
            while (iss >> part) {
                std::size_t firstSlash = part.find('/');
                std::size_t secondSlash = part.rfind('/');
                int vertIdx = std::stoi(part.substr(0, firstSlash)) - 1;
                vertexIndices.push_back(vertIdx);

                if (firstSlash != std::string::npos && firstSlash + 1 < secondSlash) {
                    int uvIdx = std::stoi(part.substr(firstSlash + 1, secondSlash - firstSlash - 1)) - 1;
                    uvIndices.push_back(uvIdx);
                }

                if (secondSlash != std::string::npos && secondSlash + 1 < part.size()) {
                    int normIdx = std::stoi(part.substr(secondSlash + 1)) - 1;
                    normalIndices.push_back(normIdx);
                }
            }
            if (currentMesh) {
                if (currentMaterial.length() != 0)
                {
                    currentMesh->faces.emplace_back(vertexIndices, uvIndices, normalIndices, currentMaterial);
                }
                else {
                    currentMesh->faces.emplace_back(vertexIndices, uvIndices, normalIndices, DEFAULT_MATERIAL_NAME); // find: [1] to understand
                }
                
            }
        }
    }

    return std::make_tuple(meshes, globalVertices, globalUvs, globalNormals);
}

std::map<std::string, Material> parseMtlFile(const std::string& filename) {
    std::map<std::string, Material> materials;
    std::ifstream file(filename);
    std::string line;
    std::string currentMaterialName;

    //Add default material [1]

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string identifier;
        iss >> identifier;

        if (identifier == "newmtl") {
            iss >> currentMaterialName;
            materials[currentMaterialName] = Material();
        }
        else if (identifier == "Ka") {
            glm::vec3 ka;
            iss >> ka.x >> ka.y >> ka.z;
            materials[currentMaterialName].ambient = ka;
        }
        else if (identifier == "Kd") {
            glm::vec3 kd;
            iss >> kd.x >> kd.y >> kd.z;
            materials[currentMaterialName].diffuse = kd;
        }
        else if (identifier == "Ks") {
            glm::vec3 ks;
            iss >> ks.x >> ks.y >> ks.z;
            materials[currentMaterialName].specular = ks;
        }
        else if (identifier == "Ns") {
            float ns;
            iss >> ns;
            materials[currentMaterialName].specularExponent = ns;
        }
        else if (identifier == "d" || identifier == "Tr") {
            float d;
            iss >> d;
            materials[currentMaterialName].transparency = d;
        }
        else if (identifier == "Ni") {
            float ni;
            iss >> ni;
            materials[currentMaterialName].opticalDensity = ni;
        }
        else if (identifier == "map_Kd") {
            std::string map_kd;
            iss >> map_kd;
            materials[currentMaterialName].diffuseMap = map_kd;
        }
    }

    return materials;
}