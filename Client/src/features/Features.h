#pragma once
#include "common/Logger.h"
#include "PacketTypes.h"

class ClientApp;

class Features {
public:
    template<typename T>
    static void Initialize(ClientApp* app, int interval = 0) {
        s_App = app;
        if (s_App && interval > 0) {
            static T feature;  // Create a single static instance
            s_App->GetTaskManager()->ScheduleRecurringTask([&]() {
                feature.Loop();
            }, std::chrono::milliseconds(interval));
        }
    }

    virtual void Loop() = 0;

protected:
    static ClientApp* s_App;
};