#pragma once

#include "utils/utils.hpp"

std::pair<unsigned char*, int> loadImageAndBpp(std::string texturePath, int& textureWidth, int& textureHeight);

void loadAllTexturesIntoMap(MaterialGltf& material, std::map<std::string, std::pair<unsigned char*, int>>& textureTypeMap);

std::vector<Mesh> parseGltfFileToMesh(const std::string& filename);

void writeBinaryPLY(const std::string& filename, const std::vector<Gaussian3D>& gaussians);

