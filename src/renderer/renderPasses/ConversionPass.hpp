#pragma once
#include "RenderPass.hpp"

class ConversionPass : RenderPass {
public:
    ~ConversionPass() = default;
    void execute(const std::string& meshFilePath, const std::string& baseFolder,
                   int resolution, float gaussianStd,
                   GLuint converterShaderProgram, GLuint computeShaderProgram,
                   std::vector<std::pair<Mesh, GLMesh>>& dataMeshAndGlMesh,
                   int normalizedUvSpaceWidth, int normalizedUvSpaceHeight,
                   std::map<std::string, TextureDataGl>& textureTypeMap,
                   GLuint& gaussianBuffer, GLuint &drawIndirectBuffer);

    bool isEnabled() const { return isPassEnabled; }
    void setIsEnabled(bool isPassEnabled) { this->isPassEnabled = isPassEnabled; };
private:
    bool isPassEnabled;
    void conversion(
        GLuint shaderProgram, GLuint vao,
        GLuint framebuffer, size_t vertexCount,
        int normalizedUVSpaceWidth, int normalizedUVSpaceHeight,
        const std::map<std::string, TextureDataGl>& textureTypeMap, MaterialGltf material, unsigned int referenceResolution, float GAUSSIAN_STD
    );
    void aggregation(GLuint& computeShaderProgram, GLuint* drawBuffers, GLuint& gaussianBuffer, GLuint& drawIndirectBuffer, unsigned int resolutionTarget);
};