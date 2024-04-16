#pragma once
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include "utils.hpp"

#define DEFAULT_MATERIAL_NAME "mm_default_material_mm"

std::tuple<std::vector<Mesh>, std::vector<glm::vec3>, std::vector<glm::vec2>, std::vector<glm::vec3>> parseObjFileToMeshes(const std::string& filename);

std::map<std::string, Material> parseMtlFile(const std::string& filename);