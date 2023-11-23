#include "connection.h"
#include <stdio.h>

static long id = 0;

connection new_connection() { return id++; };
