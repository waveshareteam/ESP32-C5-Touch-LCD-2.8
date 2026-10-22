#pragma once

#include "systems/phone/esp_brookesia_phone_app.hpp"

namespace esp_brookesia::apps {

class Settings: public systems::phone::App {
public:
    static Settings *requestInstance(bool use_status_bar = false, bool use_navigation_bar = false);
    ~Settings();

protected:
    Settings(bool use_status_bar, bool use_navigation_bar);

    bool run(void) override;
    bool back(void) override;
    bool close(void) override;
    bool init(void) override;
    bool deinit(void) override;
    bool pause(void) override;
    bool resume(void) override;

private:
    static Settings *_instance;
    static void closeFromUi(void);
};

} // namespace esp_brookesia::apps
