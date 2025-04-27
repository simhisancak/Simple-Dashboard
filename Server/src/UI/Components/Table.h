#pragma once
#include "Component.h"
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include "imgui.h"

namespace FracqServer {
namespace UI {
    namespace Components {

        template <typename T> class Table;

        struct ColumnDef {
            std::string name;
            float width;
            bool isStretch;

            ColumnDef(const std::string& _name, float _width = 0.0f, bool _isStretch = false)
                : name(_name)
                , width(_width)
                , isStretch(_isStretch) { }
        };

        template <typename T> using CellRenderer = std::function<void(const T&, int, int)>;

        template <typename T> class Table : public Component {
        public:
            Table()
                : m_ShowBorders(true)
                , m_ShowRowBackground(true)
                , m_EnableVerticalScrolling(true)
                , m_DefaultCellRenderer([](const T& item, int col, int row) { }) { }

            void Render() override {
                if (m_Columns.empty()) {
                    return;
                }

                UpdatePosition();

                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(6, 4));

                ImGuiTableFlags flags = 0;
                if (m_ShowBorders) {
                    flags |= ImGuiTableFlags_Borders;
                }
                if (m_ShowRowBackground) {
                    flags |= ImGuiTableFlags_RowBg;
                }
                if (m_EnableVerticalScrolling) {
                    flags |= ImGuiTableFlags_ScrollY;
                }

                flags |= ImGuiTableFlags_SizingFixedFit;
                flags |= ImGuiTableFlags_NoHostExtendX;

                ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0.12f, 0.20f, 0.35f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.17f, 0.27f, 0.44f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));

                if (ImGui::BeginTable("##DynamicTable",
                                      static_cast<int>(m_Columns.size()),
                                      flags,
                                      m_Size)) {
                    for (const auto& column : m_Columns) {
                        if (column.isStretch) {
                            ImGui::TableSetupColumn(column.name.c_str(),
                                                    ImGuiTableColumnFlags_WidthStretch);
                        } else {
                            ImGui::TableSetupColumn(column.name.c_str(),
                                                    ImGuiTableColumnFlags_WidthFixed,
                                                    column.width);
                        }
                    }

                    ImGui::TableHeadersRow();

                    for (size_t row = 0; row < m_Data.size(); row++) {
                        ImGui::TableNextRow();

                        for (size_t col = 0; col < m_Columns.size(); col++) {
                            ImGui::TableNextColumn();

                            if (col < m_CellRenderers.size() && m_CellRenderers[col]) {
                                m_CellRenderers[col](m_Data[row],
                                                     static_cast<int>(col),
                                                     static_cast<int>(row));
                            } else {
                                m_DefaultCellRenderer(m_Data[row],
                                                      static_cast<int>(col),
                                                      static_cast<int>(row));
                            }
                        }
                    }

                    ImGui::EndTable();
                }

                ImGui::PopStyleColor(4);
                ImGui::PopStyleVar();
            }

            void AddColumn(const std::string& name, float width = 0.0f, bool isStretch = false) {
                m_Columns.emplace_back(name, width, isStretch);
            }

            void SetData(const std::vector<T>& data) { m_Data = data; }

            void SetColumnRenderer(int columnIndex, CellRenderer<T> renderer) {
                if (columnIndex >= static_cast<int>(m_CellRenderers.size())) {
                    m_CellRenderers.resize(static_cast<size_t>(columnIndex) + 1);
                }
                m_CellRenderers[static_cast<size_t>(columnIndex)] = renderer;
            }

            void SetDefaultRenderer(CellRenderer<T> renderer) { m_DefaultCellRenderer = renderer; }

            void SetShowBorders(bool showBorders) { m_ShowBorders = showBorders; }

            void SetShowRowBackground(bool showRowBackground) {
                m_ShowRowBackground = showRowBackground;
            }

            void SetVerticalScrolling(bool enableScrolling) {
                m_EnableVerticalScrolling = enableScrolling;
            }

            void ClearColumns() {
                m_Columns.clear();
                m_CellRenderers.clear();
            }

            size_t GetColumnCount() const { return m_Columns.size(); }

            size_t GetRowCount() const { return m_Data.size(); }

        private:
            std::vector<ColumnDef> m_Columns;
            std::vector<T> m_Data;
            std::vector<CellRenderer<T>> m_CellRenderers;
            CellRenderer<T> m_DefaultCellRenderer;
            bool m_ShowBorders;
            bool m_ShowRowBackground;
            bool m_EnableVerticalScrolling;
        };

    } // namespace Components
} // namespace UI
} // namespace FracqServer
