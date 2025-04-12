#pragma once

#include "Application.h"
#include <memory>
#include <string>

namespace UI {
namespace Views {

class DashboardContent {
public:
    DashboardContent(Application* app) : m_App(app) {}
    virtual ~DashboardContent() = default;

    // Her content implementasyonu bu fonksiyonu override etmeli
    virtual void Render(int selectedPid = -1) = 0;

    // Content'in başlığını döndürür (header'da gösterilecek)
    virtual const char* GetTitle() const = 0;

    // Content'in benzersiz ID'sini döndürür
    virtual const char* GetContentId() const = 0;

    // İki content'in aynı olup olmadığını kontrol eder
    bool IsSameContent(const DashboardContent* other) const {
        if (!other) return false;
        return strcmp(GetContentId(), other->GetContentId()) == 0;
    }

protected:
    Application* m_App;
};
}
} 