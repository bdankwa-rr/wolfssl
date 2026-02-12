/*
 *   Dummy C code to get startup code to link without Ada sources.
 *
 *   �2009 Aero Engine Controls
 *
 */

#include "startup.h"
#include <stdint.h>
#include <stdio.h>

void _interrupt_handler (void) __attribute__ ((interrupt));
extern void _wdt_service();

uint32_t *LED_PTR = (uint32_t *)0x20000078;

void _illint (void)
{
}

/* Access to the WDT data */
extern volatile struct wdt_buffer_fmt _wdt_data;

static uint32_t counter = 0;
static uint32_t phase = 0;
static uint32_t firstPass = 1;
static uint32_t counterHeartbeat = 0;

void _interrupt_handler (void)
{
    if (firstPass == 1) {
			*LED_PTR = 0;
			firstPass = 0;
		}

		counter++;
		
		if (counter >= 500 && phase == 0) {
			*LED_PTR = !(*LED_PTR);
			phase = 1;
			counter = 0;
			counterHeartbeat++;
			//printf("Heartbeat %08X\r\n", counterHeartbeat);
		}

		if (counter >= 50 && phase < 4 && phase > 0) {
			*LED_PTR = !(*LED_PTR);
			counter = 0;
			phase++;
		}

		if (phase >= 4) {
			phase = 0;
		}

	  /* Point to the WDT data */
	  volatile struct wdt_buffer_fmt *wdt_bufptr = &_wdt_data;
	  /* Hardware addresses */
	  volatile struct int_reg_fmt *const interrupt_regs = (struct int_reg_fmt *) INTERRUPT_REGS;

	  /* SRC interrupt? */
	  if (interrupt_regs->source & INTERRUPT_MASK_SRC)
	    {
	      _wdt_service ();
	    }
	  /* Not an SRC interrupt - assume it's an illegal one */
	  else
	    _illint ();

}
