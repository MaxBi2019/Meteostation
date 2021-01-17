#include "display.h"



static LCD5110_display lcd1;

static void
init_display(int size)
{
  char str[sizeof(EMPTY)];
  display->buffer = calloc(BLOCK_SIZE, size);
  display->display = 0;
  for (size_t ind=0; ind < size; ++ind)
      memmove(display->buffer+ind*BLOCK_SIZE, str, sprintf(str, EMPTY, ind));
  display->size = size;
  lcd1.hw_conf.spi_handle = &hspi1;
  lcd1.hw_conf.spi_cs_pin =  LCD1_CS_Pin;
  lcd1.hw_conf.spi_cs_port = LCD1_CS_GPIO_Port;
  lcd1.hw_conf.rst_pin =  LCD1_RST_Pin;
  lcd1.hw_conf.rst_port = LCD1_RST_GPIO_Port;
  lcd1.hw_conf.dc_pin =  LCD1_DC_Pin;
  lcd1.hw_conf.dc_port = LCD1_DC_GPIO_Port;
  lcd1.def_scr = lcd5110_def_scr;
  LCD5110_init(&lcd1.hw_conf, LCD5110_NORMAL_MODE, 0x40, 2, 3);
  print("Initialized\n");
}

static char*
get_frame(int index)
{
  return display->buffer+index*BLOCK_SIZE;
}

static void
set_frame(int index)
{
  display->display = index;
}

static void
update_frame()
{

	LCD5110_clear_scr(&lcd1);
	LCD5110_print(get_frame(display->display), BLACK, &lcd1);
	LCD5110_refresh(&lcd1);
	//print(get_frame(display->display));

}

static void
edit_frame(screen* screen_ptr, ...)
{
  va_list arguments;
  va_start(arguments, screen_ptr);
  vsprintf(get_frame(screen_ptr->index), screen_ptr->template, arguments);
}

static display_driver dsp_driver = {
    .init = init_display,
    .update_frame = update_frame,
	.set_frame = set_frame,
    .edit_frame = edit_frame,
	.display = TIME_SCREEN
};



display_driver *display = &dsp_driver;
