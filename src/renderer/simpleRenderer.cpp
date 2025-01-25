class Renderer {
public:
    Renderer();
    ~Renderer();

    bool initialize();         // Initialize OpenGL state, resources, passes
    void renderFrame();        // Execute all enabled render passes
    void resize(int width, int height); // Handle window resizing, update viewport, projections
    void cleanup();            // Clean up resources

private:
    std::vector<std::unique_ptr<RenderPass>> renderPasses;
    // Possibly other members like shaders, buffers, scene data, etc.
};