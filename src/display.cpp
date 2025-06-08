
#include <inttypes.h>
#include "display.h"


uint8_t balkenh = 50;
uint8_t balkenb = 5;

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

void initDisplay()
{
  u8g2.begin();
  uint8_t c = 0;
}



