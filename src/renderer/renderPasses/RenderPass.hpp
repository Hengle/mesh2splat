#pragma once
#include "../../utils/utils.hpp"
#include "../../utils/shaderUtils.hpp"

class RenderPass {
public:
    virtual ~RenderPass() = default;
    virtual void execute(...) = 0;
    virtual bool isEnabled() const { return isPassEnabled; }
    virtual void setIsEnabled(bool isPassEnabled) { this->isPassEnabled = isPassEnabled; };
    
    struct DrawArraysIndirectCommand {
        GLuint count;        // Number of vertices to draw.
        GLuint instanceCount;    // Number of instances.
        GLuint first;        // Starting index in the vertex buffer.
        GLuint baseInstance; // Base instance for instanced rendering.
    };

private:
    bool isPassEnabled;

};