#pragma once

#include "Drafter/Overlay/ImGuiOverlay.h"

#include <format>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace Drafter {

/**
 * @brief Overlay that displays a live data tree in an ImGui window.
 *
 * Build the tree structure once before the service loop by calling
 * CreateBranch() and Branch::AddLeaf().  Each leaf stores a callback
 * that is invoked every frame to fetch the current value.
 *
 * Derives from ImGuiOverlay, which manages the shared ImGui context and
 * frame lifecycle.  Multiple ImGuiOverlay-derived instances can coexist
 * without conflict.
 */
class DataTreeOverlay : public ImGuiOverlay {
  public:
    /**
     * @brief A collapsible node in the data tree.
     *
     * Created via DataTreeOverlay::CreateBranch().  Branches are owned by
     * the overlay; pointers returned by CreateBranch() remain stable.
     */
    class Branch {
      public:
        /**
         * @brief Add a leaf that displays a live value each frame.
         *
         * The callback is invoked once per frame.  If it returns
         * std::string it is used directly; any other type is formatted
         * via std::format.
         *
         * @param label    Display name shown left of the value.
         * @param value_fn Callable returning the current value.
         */
        template<typename Fn>
        void AddLeaf(std::string label, Fn value_fn) {
            using T = std::invoke_result_t<Fn>;
            m_leaves.push_back({std::move(label),
                [fn = std::move(value_fn)]() -> std::string {
                    if constexpr (std::is_same_v<T, std::string>)
                        return fn();
                    else
                        return std::format("{}", fn());
                }});
        }

      private:
        friend class DataTreeOverlay;

        struct Leaf {
            std::string                  label;
            std::function<std::string()> value_fn;
        };

        std::string            m_label;
        std::vector<Leaf>      m_leaves;
        std::vector<Branch *>  m_children;
    };

    /**
     * @brief Construct and attach to a canvas.
     *
     * @param canvas The canvas to attach to.
     * @param title  Title shown in the ImGui window title bar.
     */
    explicit DataTreeOverlay(Canvas &canvas, std::string title = "Data");

    /**
     * @brief Create a branch, optionally as a child of another branch.
     *
     * Passing nullptr (or omitting @p parent) creates a root branch.
     * The returned pointer is stable for the lifetime of the overlay.
     *
     * @param label  Display name for the collapsible section.
     * @param parent Parent branch, or nullptr for a root branch.
     * @return Non-owning pointer to the new branch.
     */
    Branch *CreateBranch(std::string label, Branch *parent = nullptr);

  protected:
    /** @brief Render the data tree ImGui window. */
    void RenderImGuiContent() override;

  private:
    /** @brief Recursively render a branch and its children. */
    void RenderBranch(Branch *branch);

    std::string                          m_title;
    std::vector<std::unique_ptr<Branch>> m_branches;
    std::vector<Branch *>                m_roots;
};

} // namespace Drafter
