#include "imgui.h"
#ifndef IMGUI_DISABLE
#include "imgui_internal.h"

// System includes
#include <stdint.h>     // intptr_t
#endif

#include <stdio.h>      // FILE*, sscanf
#include <stdlib.h>     // NULL, malloc, free, qsort, atoi, atof
#include <math.h>       // sqrtf, fabsf, fmodf, powf, floorf, ceilf, cosf, sinf
#include <limits.h>     // INT_MIN, INT_MAX

namespace ImGuiCustom
{
    void Theme_EnemyMouse();
    void Theme_Cinder();

    bool ToggleButton(const char* str_id, bool* v, ImVec4 bg_color = { 255, 0, 255, 255 }, bool anim = true);
}
