#include "GaussianRelightingPass.hpp"

GaussianRelightingPass::GaussianRelightingPass()
{
    float quadVertices[] = {
        //    Pos           UV
        -1.0f,  1.0f,  0.0f, 1.0f,  
        -1.0f, -1.0f,  0.0f, 0.0f,  
         1.0f,  1.0f,  1.0f, 1.0f,  
         1.0f, -1.0f,  1.0f, 0.0f   
    };
    unsigned int quadIndices[] = {
        0, 1, 2, 
        1, 3, 2  
    };

    glGenVertexArrays(1, &m_fullscreenQuadVAO);
    glGenBuffers(1, &m_fullscreenQuadVBO);
    glGenBuffers(1, &m_fullscreenQuadEBO);

    glBindVertexArray(m_fullscreenQuadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreenQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_fullscreenQuadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //glBindVertexArray(0);
}

void GaussianRelightingPass::execute(RenderContext& renderContext)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, renderContext.rendererResolution.x, renderContext.rendererResolution.y);

#ifdef  _DEBUG
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, PassesDebugIDs::GAUSSIAN_SPLATTING_RENDER, -1, "GAUSSIAN_SPLATTING_DEFERRED_PASS");
#endif 

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(renderContext.shaderPrograms.deferredRelightingShaderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderContext.gPosition);
    glUniform1i(glGetUniformLocation(renderContext.shaderPrograms.deferredRelightingShaderProgram, "gPosition"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, renderContext.gNormal);
    glUniform1i(glGetUniformLocation(renderContext.shaderPrograms.deferredRelightingShaderProgram, "gNormal"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, renderContext.gAlbedo);
    glUniform1i(glGetUniformLocation(renderContext.shaderPrograms.deferredRelightingShaderProgram, "gAlbedo"), 2);

    glBindVertexArray(m_fullscreenQuadVAO);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

#ifdef  _DEBUG
    glPopDebugGroup();
#endif 

    glBindVertexArray(0);
}
