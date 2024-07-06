//
// Created by Simon Weckler on 06.07.24.
//

#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include "types.h"
#include <stdlib.h>

struct Request *parseCSVFile(const char *filename, size_t *requestCount);


#endif
