#pragma once

typedef enum {
    ScrollDirectionNone,
    ScrollDirectionDown,
    ScrollDirectionUp,
} ScrollDirection;

void ask_for_scroll(AppDataPoint *data, ScrollDirection direction);
