#pragma once

#include "utils/utils.hpp"

std::pair<unsigned char*, int> loadImage(std::string texturePath, int& textureWidth, int& textureHeight);

std::pair<bool, int> loadImageIntoVector(std::string filepath, int& width, int& height, std::vector<unsigned char>& vecToFill);

//std::tuple<std::vector<Mesh>, std::vector<glm::vec3>, std::vector<glm::vec2>, std::vector<glm::vec3>> parseObjFileToMeshes(const std::string& filename);

//std::map<std::string, Material> parseMtlFile(const std::string& filename);

std::tuple<std::vector<Mesh>, std::vector<glm::vec3>, std::vector<glm::vec2>, std::vector<glm::vec3>> parseGltfFileToMesh(const std::string& filename);

void writeBinaryPLY(const std::string& filename, const std::vector<Gaussian3D>& gaussians);

