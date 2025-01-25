#include "RadixSortPass.hpp"

void RadixSortPass::execute(const std::string& meshFilePath, const std::string& baseFolder,
    int resolution, float gaussianStd,
    GLuint converterShaderProgram, GLuint computeShaderProgram,
    std::vector<std::pair<Mesh, GLMesh>>& dataMeshAndGlMesh,
    int normalizedUvSpaceWidth, int normalizedUvSpaceHeight,
    std::map<std::string, TextureDataGl>& textureTypeMap,
    GLuint& gaussianBuffer, GLuint& drawIndirectBuffer)
{

}

void RadixSortPass::computeKeyValuesPre(
    GLuint radixSortPrepassProgram, 
    GLuint drawIndirectBuffer, GLuint gaussianBuffer,
    GLuint keysBuffer, GLuint valuesBuffer,
    glm::mat4 viewMatrix)
{
     glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawIndirectBuffer);

    DrawArraysIndirectCommand* cmd = (DrawArraysIndirectCommand*)glMapBufferRange(
        GL_DRAW_INDIRECT_BUFFER, 0, sizeof(DrawArraysIndirectCommand), GL_MAP_READ_BIT
    );

    unsigned int validCount = cmd->instanceCount;
    glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);

    // Transform Gaussian positions to view space and apply global sort
    glUseProgram(radixSortPrepassProgram);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, gaussianBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, gaussianBuffer);
    setUniformMat4(radixSortPrepassProgram, "u_view", viewMatrix);
    setUniform1ui(radixSortPrepassProgram, "u_count", validCount);
    setupKeysBufferSsbo(validCount, keysBuffer, 1);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, keysBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, keysBuffer);
    setupValuesBufferSsbo(validCount, valuesBuffer, 2);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, valuesBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, valuesBuffer);
    unsigned int threadGroup_xy = (validCount + 255) / 256;
    glDispatchCompute(threadGroup_xy, 1, 1);
    glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
}

void RadixSortPass::sort(GLuint keysBuffer, GLuint valuesBuffer, unsigned int validCount)
{
    glu::RadixSort sorter;
    sorter(keysBuffer, valuesBuffer, validCount); //Syncronization is already handled within the sorter
};

void RadixSortPass::gatherPost(
    GLuint radixSortGatherProgram,
    GLuint gaussianBuffer, GLuint gaussianBufferSorted,
    GLuint valuesBuffer, GLuint drawIndirectBuffer,
    unsigned int validCount
)
{
    glUseProgram(radixSortGatherProgram);
    setUniform1ui(radixSortGatherProgram, "u_count", validCount);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, gaussianBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, gaussianBuffer);
    setupSortedBufferSsbo(validCount, gaussianBufferSorted, 1); // <-- last int is binding pos
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, gaussianBufferSorted);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, valuesBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, drawIndirectBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, drawIndirectBuffer);
    unsigned int threadGroup_xy = (validCount + 255) / 256;
    glDispatchCompute(threadGroup_xy, 1, 1);

    glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
}
