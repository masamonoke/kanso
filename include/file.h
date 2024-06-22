#pragma once

#include <stdint.h>
#include <stdbool.h>

__attribute__((nonnull(1, 2), warn_unused_result))
bool file_read(const char* filename, char** buf);
