/*
 *   C Start-up code.
 *
 *   �2007 Goodrich Control Systems Limited. Copyright in this document belongs to
 *   Goodrich Control Systems Limited and all rights are reserved. No reproduction
 *   of all or part of this document shall be made without the prior written consent
 *   of Goodrich Control Systems Limited. This document contains information that
 *   may be confidential and its disclosure to others requires the written consent
 *   of Goodrich Control Systems Limited.
 *
 *   Build Version       Author          Date                    Change Reference
 *   1.4                 D. Hawthorn     06-Aug-2007             1. Changed include declaration of startup.h.
 *   1.3                 M.E.Bradbury    19-Apr-2007             1. Wait for m_servesr loop corrected ('!' inserted).
 *                                                                  (detected during SPIN analysis by PG).
 *   1.2                 M.E.Bradbury    27-Mar-2007             1. WDT setup moved to separate routine.
 *                                                               2. Call to '_boot_functions' moved to preamble.
 *                                                               3. Procedure '_background' changed to '_user_entry'.
 *                                                               4. Various other tidy-ups.
 *                                                                  Ref: Review Report TBD
 *   1.1                 M.E.Bradbury    04-Oct-2006             1. WDT and assembler enumerated types and
 *                                                                  object references moved to header file.
 *   1.0                 M.E.Bradbury    18-Jul-2006             1. Original
 *
 *
 * CM INFORMATION:
 *   CM FILE NAME:-               %name: startup.c %
 *   CM FILE VERSION:-            %version: 3 %
 *   CM FILE INSTANCE:-           %instance: 2 %
 *   CM ORIGINAL CHECK-IN DATE:-  %date_created: Mon Aug 06 15:39:43 2007 %
 *   CM ORIGINAL CHECK-IN BY:-    %created_by: asc %
 *   CM MODIFIED CHECK-IN DATE:-  %date_modified: %
 *   CM MODIFIED CHECK-IN BY:-    %derived_by: asc %
 */

/*
 *   NB Since this routine is to be used prior to the elaboration process, care
 *   must be taken to ensure no declarations use the .bss and .data areas.
 *
 *   This routine is NOT intended to be altered by the user since it forms
 *   part of the "generic" set of start-up and scheduler code.
 */

#include <stdio.h>
#include <string.h>
#include "startup.h"

/* The Ada program entry point */
extern void _user_entry (void);

/* Binder output routine to call to elaborate the Ada */
extern void adainit (void);

/* Interrupt handler (Ada) for use after the elaborations have completed */
extern void _interrupt_handler (void) __attribute__ ((interrupt));

#ifdef GPS_SCHEDULER
/* Scheduler initialisation routine (Ada) */
extern void _initialise_scheduler (void);
#endif

/* Prototypes for externals */
extern void _wdt_table_setup (void);
extern void _wdt_setup (void);
extern void _wdt_service (void);
extern void _useradainit (void);
extern void _preelab (void);
extern void _error_handler (void);
extern void _illint (void);

/* Local prototypes */
void _ramprep (void);
static void local_handler (void) __attribute__ ((interrupt));

/* Access to the table of addresses in the preamble code */
extern struct addr_array_fmt _address_array;

/* Access to the WDT data */
extern volatile struct wdt_buffer_fmt _wdt_data;

/*
 * Entry point from the assembler preamble code
 */
void
_entry_point (void)
{
  /* Point to the WDT data */
  volatile struct wdt_buffer_fmt *wdt_bufptr = &_wdt_data;
  /* Hardware addresses */
  volatile struct int_reg_fmt *const interrupt_regs = (struct int_reg_fmt *) INTERRUPT_REGS;
  volatile unsigned long *const interrupt_vector = (unsigned long *) INTERRUPT_VECTOR;
  volatile unsigned long *const error_register = (unsigned long *) ERROR_REGISTERS;

  /* Use the local interrupt handler while elaborations are in progress */
  *interrupt_vector = (unsigned long) local_handler;
  interrupt_regs->mask = INTERRUPT_MASK_SRC;

  /* Disable software interrupts */
  interrupt_regs->soft_source = 0;

  /* Set up the WDT data table for use by various other components */
  _wdt_table_setup ();

  /* Initialise the SRC and WDT(s) */
  _wdt_setup ();

  /* Perform the initial service of the SRC and WDT(s) */
  _wdt_service ();

  /* Use local interrupt handler during elaboration and to monitor completion of same */
  asm volatile("eni");

  /* Elaborate the Ada */
  _ramprep ();

  /* Indicate initialisation is complete */
  wdt_bufptr->ada_ready_to_go = 1;

  /*
   * Check for confirmation that code execution can continue by waiting
   * for the 'm_servesr' flag to be set true by the local interrupt handler.
   * A time-out counter in local interrupt handler monitors the progress.
   */
  /*while (!wdt_bufptr->m_servesr)
    ;*/

#ifdef GPS_SCHEDULER
  /* Initialise the (Ada) scheduler */
  _initialise_scheduler ();
#endif

  /* Switch to the main (Ada) interrupt handler */
  *interrupt_vector = (unsigned long) _interrupt_handler;

  /* Read and save the contents of the error register */
  wdt_bufptr->err_reg = *error_register;

  /* Start the Ada program via the user's entry step */
  _user_entry ();

  /* Catch a return from the Ada program */
  _error_handler ();

  return;
}


/*
 * Ram preparation.
 * NB This function is also used by CTT for elaboration.
 */
void
_ramprep (void)
{
  /* Point to the table of addresses */
  struct addr_array_fmt *addr_array = &_address_array;

  /* Clear the .bss section */
  memset (addr_array->sbss, 0, addr_array->ebss - addr_array->sbss);

  /* Upload the .data section to ram */
  memcpy (addr_array->sdata, addr_array->sidata, addr_array->edata - addr_array->sdata);
}


/*
 * Monitor completion of initialisation process via local interrupt handler;
 * The "n_serves" entry of the WDT data table is used as a time-out counter.
 */
static void
local_handler (void)
{
  /* Point to the WDT data */
  volatile struct wdt_buffer_fmt *wdt_bufptr = &_wdt_data;
  /* Hardware addresses */
  volatile struct int_reg_fmt *const interrupt_regs = (struct int_reg_fmt *) INTERRUPT_REGS;

  /* SRC interrupt? */
  if (interrupt_regs->source & INTERRUPT_MASK_SRC)
    {
      _wdt_service ();
      /* Count the service */
      wdt_bufptr->n_serves++;
      /* Wait until lower limit reached */
      if (wdt_bufptr->n_serves > wdt_bufptr->wdt_low_limit)
	{
          /* When elaboration complete indicate it */
	  if (wdt_bufptr->ada_ready_to_go)
	    wdt_bufptr->m_servesr = 1;
	  /* Initialisation time-out check */
	  else if (wdt_bufptr->n_serves > wdt_bufptr->wdt_supr_limit)
            _error_handler ();
	}
    }
  /* Not an SRC interrupt - assume it's an illegal one */
  else
    _illint ();
}
