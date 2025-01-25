#pragma once
#include "RenderPass.hpp"
#include "../../radixSort/RadixSort.hpp"

class RadixSortPass : RenderPass {
public:
    ~RadixSortPass() = default;
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
    
    void computeKeyValuesPre(
        GLuint radixSortPrepassProgram, 
        GLuint drawIndirectBuffer, GLuint gaussianBuffer,
        GLuint keysBuffer, GLuint valuesBuffer,
        glm::mat4 viewMatrix
    );
    
    void sort(GLuint keysBuffer, GLuint valuesBuffer, unsigned int validCount);
    
    void gatherPost(
        GLuint radixSortGatherProgram,
        GLuint gaussianBuffer, GLuint gaussianBufferSorted,
        GLuint valuesBuffer, GLuint drawIndirectBuffer,
        unsigned int validCount
    );
};