#pragma once
#include "Mediator.hpp"
#include "Renderer.hpp"
#include "../imGuiUi/ImGuiUi.hpp"

class GuiRendererConcreteMediator : public IMediator {
public:
    GuiRendererConcreteMediator(Renderer& renderer, ImGuiUI& imguiUI)
        : renderer(renderer), imguiUI(imguiUI) {}
    void update();

    void notify(EventType event);

private:
    Renderer& renderer;
    ImGuiUI& imguiUI;
};
