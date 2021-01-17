#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "screen.h"
#include "terminal.h"
#include "lcd5110.h"

#define DISPLAY_SIZE 200
#define BLOCK_SIZE (DISPLAY_SIZE+1)
#define EMPTY "NONE(%u)"


typedef struct
{
    void (*init)(int);
    void (*update_frame)();
    void (*set_frame)(int);
    void (*edit_frame)(screen*, ...);
    uint8_t display;
    char *buffer;
    size_t size;
} display_driver;



extern display_driver *display;
#endif //_DISPLAY_H_
