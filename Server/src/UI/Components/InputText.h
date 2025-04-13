#pragma once
#include "Component.h"
#include <functional>
#include <string>

namespace UI::Components
{

    template <typename T>
    T Clamp(const T &value, const T &min, const T &max)
    {
        if (value < min)
            return min;
        if (value > max)
            return max;
        return value;
    }

    class InputText : public Component
    {
    public:
        InputText(const char *label, std::string *value, ImGuiInputTextFlags flags = 0)
            : m_Label(label), m_StringValue(value), m_Flags(flags), m_Type(Type::String)
        {
        }

        InputText(const char *label, int *value, int min = 0, int max = 100, ImGuiInputTextFlags flags = 0)
            : m_Label(label), m_IntValue(value), m_MinValue(min), m_MaxValue(max), m_Flags(flags), m_Type(Type::Int)
        {
        }

        InputText(const char *label, char *value, size_t size, ImGuiInputTextFlags flags = 0)
            : m_Label(label), m_CharArrayValue(value), m_CharArraySize(size), m_Flags(flags), m_Type(Type::CharArray)
        {
        }

        ~InputText() override = default;

        void Render() override
        {
            UpdatePosition();

            bool enterPressed = false;

            switch (m_Type)
            {
            case Type::String:
            {
                char buffer[256] = "";
                strcpy_s(buffer, m_StringValue->c_str());
                enterPressed = ImGui::InputText(m_Label, buffer, sizeof(buffer), m_Flags);
                *m_StringValue = buffer;
                break;
            }
            case Type::Int:
            {
                char buffer[32] = "";
                sprintf_s(buffer, "%d", *m_IntValue);
                enterPressed = ImGui::InputText(m_Label, buffer, sizeof(buffer), m_Flags | ImGuiInputTextFlags_CharsDecimal);
                *m_IntValue = Clamp(std::atoi(buffer), m_MinValue, m_MaxValue);
                break;
            }
            case Type::CharArray:
            {
                enterPressed = ImGui::InputText(m_Label, m_CharArrayValue, m_CharArraySize, m_Flags);
                break;
            }
            }

            if (m_OnChangeCallback)
            {
                m_OnChangeCallback(GetValue());
            }

            if (enterPressed && (m_Flags & ImGuiInputTextFlags_EnterReturnsTrue) && m_OnEnterPressedCallback)
            {
                m_OnEnterPressedCallback();
            }
        }

        void SetOnChange(std::function<void(const std::string &)> callback)
        {
            m_OnChangeCallback = std::move(callback);
        }

        void SetOnEnterPressed(std::function<void()> callback)
        {
            m_OnEnterPressedCallback = std::move(callback);
        }

        std::string GetValue() const
        {
            switch (m_Type)
            {
            case Type::String:
                return *m_StringValue;
            case Type::Int:
                return std::to_string(*m_IntValue);
            case Type::CharArray:
                return std::string(m_CharArrayValue);
            default:
                return "";
            }
        }

    private:
        enum class Type
        {
            String,
            Int,
            CharArray
        };

        const char *m_Label;
        union
        {
            std::string *m_StringValue;
            int *m_IntValue;
            char *m_CharArrayValue;
        };
        int m_MinValue = 0;
        int m_MaxValue = 100;
        size_t m_CharArraySize = 0;
        ImGuiInputTextFlags m_Flags;
        Type m_Type;
        std::function<void(const std::string &)> m_OnChangeCallback;
        std::function<void()> m_OnEnterPressedCallback;
    };

}