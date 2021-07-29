#ifndef CALLS_H
#define CALLS_H

#include <stddef.h>

#define CALLTABLE ((void **) 0x00200000)

#define uart_init() ((void (*)()) CALLTABLE[0x0000])()

#define uart_lcr() ((unsigned int (*)()) CALLTABLE[0x0001])()

#define uart_flush() ((void (*)()) CALLTABLE[0x0002])()

#define uart_send(a) ((void (*)(unsigned int)) CALLTABLE[0x0003])(a)

#define uart_recv() ((unsigned int (*)()) CALLTABLE[0x0004])()

#define uart_check() ((unsigned int (*)()) CALLTABLE[0x0005])()

#define uart_overrun() ((unsigned int (*)()) CALLTABLE[0x0006])()

#define timer_init() ((void (*)()) CALLTABLE[0x0007])()

#define timer_tick() ((unsigned int (*)()) CALLTABLE[0x0008])()

#define leds_off() ((void (*)()) CALLTABLE[0x0009])()

#define malloc(size) ((void *(*)(size_t)) CALLTABLE[0x000a])(size)

#define free(p) ((void (*)(void *)) CALLTABLE[0x000b])(p)

#define util_sendstr(s) ((void (*)()) CALLTABLE[0x1000])(s)

#define util_uint2hexstr(c, s) ((unsigned int (*)()) CALLTABLE[0x1001])((c), (s))

#define util_uint2str(c, s) ((unsigned int (*)()) CALLTABLE[0x1002])((c), (s))

#define util_sendhexint(u) ((void (*)()) CALLTABLE[0x1003])(u)

#define util_senduint(u) ((void (*)()) CALLTABLE[0x1004])(u)

#define util_sendint(i) ((void (*)()) CALLTABLE[0x1005])(i)

#endif
