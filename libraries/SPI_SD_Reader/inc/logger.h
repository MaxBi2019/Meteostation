#include "terminal.h"
#include "sd.h"
#include "fatfs.h"
#include <stdlib.h>

#define HEADER        "Time,Pressure,Humidity,Temperature,CO2"
#define FILE_NAME     "logs.csv"

extern char USERPath[4]; /* logical drive path */

void Logger_Init();
int write_logs(char str[]);
