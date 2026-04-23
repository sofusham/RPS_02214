#include <cstdarg>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "driver/usb_serial_jtag.h"


#include "serial.h"


void serial_printf(const char* format, ...)
{
    char buffer[256];  // adjust size if needed

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    usb_serial_jtag_write_bytes(buffer, strlen(buffer), portMAX_DELAY);
}