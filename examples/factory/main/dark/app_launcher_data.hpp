/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "systems/phone/widgets/app_launcher/esp_brookesia_app_launcher.hpp"

namespace esp_brookesia::systems::phone {

// 【修改1】重命名为240x320，样式参数完全不变
constexpr AppLauncherIcon::Data STYLESHEET_240_320_DARK_APP_LAUNCHER_ICON_DATA = {
    .main = {
        .size = gui::StyleSize::SQUARE(90),
        .layout_row_pad = 10,
    },
    .image = {
        .default_size = gui::StyleSize::SQUARE(60),
        .press_size = gui::StyleSize::SQUARE(50),
    },
    .label = {
        .text_font = gui::StyleFont::SIZE(16),
        .text_color = gui::StyleColor::COLOR(0xFFFFFF),
    },
};

// 【修改2】重命名为240x320，核心修改一页图标数量为4
constexpr AppLauncherData STYLESHEET_240_320_DARK_APP_LAUNCHER_DATA = {
    .main = {
        .y_start = 0,
        .size = gui::StyleSize::RECT_PERCENT(100, 100),
    },
    .table = {
        .default_num = 4,  // 【核心修改】一页4个APP图标（原2→4）
        .size = gui::StyleSize::RECT_PERCENT(80, 70),
    },
    .indicator = {
        .main_size = gui::StyleSize::RECT_W_PERCENT(100, 20),
        .main_layout_column_pad = 30,
        .main_layout_bottom_offset = 30,
        .spot_inactive_size = gui::StyleSize::SQUARE(12),
        .spot_active_size = gui::StyleSize::RECT(40, 12),
        .spot_inactive_background_color = gui::StyleColor::COLOR(0xC6C6C6),
        .spot_active_background_color = gui::StyleColor::COLOR(0xFFFFFF),
    },
    .icon = STYLESHEET_240_320_DARK_APP_LAUNCHER_ICON_DATA,
    .flags = {
        .enable_table_scroll_anim = 0,
    },
};

} // namespace esp_brookesia::systems::phone