#include "screen.h"



static screen dt_screen =
{
        .template = data_screen_t,
        .index = DATA_SCREEN
};

static screen mm_screen =
{
        .template = measurements_screen_t,
        .index = MEASUREMENT_SCREEN
};

static screen tm_screen =
{
        .template = time_screen_t,
        .index = TIME_SCREEN
};


static screen sdinfo_screen =
{
        .template = sd_info_t,
        .index = SD_SCREEN
};

screen *sd_screen = &sdinfo_screen;
screen *time_screen = &tm_screen;
screen *measurements_screen = &mm_screen;
screen *data_screen = &dt_screen;
