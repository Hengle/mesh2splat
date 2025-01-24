#include "renderer.hpp"

//TODO: create a separete camera class, avoid it bloating and getting too messy

Renderer::Renderer()
{
    glGenVertexArrays(1, &pointsVAO);
    converterShaderProgram   = createConverterShaderProgram();
    renderShaderProgram      = createRendererShaderProgram(); 
    computeShaderProgram     = createComputeShaderProgram(); 
    gaussianBuffer           = -1;
    drawIndirectBuffer       = -1;
    normalizedUvSpaceWidth   = 0;
    normalizedUvSpaceHeight  = 0;

    lastShaderCheckTime     = glfwGetTime();
    initializeShaderFileMonitoring(shaderFiles, converterShadersInfo, computeShadersInfo, rendering3dgsShadersInfo);
}

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &pointsVAO);
    glDeleteBuffers(1, &gaussianBuffer);
    glDeleteBuffers(1, &drawIndirectBuffer);
    glDeleteProgram(renderShaderProgram);
    glDeleteProgram(converterShaderProgram);
    glDeleteProgram(computeShaderProgram);
}

void Renderer::initializeOpenGLState() {
    //TODO: Better have a way to set these opengl states at the render pass level in next refactor
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);

    
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendEquation(GL_FUNC_ADD);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);

    //glEnable(GL_MULTISAMPLE);
}

//TODO: kinda bad that I pass parameters to this function that are global variables (in )
glm::vec3 Renderer::computeCameraPosition(float yaw, float pitch, float distance) {
    // Convert angles to radians
    float yawRadians   = glm::radians(yaw);
    float pitchRadians = glm::radians(pitch);

    // Calculate the new camera position in Cartesian coordinates
    float x = distance * cos(pitchRadians) * cos(yawRadians);
    float y = distance * sin(pitchRadians);
    float z = distance * cos(pitchRadians) * sin(yawRadians);
    
    return glm::vec3(x, y, z);
}

unsigned int Renderer::getSplatBufferCount(GLuint counterBuffer)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer);
    unsigned int splatCount = 1000;
    unsigned int* counterPtr = (unsigned int*) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int), GL_MAP_READ_BIT);
    if (counterPtr) {
        splatCount = *counterPtr;
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }
    return splatCount;
}
/*
void debugPrintGaussians(GLuint gaussianBuffer, unsigned int maxPrintCount = 50)
{
    // 1. Bind the buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, gaussianBuffer);

    // 2. Determine how many bytes it holds
    GLint bufferSize = 0;
    glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &bufferSize);

    // 3. Map the buffer range for reading
    //    (You could also use GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT, etc. 
    //     but here we’ll keep it simple.)
    const GaussianDataSSBO* mapped = static_cast<const GaussianDataSSBO*>(
        glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, bufferSize, GL_MAP_READ_BIT)
    );
    if (!mapped) {
        std::cerr << "Failed to map Gaussian SSBO for reading.\n";
        return;
    }

    // 4. Calculate how many Gaussians are actually in this buffer
    size_t gaussianCount = bufferSize / sizeof(GaussianDataSSBO);

    // 5. Print some or all of them
    size_t printCount = std::min<size_t>(gaussianCount, maxPrintCount);
    std::cout << "GaussianBuffer has " << gaussianCount << " entries. Printing first "
              << printCount << ":\n";
    
    for (size_t i = 0; i < printCount; ++i) {
        const GaussianDataSSBO& g = mapped[i];
        std::cout << "Index " << i << ": "
                  << "pos(" << g.position.x << ", " << g.position.y << ", " << g.position.z << ", " << g.position.w << "), "
                  << "col(" << g.color.x << ", " << g.color.y << ", " << g.color.z << ", " << g.color.w << "), "
                  << "scale(" << g.scale.x << ", " << g.scale.y << ", " << g.scale.z << ", " << g.scale.w << ")\n"
                  << "normal(" << g.normal.x << ", " << g.normal.y << ", " << g.normal.z << ", " << g.normal.w << "), "
                  << "rotation(" << g.rotation.x << ", " << g.rotation.y << ", " << g.rotation.z << ", " << g.rotation.w << "), "
                  << "pbr(" << g.pbr.x << ", " << g.pbr.y << ", " << g.pbr.z << ", " << g.pbr.w << ")\n";
    }

    // 6. Unmap the buffer when done
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}*/

void Renderer::run3dgsRenderingPass(GLFWwindow* window, GLuint pointsVAO, GLuint gaussianBuffer, GLuint drawIndirectBuffer, GLuint renderShaderProgram, float std_gauss, int resolutionTarget)
{
    //TODO: take inspo for StopThePop: https://arxiv.org/pdf/2402.00525
    //When sorting consider that a global sort based on mean depth in view space its consistent during translation
    if (gaussianBuffer == static_cast<GLuint>(-1) ||
        drawIndirectBuffer == static_cast<GLuint>(-1) ||
        pointsVAO == 0 ||
        renderShaderProgram == 0) return;

    glUseProgram(renderShaderProgram);

    //TODO: this will work once sorting is working
	glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    //The correct one, from slide deck of Bernard K.
	glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    float fov = 90.0f;
    float closePlane = 0.01f;
    float farPlane = 100.0f;
    glm::mat4 projection = glm::perspective(glm::radians(fov),
                                            (float)width / (float)height,
                                            closePlane, farPlane);

    glViewport(0, 0, width, height);

    //TODO: refactor, should not be here. Create a separate transformations/camera class
    glm::vec3 cameraPos = computeCameraPosition(yaw, pitch, distance);
    glm::vec3 target(0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(cameraPos, target, up);
    glm::mat4 model = glm::mat4(0.9f);
    glm::mat4 MVP = projection * view * model;

    float htany = tan(glm::radians(fov) / 2);
    float htanx = htany / height * width;
    float focal_z = height / (2 * htany);
    glm::vec3 hfov_focal = glm::vec3(htanx, htany, focal_z);

    //Probably better with indexing, may save some performance
    std::vector<float> quadVertices = {
        // Tr1
        -1.0f, -1.0f,   0.0f,
         -1.0f, 1.0f,   0.0f,
         1.0f,  1.0f,   0.0f,
        // Tr2 
         -1.0f,  -1.0f, 0.0f,
        1.0f,   1.0f,   0.0f,
        1.0f,   -1.0f,  0.0f,
    };

    glBindVertexArray(pointsVAO);

    GLuint quadVBO;
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), quadVertices.data(), GL_DYNAMIC_DRAW);

    //TODO: should name all uniforms with this convention for clarity
    setUniformMat4(renderShaderProgram, "u_MVP", MVP);
    setUniformMat4(renderShaderProgram, "u_worldToView", view);
    setUniformMat4(renderShaderProgram, "u_objectToWorld", model);
    setUniformMat4(renderShaderProgram, "u_viewToClip", projection);
    setUniform2f(renderShaderProgram,   "u_resolution", glm::ivec2(width, height));
    setUniform3f(renderShaderProgram,   "u_hfov_focal", hfov_focal);
    setUniform1f(renderShaderProgram,   "u_std_dev", std_gauss / (float(resolutionTarget)));


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(0);

    glFinish();
    glBindBuffer(GL_ARRAY_BUFFER, gaussianBuffer);
    
    GLint bufferSize = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawIndirectBuffer);
    //TODO: should not need to re-define this buffer each time lol
    struct DrawArraysIndirectCommand {
        GLuint count;        // Number of vertices to draw.
        GLuint instanceCount;    // Number of instances.
        GLuint first;        // Starting index in the vertex buffer.
        GLuint baseInstance; // Base instance for instanced rendering.
    };
    DrawArraysIndirectCommand* cmd = (DrawArraysIndirectCommand*)glMapBufferRange(
        GL_DRAW_INDIRECT_BUFFER, 0, sizeof(DrawArraysIndirectCommand), GL_MAP_READ_BIT
    );
    unsigned int validCount = cmd->instanceCount;
    glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);
    std::vector<GaussianDataSSBO> gaussians(validCount);
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, validCount * sizeof(GaussianDataSSBO), gaussians.data());

    //TODO: JUST MISSING RADIX SORT COMPUTE PASSES
    // Transform Gaussian positions to view space and apply global sort
    auto viewSpaceDepth = [&](const GaussianDataSSBO& g) -> float {
        glm::vec4 viewPos = view * glm::vec4(g.position.x,g.position.y, g.position.z, 1.0);
        return viewPos.z; 
    };

    std::sort(gaussians.begin(), gaussians.end(),
              [&](const GaussianDataSSBO& a, const GaussianDataSSBO& b) {
                  return viewSpaceDepth(a) > viewSpaceDepth(b);
              });
    
    //glBindBuffer(GL_ARRAY_BUFFER, gaussianBuffer);
    glBufferData(GL_ARRAY_BUFFER, 
             gaussians.size() * sizeof(GaussianDataSSBO), 
             gaussians.data(), 
             GL_DYNAMIC_DRAW);


    //We need to redo this vertex attrib binding as the buffer could have been deleted if the compute/conversion pass was run, and we need to free the data to avoid
    // memory leak. Should structure renderer architecture
    unsigned int stride = sizeof(glm::vec4) * 6; //TODO: ISSUE6 (check for it)
    
    for (int i = 1; i <= 6; ++i) {
        glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec4) * (i - 1)));
        glEnableVertexAttribArray(i);
        glVertexAttribDivisor(i, 1);
    }

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawIndirectBuffer);
    
    glDrawArraysIndirect(GL_TRIANGLES, 0); //instance parameters set in framBufferReaderCS.glsl

    glBindVertexArray(0);
    glDisable(GL_BLEND);
}
	
void Renderer::clearingPrePass(glm::vec4 clearColor)
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 0); //Important for correct blending
    glClear(GL_COLOR_BUFFER_BIT);
}

bool Renderer::updateShadersIfNeeded() {
    for (auto& entry : shaderFiles) {
        ShaderFileInfo& info = entry.second;
        if (shaderFileChanged(info)) {
            // Update timestamp
            info.lastWriteTime = fs::last_write_time(info.filePath);

            this->renderShaderProgram   = reloadShaderProgram(converterShadersInfo,     this->renderShaderProgram);
            this->computeShaderProgram  = reloadShaderProgram(computeShadersInfo,       this->computeShaderProgram);
            this->renderShaderProgram   = reloadShaderProgram(rendering3dgsShadersInfo, this->renderShaderProgram);

            return true; //TODO: ideally it should just reload the programs for which that shader is included, need dependency for that
        }
    }
    return false;
}

//TODO: implement shader hot-reloading
void Renderer::renderLoop(GLFWwindow* window, ImGuiUI& gui)
{
    //Yeah not greates setup, the logic itself should not probably reside in the gui, but good enough like this.
    //Should implement the concept of a render pass rather than having a specialized class handle one type of pass
    //TODO: make render passes and logic handling more modular, this is very fixed and wobbly

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        clearingPrePass(gui.getSceneBackgroundColor());

        gui.preframe();
        gui.renderUI();

        double currentTime = glfwGetTime();
        if (currentTime - lastShaderCheckTime > 1.0) {
            gui.setRunConversion(updateShadersIfNeeded());
            lastShaderCheckTime = currentTime;
        }

        {
            if (gui.shouldLoadNewMesh()) {
                mesh2SplatConversionHandler.prepareMeshAndUploadToGPU(gui.getFilePath(), gui.getFilePathParentFolder(), dataMeshAndGlMesh, normalizedUvSpaceWidth, normalizedUvSpaceHeight);
                for (auto& mesh : dataMeshAndGlMesh)
                {
                    Mesh meshData = mesh.first;
                    GLMesh meshGl = mesh.second;
                    printf("Loading textures into utility std::map\n");
                    loadAllTextureMapImagesIntoMap(meshData.material, textureTypeMap);
                    generateTextures(meshData.material, textureTypeMap);
                }
                std::string meshFilePath(gui.getFilePath());
                if (!meshFilePath.empty()) {
                    mesh2SplatConversionHandler.runConversionPass(
                        meshFilePath, gui.getFilePathParentFolder(),
                        gui.getResolutionTarget(), gui.getGaussianStd(),
                        converterShaderProgram, computeShaderProgram,
                        dataMeshAndGlMesh, normalizedUvSpaceWidth, normalizedUvSpaceHeight,
                        textureTypeMap, gaussianBuffer, drawIndirectBuffer
                    );
                }

                gui.setLoadNewMesh(false);
            }


            if (gui.shouldRunConversion()) {
                std::string meshFilePath(gui.getFilePath());
                if (!meshFilePath.empty()) {
                    //Entry point for conversion code
                    mesh2SplatConversionHandler.runConversionPass(
                        meshFilePath, gui.getFilePathParentFolder(),
                        gui.getResolutionTarget(), gui.getGaussianStd(),
                        converterShaderProgram, computeShaderProgram,
                        dataMeshAndGlMesh, normalizedUvSpaceWidth, normalizedUvSpaceHeight,
                        textureTypeMap, gaussianBuffer, drawIndirectBuffer
                    );
                }
                gui.setRunConversion(false);
            }

            if (gui.shouldSavePly() && !gui.getFilePathParentFolder().empty())
            {
                GaussianDataSSBO* gaussianData = nullptr;
                unsigned int gaussianCount;
                read3dgsDataFromSsboBuffer(drawIndirectBuffer, gaussianBuffer, gaussianData, gaussianCount);
                if (gaussianData) {
                    writeBinaryPlyStandardFormatFromSSBO(gui.getFullFilePathDestination(), gaussianData, gaussianCount);
                }
                gui.setShouldSavePly(false);
            }

            //TODO: should have per render passes class in renderer and a scene object + a vector of optional parameters and a blackboard
            run3dgsRenderingPass(window, pointsVAO, gaussianBuffer, drawIndirectBuffer, renderShaderProgram, gui.getGaussianStd(), gui.getResolutionTarget());
        }

        gui.postframe();

        glfwSwapBuffers(window);
    }

    glfwTerminate();
}