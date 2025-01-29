#pragma once
#include "RenderPass.hpp"

class GaussiansPrepass : public IRenderPass {
public:
    ~GaussiansPrepass() = default;
    void execute(RenderContext& renderContext);
};