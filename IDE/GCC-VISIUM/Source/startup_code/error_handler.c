/*
 *   C Start-up code error handler routine.
 *   Trent 1000 Specific Version.
 *
 *   ©2007 Goodrich Control Systems Limited. Copyright in this document belongs to
 *   Goodrich Control Systems Limited and all rights are reserved. No reproduction
 *   of all or part of this document shall be made without the prior written consent
 *   of Goodrich Control Systems Limited. This document contains information that
 *   may be confidential and its disclosure to others requires the written consent
 *   of Goodrich Control Systems Limited.
 *
 *   Build Version       Author          Date                    Change Reference
 *   1.2                 M.E.Bradbury    27-Mar-2007             1. WDT buffer access method changed.
 *                                                               2. Error registers now a structure.
 *                                                                  Ref: Review Report TBD
 *   1.1                 M.E.Bradbury    04-Oct-2006             1. WDT enumerated type and object
 *                                                                  reference moved to header file.
 *   1.0                 M.E.Bradbury    18-Jul-2006             1. Original
 *
 *
 * CM INFORMATION:
 *   CM FILE NAME:-               %name: error_handler.c %
 *   CM FILE VERSION:-            %version: 1 %
 *   CM FILE INSTANCE:-           %instance: 2 %
 *   CM ORIGINAL CHECK-IN DATE:-  %date_created: Wed Apr 18 13:17:44 2007 %
 *   CM ORIGINAL CHECK-IN BY:-    %created_by: asc %
 *   CM MODIFIED CHECK-IN DATE:-  %date_modified: %
 *   CM MODIFIED CHECK-IN BY:-    %derived_by: asc %
 */

/*
 *   NB Since this routine can be used prior to the elaboration process, care
 *   must be taken to ensure no declarations use the .bss and .data areas.
 *
 *   This routine is intended to be altered by the user to suit particular
 *   project requirements but the interface to other code items must remain
 *   unaltered.
 */

#include "startup.h"

/* WDT data area */
extern volatile struct wdt_buffer_fmt _wdt_data;

/*
 * Error handler.
 */
void
_error_handler (void)
{
  /* Point to the WDT data */
  volatile struct wdt_buffer_fmt *wdt_bufptr = &_wdt_data;
  /* Hardware address */
  volatile struct mcm_err_reg_fmt *const error_register = (struct mcm_err_reg_fmt *) ERROR_REGISTERS;

  /* Read error register continuously and o/p it via the WDT buffer */
  while(1)
    wdt_bufptr->err_reg = error_register->error;
}
