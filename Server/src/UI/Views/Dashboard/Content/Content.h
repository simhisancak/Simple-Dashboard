#pragma once

#include "Application.h"
#include <memory>
#include <string>

namespace FracqServer {
namespace UI {
    namespace Views {

        class DashboardContent {
        public:
            DashboardContent(Application* app)
                : m_App(app) { }
            virtual ~DashboardContent() = default;

            virtual void Render(int selectedPid = -1) = 0;

            virtual const char* GetTitle() const = 0;

            virtual const char* GetContentId() const = 0;

            bool IsSameContent(const DashboardContent* other) const {
                if (!other)
                    return false;
                return strcmp(GetContentId(), other->GetContentId()) == 0;
            }

        protected:
            Application* m_App;
        };
    } // namespace Views
} // namespace UI
} // namespace FracqServer
