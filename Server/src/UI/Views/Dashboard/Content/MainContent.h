#pragma once

#include "FarmBot/FarmBot.h"
#include "SaveLoad/SaveLoad.h"
#include <memory>
#include <vector>

namespace UI
{
    namespace Views
    {

        class DashboardMainContent
        {
        public:
            DashboardMainContent(Application *app);
            ~DashboardMainContent() = default;

            void Render(const DashboardContent *activeContent, int selectedPid);

            const std::vector<std::unique_ptr<DashboardContent>> &GetContents() const { return m_Contents; }

        private:
            std::vector<std::unique_ptr<DashboardContent>> m_Contents;
        };
    }
}