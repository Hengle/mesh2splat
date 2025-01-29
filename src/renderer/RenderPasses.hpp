#pragma once

//Base 
#include "renderPasses/RenderPass.hpp"

//Render Passes
#include "renderPasses/ConversionPass.hpp"
#include "renderPasses/GaussiansPrepass.hpp"
#include "renderPasses/RadixSortPass.hpp"
#include "renderPasses/GaussianSplattingPass.hpp"

static std::string conversionPassName			= "conversion";
static std::string gaussiansPrePassName			= "gaussianPrepass";
static std::string radixSortPassName			= "radixSort";
static std::string gaussianSplattingPassName	= "gaussianSplatting";