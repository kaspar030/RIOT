#include <stdio.h>

#include "driverlib/setup.h"
#include "driverlib/vims.h"

void cc26xx_init(void)
{
    SetupTrimDevice();

    VIMSModeSet(VIMS_BASE, VIMS_MODE_ENABLED);
    VIMSConfigure(VIMS_BASE, true, true);

}
