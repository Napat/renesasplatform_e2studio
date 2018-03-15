/* HAL-only entry function */
#include "hal_data.h"

extern void hal_application_code(void);

void hal_entry(void)
{
    /* TODO: add your own code here */
    hal_application_code();
}
