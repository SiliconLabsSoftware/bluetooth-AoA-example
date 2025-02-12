#include <stdint.h>

#define SL_UNUSED_PARAMETER(param) (void) (param)
const unsigned int SL_MAX_UART_CONNECTIONS = 3;
uint8_t sl_uart_char;

typedef enum {
  SL_CARD_SPADE,
  SL_CARD_HEART,
  SL_CARD_CLUB,
  SL_CARD_DIAMOND
} sl_card_suit_t;

typedef struct {
  uint32_t hour;
  uint32_t minute;
  uint32_t second;
} sl_time_t;

sl_time_t sl_time;

void sl_do_something(uint8_t bar)
{
  // uint8_t arr[100]; //magic number
  uint8_t foo = 1;
  if (foo > bar) { // The preceding brace is *required*
    do_this();
  } else if (foo < bar) {
    do_that();
  } else {
    do_another_thing();
  }

  if (foo > bar) {
    do_this();
  }

  while (1) {
  }

  do {
    sli_do_work();
  } while (foo);

  switch (bar) {
    case 1:
      sli_do_this();
      break;

    case 2:
      sli_do_that();
      break;

    case 3:
    {
      sli_do_the_other_thing();
      break;
    }

    case 0:
    default:
      sli_do_what();
      break;
  }
}

void sl_do_something2(int a,
                      int b,
                      int c,
                      const char *string1,
                      const char *string2)
{
  // ...
}

void sli_do_something(int a, int b, int c)
{
  // ...
}
