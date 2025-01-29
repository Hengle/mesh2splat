#include "GaussiansPrepass.hpp"

void GaussiansPrepass::execute(RenderContext& renderContext)
{

#ifdef  _DEBUG
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, PassesDebugIDs::GAUSSIAN_SPLATTING_PREPASS, -1, "GAUSSIAN_SPLATTING_PREPASS");
#endif 

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, renderContext.drawIndirectBuffer);

    DrawElementsIndirectCommand* cmd = (DrawElementsIndirectCommand*)glMapBufferRange(
        GL_DRAW_INDIRECT_BUFFER, 0, sizeof(DrawElementsIndirectCommand), GL_MAP_WRITE_BIT 
    );

    unsigned int validCount = cmd->instanceCount;
    cmd->instanceCount = 0;
    //glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(DrawElementsIndirectCommand), &cmd);
    glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);

    // Transform Gaussian positions to view space and apply global sort
    glUseProgram(renderContext.shaderPrograms.computeShaderGaussianPrepassProgram);

    glUtils::setUniform1f(renderContext.shaderPrograms.computeShaderGaussianPrepassProgram,     "u_stdDev", renderContext.gaussianStd / (float(renderContext.resolutionTarget)));
    glUtils::setUniform3f(renderContext.shaderPrograms.computeShaderGaussianPrepassProgram,     "u_hfov_focal", renderContext.hfov_focal);
    glUtils::setUniformMat4(renderContext.shaderPrograms.computeShaderGaussianPrepassProgram,   "u_worldToView", renderContext.viewMat);
    glUtils::setUniformMat4(renderContext.shaderPrograms.computeShaderGaussianPrepassProgram,   "u_viewToClip", renderContext.projMat);
    glUtils::setUniform2f(renderContext.shaderPrograms.computeShaderGaussianPrepassProgram,     "u_resolution", renderContext.rendererResolution);
    glUtils::setUniform3f(renderContext.shaderPrograms.computeShaderGaussianPrepassProgram,     "u_camPos", renderContext.camPos);
    glUtils::setUniform1i(renderContext.shaderPrograms.computeShaderGaussianPrepassProgram,     "u_renderMode", renderContext.renderMode);


    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderContext.gaussianBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, renderContext.gaussianBuffer);

    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderContext.gaussianBufferPostFiltering);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, renderContext.gaussianBufferPostFiltering);

    if(validCount > MAX_GAUSSIANS_TO_SORT) glUtils::resizeAndBindToPosSSBO<glm::vec4>(validCount * 3, renderContext.perQuadTransformationsBuffer, 2);

    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderContext.perQuadTransformationsBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, renderContext.perQuadTransformationsBuffer);

    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderContext.drawIndirectBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, renderContext.drawIndirectBuffer);
    
    //Technically should happen on all of them, right?
    unsigned int threadGroup_xy = (renderContext.readGaussians.size() + 255) / 256;
    glDispatchCompute(threadGroup_xy, 1, 1);
    glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

#ifdef  _DEBUG
    glPopDebugGroup();
#endif 
}
