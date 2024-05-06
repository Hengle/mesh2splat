#pragma once

#include "utils/utils.hpp"

std::pair<unsigned char*, int> loadImage(std::string texturePath, int& textureWidth, int& textureHeight);

std::vector<Mesh> parseGltfFileToMesh(const std::string& filename);

void writeBinaryPLY(const std::string& filename, const std::vector<Gaussian3D>& gaussians);

