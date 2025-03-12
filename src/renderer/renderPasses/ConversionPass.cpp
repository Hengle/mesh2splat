#pragma once
#include "ConversionPass.hpp"

void ConversionPass::execute(RenderContext &renderContext)
{
    
    //TODO: If model has many meshes this is probably not the most efficient approach.
    //For how the mesh2splat method currently works, we still need to generate a separate frame and drawbuffer per mesh, but the gpu conversion
    //could be done via batch rendering
    //If we are running the conversion pass means the currently existing framebuffer with respective draw buffers should be deleted before the conversion passes
    renderContext.numberOfGaussians = 0;
    glUtils::resetAtomicCounter(renderContext.atomicCounterBufferConversionPass);

    GLsizeiptr bufferSize = renderContext.resolutionTarget * renderContext.resolutionTarget * renderContext.dataMeshAndGlMesh.size() * sizeof(glm::vec4) * 6;
    GLint currentSize;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderContext.gaussianBuffer);    
    glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &currentSize);
    if (currentSize != bufferSize) {
        glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
    }
    
    for (auto& mesh : renderContext.dataMeshAndGlMesh) {
        GLuint framebuffer;
        GLuint drawBuffers = glUtils::setupFrameBuffer(framebuffer, renderContext.resolutionTarget, renderContext.resolutionTarget);

        conversion(renderContext, mesh, framebuffer);
        glFinish();
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, renderContext.atomicCounterBufferConversionPass);
        uint32_t numGs;
        glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(uint32_t), &numGs);
        renderContext.numberOfGaussians += numGs;

        glDeleteRenderbuffers(1, &drawBuffers); 
        glDeleteFramebuffers(1, &framebuffer);
    }
}


void ConversionPass::conversion(
        RenderContext renderContext, std::pair<utils::Mesh, utils::GLMesh> mesh, GLuint dummyFramebuffer
    ) 
{
#ifdef  _DEBUG
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, PassesDebugIDs::CONVERSION_PASS, -1, "CONVERSION_VS_GS_PS_PASS");
#endif 
    // Use shader program and perform tessellation
    glUseProgram(renderContext.shaderPrograms.converterShaderProgram);
    glViewport(0, 0, renderContext.resolutionTarget, renderContext.resolutionTarget);

    //-------------------------------SET UNIFORMS-------------------------------   
    //Textures
    if (renderContext.meshToTextureData.find(mesh.first.name) != renderContext.meshToTextureData.end())
    {
        auto& textureMap = renderContext.meshToTextureData.at(mesh.first.name);

        if (textureMap.find(BASE_COLOR_TEXTURE) != textureMap.end())
        {
            glUtils::setTexture2D(renderContext.shaderPrograms.converterShaderProgram, "albedoTexture", textureMap.at(BASE_COLOR_TEXTURE).glTextureID, 0);
            glUtils::setUniform1i(renderContext.shaderPrograms.converterShaderProgram, "hasAlbedoMap", 1);
        }
        if (textureMap.find(NORMAL_TEXTURE) != textureMap.end())
        {
            glUtils::setTexture2D(renderContext.shaderPrograms.converterShaderProgram, "normalTexture", textureMap.at(NORMAL_TEXTURE).glTextureID,         1);
            glUtils::setUniform1i(renderContext.shaderPrograms.converterShaderProgram, "hasNormalMap", 1);
        }
        if (textureMap.find(METALLIC_ROUGHNESS_TEXTURE) != textureMap.end())
        {
            glUtils::setTexture2D(renderContext.shaderPrograms.converterShaderProgram, "metallicRoughnessTexture", textureMap.at(METALLIC_ROUGHNESS_TEXTURE).glTextureID,     2);
            glUtils::setUniform1i(renderContext.shaderPrograms.converterShaderProgram, "hasMetallicRoughnessMap", 1);
        }
        if (textureMap.find(AO_TEXTURE) != textureMap.end())
        {
            glUtils::setTexture2D(renderContext.shaderPrograms.converterShaderProgram, "occlusionTexture", textureMap.at(AO_TEXTURE).glTextureID,          3);
        }
        if (textureMap.find(EMISSIVE_TEXTURE) != textureMap.end())
        {
            glUtils::setTexture2D(renderContext.shaderPrograms.converterShaderProgram, "emissiveTexture", textureMap.at(EMISSIVE_TEXTURE).glTextureID,     4);
        }
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, renderContext.gaussianBuffer);    
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, renderContext.atomicCounterBufferConversionPass);
    glBindVertexArray(mesh.second.vao);
    glBindFramebuffer(GL_FRAMEBUFFER, dummyFramebuffer);

    glDrawArrays(GL_TRIANGLES, 0, mesh.second.vertexCount); 

#ifdef  _DEBUG
    glPopDebugGroup();
#endif 
 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}