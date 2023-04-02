#include "utils.h"

float float_abs(float n) { return n < 0 ? -n : n; }

bool is_touching(touchPosition *touch) { return touch->px != 0 || touch->py != 0; }