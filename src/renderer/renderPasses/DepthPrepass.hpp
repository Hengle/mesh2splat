#pragma once
#include "RenderPass.hpp"

class DepthPrepass : public IRenderPass {
public:
    ~DepthPrepass() = default;
    void execute(RenderContext& renderContext);
};