#pragma once

// QMI8658 1.x defines M_PI unconditionally. ESP-IDF 6's C library also
// provides it, so preserve the system definition around the upstream header.
// Keep the managed source untouched; remove when upstream guards this macro.
#include <math.h>
#pragma push_macro("M_PI")
#undef M_PI
#include_next "qmi8658.h"
#pragma pop_macro("M_PI")
