## Description

This application cycles through the various low power modes. Switching between the different modes can be triggered by either pressing a button (PB1/SW4) or automatically with an RTC alarm. This is configured with the define statements at the top of the application.

## Required Connections:

-   Connect a USB cable between the PC and the CN2 (USB/PWR) connector.
-   Connect the 5V power cable at (5V IN).
-   Close jumper (RX - P0.0) and (TX - P0.1) at Headers JP23 (UART 0 EN).
-   Open a terminal application on the PC and connect to the EV kit's console UART at 115200, 8-N-1.

## Expected Output

```
****Low Power Mode Example****

This code cycles through the MAX78000 power modes, using the RTC alarm to exit from each mode.  The modes will change every 2 seconds.

Running in ACTIVE mode.
All unused RAMs shutdown.
Entering SLEEP mode.
Waking up from SLEEP mode.
Entering DEEPSLEEP mode.
Waking up from DEEPSLEEP mode.
Entering BACKUP mode.
****Low Power Mode Example****

```

