#include "./utils/normalizedUvUnwrapping.hpp"
#include "renderer/renderer.hpp"
#include "glewGlfwHandlers/glewGlfwHandler.hpp"
#include "renderer/guiRendererConcreteMediator.hpp"

int main(int argc, char** argv) {
    GlewGlfwHandler glewGlfwHandler(glm::ivec2(1080, 720), "Mesh2Splat");
    if(glewGlfwHandler.init() == -1) return -1;

    ImGuiUI ImGuiUI(3, 0, 0.1f, 0.5f); //TODO: give a meaning to these params
    ImGuiUI.initialize(glewGlfwHandler.getWindow());
    Renderer renderer(glewGlfwHandler.getWindow());
    renderer.initialize();
    GuiRendererConcreteMediator guiRendererMediator(renderer, ImGuiUI);

    while (!glfwWindowShouldClose(glewGlfwHandler.getWindow())) {
        glfwPollEvents();
        renderer.updateTransformations();
        renderer.clearingPrePass(ImGuiUI.getSceneBackgroundColor());

        ImGuiUI.preframe();
        ImGuiUI.renderUI();
        
        guiRendererMediator.update();
        
        renderer.renderFrame();

        ImGuiUI.displayGaussianCount(renderer.getGaussianCountFromIndirectBuffer());
        ImGuiUI.postframe();

        glfwSwapBuffers(glewGlfwHandler.getWindow());
    }

    glfwTerminate();

    return 0;
}

