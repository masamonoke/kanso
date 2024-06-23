#pragma once

#include "loaded_model.h"


__attribute__((nonnull(1, 2), warn_unused_result))
bool model_loader_load_model(loaded_model_t* model, const char* path);
