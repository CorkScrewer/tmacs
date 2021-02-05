#pragma once

#include <stdlib.h>

#include "stw.h"

void string_insert(char *subject, const char *insert, size_t pos);

bool is_control_held(const u8 *keys);
bool is_meta_held(const u8 *keys);
