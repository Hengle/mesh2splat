#pragma once
#include "utils.hpp"
#include "../parsers/parsers.hpp"
#include "../../thirdParty/xatlas/xatlas.h"
#include "../../thirdParty/stb_image_write.h"

namespace uvUnwrapping
{
	void generateNormalizedUvCoordinatesPerMesh(int& uvSpaceWidth, int& uvSpaceHeight, std::vector<Mesh>& meshes);

	void generateNormalizedUvCoordinatesPerFace(int& uvSpaceWidth, int& uvSpaceHeight, Mesh& meshes);
}

