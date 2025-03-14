#pragma once
#include "utils.hpp"
#include "parsers/parsers.hpp"
#include "xatlas/xatlas.h"
#include "stb_image_write.h"

namespace uvUnwrapping
{
	void generateNormalizedUvCoordinatesPerMesh(int& uvSpaceWidth, int& uvSpaceHeight, std::vector<utils::Mesh>& meshes);

	void generateNormalizedUvCoordinatesPerFace(int& uvSpaceWidth, int& uvSpaceHeight, utils::Mesh& meshes);
}

