#pragma once

#include "Content/Content.h"
#include <functional>
#include <vector>

namespace FracqServer {
namespace UI {
    namespace Views {

        class DashboardHeader {
        public:
            DashboardHeader();
            ~DashboardHeader() = default;

            void Render(const std::vector<std::unique_ptr<DashboardContent>>& contents,
                        const DashboardContent*& activeContent);

        private:
            void
            RenderNavigationButtons(const std::vector<std::unique_ptr<DashboardContent>>& contents,
                                    const DashboardContent*& activeContent);
        };
    } // namespace Views
} // namespace UI
} // namespace FracqServer
