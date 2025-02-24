#pragma once
#include "RenderPass.hpp"

class GaussianRelightingPass : public IRenderPass {
public:
    GaussianRelightingPass();
    ~GaussianRelightingPass() = default;
    void execute(RenderContext& renderContext);
private:
    GLuint m_fullscreenQuadVAO = 0;
    GLuint m_fullscreenQuadVBO = 0;
    GLuint m_fullscreenQuadEBO = 0;

};