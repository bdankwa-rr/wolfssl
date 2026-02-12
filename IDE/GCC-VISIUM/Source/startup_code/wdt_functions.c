/*
 *   C Start-up code SRC & WDT related routines.
 *   Trent 1000 Control & Safety Specific Version.
 *
 *   ©2007 Goodrich Control Systems Limited. Copyright in this document belongs to
 *   Goodrich Control Systems Limited and all rights are reserved. No reproduction
 *   of all or part of this document shall be made without the prior written consent
 *   of Goodrich Control Systems Limited. This document contains information that
 *   may be confidential and its disclosure to others requires the written consent
 *   of Goodrich Control Systems Limited.
 *
 *   Build Version       Author          Date                    Change Reference
 *   1.2                 M.E.Bradbury    24-Apr-2007             1. Trent 1000 request to add '_read_n_serves' method.
 *   1.1                 M.E.Bradbury    02-Apr-2007             1. Further modified for Trent to move FPGA WDT test
 *                                                                  functionality from here to Ada interrupt handler.
 *   1.0                 M.E.Bradbury    27-Mar-2007             1. Original - combines routines '_setup_hw'
 *                                                                  and '_service_hw' and provides method for
 *                                                                  changing SRC and WDT period.
 *
 * CM INFORMATION:
 *   CM FILE NAME:-               %name: wdt_functions.c %
 *   CM FILE VERSION:-            %version: 2 %
 *   CM FILE INSTANCE:-           %instance: 1 %
 *   CM ORIGINAL CHECK-IN DATE:-  %date_created: Mon Apr 30 17:07:28 2007 %
 *   CM ORIGINAL CHECK-IN BY:-    %created_by: dmh %
 *   CM MODIFIED CHECK-IN DATE:-  %date_modified: %
 *   CM MODIFIED CHECK-IN BY:-    %derived_by: dmh %
 */

/*
 *   NB Since some of these routines can be used prior to the elaboration process,
 *   care must be taken to ensure no declarations use the .bss and .data areas.
 *
 *   The routines are intended to be altered by the user to suit particular
 *   project requirements but the interface to other code items must remain
 *   unaltered.
 */

#include "startup.h"

/* Declare the WDT buffer and place it in appropriate linker section */
volatile struct wdt_buffer_fmt _wdt_data __attribute__ ((section (".wdtvars")));

extern void _ramprep (void);

/*
 * Initialise the primary WDT window in the hardware.
 */
void
_pwdt_wid (unsigned long int window)
{
  /* Hardware address */
  volatile struct fpga_wdt_fmt *const wdt1 = (struct fpga_wdt_fmt *) WDT1_ADDR;

  wdt1->window = window;
}

/*
 * Initialise the primary WDT delay in the hardware.
 */
void
_pwdt_del (unsigned long int delay)
{
  /* Hardware address */
  volatile struct fpga_wdt_fmt *const wdt1 = (struct fpga_wdt_fmt *) WDT1_ADDR;

  wdt1->delay = delay;
}

/*
 * Service the primary WDT in the FPGA on the serial i/o.
 */
void
_pwdt_ser (unsigned long int data)
{
  /* Hardware address */
  volatile struct fpga_wdt_fmt *const wdt1 = (struct fpga_wdt_fmt *) WDT1_ADDR;

  wdt1->service = data;
}

/*
 * Initialise the secondary WDT window in the hardware.
 */
void
_swdt_wid (unsigned long int window)
{
  /* Hardware address */
  volatile struct mcm_wdt_fmt *const wdt2 = (struct mcm_wdt_fmt *) WDT2_ADDR;

  wdt2->window = window;
}

/*
 * Service the secondary WDT in the hardware.
 */
void
_swdt_ser (unsigned long int data)
{
  /* Hardware address */
  volatile struct mcm_wdt_fmt *const wdt2 = (struct mcm_wdt_fmt *) WDT2_ADDR;

  wdt2->service = data;
}

/*
 * Service the sample rate clock in the hardware.
 */
void
_src_ser (unsigned long int delay)
{
  /* Hardware address */
  volatile unsigned long *const sample_rate_clock = (unsigned long *) SAMPLE_RATE_CLOCK;

  *sample_rate_clock = delay;
}

/*
 * A set of parameterless SRC & WDT service routines which
 * avoid the necessity of passing the service data required.
 * There was no equivalent of these in the assembler version;
 * they are used by the project's interrupt handler.
 */

/*
 * Service the primary WDT in the FPGA on the serial i/o.
 */
void
_pwdt_ser_np (void)
{
  _pwdt_ser (_wdt_data.prim_dat);
}

/*
 * Service the secondary WDT in the hardware.
 */
void
_swdt_ser_np (void)
{
  _swdt_ser (_wdt_data.sec_del);
}

/*
 * Service the sample rate clock in the hardware.
 */
void
_src_ser_np (void)
{
  _src_ser (_wdt_data.src_del);
}

/*
 * Initialise the hardware for the primary and secondary WDTs.
 */
void
_wdt_setup (void)
{
  /* Hardware address */
  volatile unsigned long *const sio_control_10 = (unsigned long *) SIO_CONT_10_ADDR;

  *sio_control_10 = SERIO_10_NOT_TX;		/* Trent 1000: Set serial i/o 10 to not transmitting */
  _pwdt_wid (_wdt_data.prim_wid);		/* Set both window & delay on the primary WDT */
  _pwdt_del (_wdt_data.prim_del);
  *sio_control_10 = SERIO_10_TX;		/* Trent 1000: Set serial i/o 10 to transmitting */
  _swdt_wid (_wdt_data.sec_wid);		/* Only the window needs setting for the secondary WDT */
}

/*
 * Service the SRC and WDTs.
 */
void
_wdt_service (void)
{
  /* Refresh the sample rate clock */
  _src_ser (_wdt_data.src_del);

  /* Service the watchdog timers */
  _pwdt_ser (_wdt_data.prim_dat);
  _swdt_ser (_wdt_data.sec_del);
}

/*
 * Initialise the WDT data area.
 */
void
_wdt_table_setup (void)
{
  _wdt_data.src_del = SRCDELVAL;
  _wdt_data.prim_wid = PRIMWIDVAL;
  _wdt_data.prim_del = PRIMDELVAL;
  _wdt_data.prim_dat = PRIMDATVAL;
  _wdt_data.sec_wid = SECWIDVAL;
  _wdt_data.sec_del = SECDELVAL;
  _wdt_data.sec_dat = 0;
  _wdt_data.n_serves = 0;
  _wdt_data.m_servesr = 0;
  _wdt_data.wdt_low_limit = WDTLOWLIMIT;
  _wdt_data.wdt_supr_limit = WDTSUPRLIMIT;
  _wdt_data.ada_ready_to_go = 0;
  _wdt_data.event1 = 0;
  _wdt_data.err_reg = 0;
  _wdt_data.ram_prep = (unsigned long) &_ramprep;
  _wdt_data.src_wdt_ser = (unsigned long) &_wdt_service;
}

/*
 * Set the SRC refresh data in the WDT data buffer.
 */
void
_src_upd (unsigned long int value)
{
  _wdt_data.src_del = value;
}

/*
 * Set the primary WDT delay value in the WDT data buffer.
 */
void
_pdel_upd (unsigned long int value)
{
  _wdt_data.prim_del = value;
}

/*
 * Set the primary WDT data value in the WDT data buffer.
 */
void
_pdat_upd (unsigned long int value)
{
  _wdt_data.prim_dat = value;
}

/*
 * Set the secondary WDT delay value in the WDT data buffer.
 */
void
_sdel_upd (unsigned long int value)
{
  _wdt_data.sec_del = value;
}

/*
 * Read the WDT buffer 'n_serves' entry in the WDT data buffer.
 */
void
_read_n_serves (unsigned long int *setting)
{
  *setting = _wdt_data.n_serves;
}

/*
 * Set the WDT buffer event1 flag in the WDT data buffer.
 */
void
_set_event1 (unsigned long int setting)
{
  _wdt_data.event1 = setting;
}

/*
 * Read the WDT buffer event1 flag in the WDT data buffer.
 */
void
_read_event1 (unsigned long int *setting)
{
  *setting = _wdt_data.event1;
}
