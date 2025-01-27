#pragma once
#include "RenderPass.hpp"
#include "../../radixSort/RadixSort.hpp"

class RadixSortPass : public IRenderPass {
public:
    ~RadixSortPass() = default;
    void execute(RenderContext& renderContext);

private:
    
    unsigned int computeKeyValuesPre(RenderContext& renderContext);
    
    void sort(RenderContext& renderContext, unsigned int validCount);
    
    void gatherPost(RenderContext& renderContext, unsigned int validCount);
};