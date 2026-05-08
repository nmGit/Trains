#include "Drafter/Overlay/DataTreeOverlay.h"

#include <imgui.h>

namespace Drafter {

DataTreeOverlay::DataTreeOverlay(Canvas &canvas, std::string title)
    : ImGuiOverlay(canvas), m_title(std::move(title)) {}

DataTreeOverlay::Branch *DataTreeOverlay::CreateBranch(std::string  label,
                                                       Branch      *parent) {
    auto    branch  = std::make_unique<Branch>();
    branch->m_label = std::move(label);
    Branch *ptr     = branch.get();
    m_branches.push_back(std::move(branch));

    if (parent)
        parent->m_children.push_back(ptr);
    else
        m_roots.push_back(ptr);

    return ptr;
}

void DataTreeOverlay::RenderImGuiContent() {
    ImGui::SetNextWindowPos(ImVec2(10.f, 10.f), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(250.f, 300.f), ImGuiCond_Once);
    ImGui::Begin(m_title.c_str());

    for (auto *root : m_roots)
        RenderBranch(root);

    ImGui::End();
}

void DataTreeOverlay::RenderBranch(Branch *branch) {
    if (!ImGui::TreeNode(branch->m_label.c_str()))
        return;

    for (const auto &leaf : branch->m_leaves)
        ImGui::Text("%s: %s", leaf.label.c_str(), leaf.value_fn().c_str());

    for (auto *child : branch->m_children)
        RenderBranch(child);

    ImGui::TreePop();
}

} // namespace Drafter
