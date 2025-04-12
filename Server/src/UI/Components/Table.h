#pragma once
#include "Component.h"
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include "imgui.h"

namespace UI::Components {

// Forward declarations
template<typename T>
class Table;

// Column definition
struct ColumnDef {
    std::string name;
    float width;
    bool isStretch;
    
    ColumnDef(const std::string& _name, float _width = 0.0f, bool _isStretch = false) 
        : name(_name), width(_width), isStretch(_isStretch) {}
};

// Cell renderer type for custom cell rendering
template<typename T>
using CellRenderer = std::function<void(const T&, int, int)>;

// Generic table component that can work with any data type
template<typename T>
class Table : public Component {
public:
    // Constructor with default settings
    Table()
        : m_ShowBorders(true)
        , m_ShowRowBackground(true)
        , m_EnableVerticalScrolling(true)
        , m_DefaultCellRenderer([](const T& item, int col, int row) {})
    {
    }

    // Render the table
    void Render() override {
        if (m_Columns.empty()) {
            return;
        }
        
        UpdatePosition();
        
        // Apply custom styling for the table
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(6, 4));
        
        // Calculate table flags
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
        
        // Enhance style for better appearance
        ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0.12f, 0.20f, 0.35f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.17f, 0.27f, 0.44f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));
        
        // Begin the table
        if (ImGui::BeginTable("##DynamicTable", static_cast<int>(m_Columns.size()), flags, m_Size)) {
            // Setup columns
            for (const auto& column : m_Columns) {
                if (column.isStretch) {
                    ImGui::TableSetupColumn(column.name.c_str(), ImGuiTableColumnFlags_WidthStretch);
                } else {
                    ImGui::TableSetupColumn(column.name.c_str(), ImGuiTableColumnFlags_WidthFixed, column.width);
                }
            }
            
            // Draw header row
            ImGui::TableHeadersRow();
            
            // Draw data rows
            for (size_t row = 0; row < m_Data.size(); row++) {
                ImGui::TableNextRow();
                
                // Draw each column
                for (size_t col = 0; col < m_Columns.size(); col++) {
                    ImGui::TableNextColumn();
                    
                    // Check if there's a custom renderer for this column
                    if (col < m_CellRenderers.size() && m_CellRenderers[col]) {
                        m_CellRenderers[col](m_Data[row], static_cast<int>(col), static_cast<int>(row));
                    } else {
                        // Use default renderer
                        m_DefaultCellRenderer(m_Data[row], static_cast<int>(col), static_cast<int>(row));
                    }
                }
            }
            
            ImGui::EndTable();
        }
        
        // Restore styles
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
    }
    
    // Add a column to the table
    void AddColumn(const std::string& name, float width = 0.0f, bool isStretch = false) {
        m_Columns.emplace_back(name, width, isStretch);
    }
    
    // Set data for the table
    void SetData(const std::vector<T>& data) {
        m_Data = data;
    }
    
    // Set renderer for a specific column
    void SetColumnRenderer(int columnIndex, CellRenderer<T> renderer) {
        // Resize renderer array if needed
        if (columnIndex >= static_cast<int>(m_CellRenderers.size())) {
            m_CellRenderers.resize(static_cast<size_t>(columnIndex) + 1);
        }
        m_CellRenderers[static_cast<size_t>(columnIndex)] = renderer;
    }
    
    // Set default cell renderer
    void SetDefaultRenderer(CellRenderer<T> renderer) {
        m_DefaultCellRenderer = renderer;
    }
    
    // Set whether to show borders
    void SetShowBorders(bool showBorders) {
        m_ShowBorders = showBorders;
    }
    
    // Set whether to show row background
    void SetShowRowBackground(bool showRowBackground) {
        m_ShowRowBackground = showRowBackground;
    }
    
    // Set whether to enable vertical scrolling
    void SetVerticalScrolling(bool enableScrolling) {
        m_EnableVerticalScrolling = enableScrolling;
    }
    
    // Clear all columns
    void ClearColumns() {
        m_Columns.clear();
        m_CellRenderers.clear();
    }
    
    // Get number of columns
    size_t GetColumnCount() const {
        return m_Columns.size();
    }
    
    // Get number of rows
    size_t GetRowCount() const {
        return m_Data.size();
    }
    
private:
    std::vector<ColumnDef> m_Columns;
    std::vector<T> m_Data;
    std::vector<CellRenderer<T>> m_CellRenderers;
    CellRenderer<T> m_DefaultCellRenderer;
    bool m_ShowBorders;
    bool m_ShowRowBackground;
    bool m_EnableVerticalScrolling;
};

} // namespace UI::Components 