#pragma once
#include "RenderPass.hpp"

class GaussianShadowPass : public IRenderPass {
public:
    GaussianShadowPass(RenderContext& renderContext);
    ~GaussianShadowPass() = default;
    void execute(RenderContext& renderContext);
    void drawToCubeMapFaces(RenderContext& renderContext);

private:
    GLuint m_shadowFBO = 0;
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;
    GLuint m_indirectDrawBuffer;
};