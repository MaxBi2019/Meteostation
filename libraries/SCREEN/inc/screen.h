#ifndef _SCREEN_H_
#define _SCREEN_H_


#define data_screen_t "Intvl: %d(s)\r\n"\
                      "DP:%s\r\n"\
                      "DH:%s\r\n"\
                      "DT:%s\r\n"\
                      "DC:%s\r\n"
#define measurements_screen_t "\r\n%s"\
                              "%s"\
                              "%s"\
                              "%s\r\n"

#define time_screen_t "%s %s\r\n"


#define sd_info_t "\r\nSD: %s\r\n"

#define DATA_SCREEN        0
#define MEASUREMENT_SCREEN 1
#define TIME_SCREEN        2
#define SD_SCREEN          3

#define SCREEN_NUMBER      4


typedef struct
{
    const char *template;
    const int index;
} screen;

extern screen *sd_screen;
extern screen *time_screen;
extern screen *data_screen;
extern screen *measurements_screen;

#endif //_SCREEN_H_
