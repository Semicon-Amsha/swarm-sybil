#include <stdbool.h>
#ifndef STATUS_LED_H_
#define STATUS_LED_H_
void status_init(void);
void status_rgb(bool r, bool g, bool b);
#endif