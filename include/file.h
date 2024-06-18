#pragma once

#include <stdint.h>
#include <stdbool.h>

/*! @brief Reads file using UNIX specific functions
 * @param[in] const char* filename: Path to file to read
 * @param[in] char** buf: Buffer where read file will be stored
 * @return int32_t status: 0 is success otherwise is failure
 * @pointer_lifetime Mallocs memory for *buf and caller should free it themself
*/
__attribute__((warn_unused_result))
bool file_read(const char* filename, char** buf);
