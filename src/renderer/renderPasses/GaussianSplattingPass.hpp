#pragma once
#include "RenderPass.hpp"

class GaussianSplattingPass : public IRenderPass {
public:
    GaussianSplattingPass(RenderContext& renderContext);
    ~GaussianSplattingPass() = default;
    void execute(RenderContext& renderContext);
private:
    GLuint quadVBO;
    GLuint quadEBO;
};