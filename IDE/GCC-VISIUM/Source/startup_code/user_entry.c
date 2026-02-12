/*
 *   C Start-up code user entry routine.
 *   Trent 1000 Control & Safety Specific Version.
 *
 *   �2007 Goodrich Control Systems Limited. Copyright in this document belongs to
 *   Goodrich Control Systems Limited and all rights are reserved. No reproduction
 *   of all or part of this document shall be made without the prior written consent
 *   of Goodrich Control Systems Limited. This document contains information that
 *   may be confidential and its disclosure to others requires the written consent
 *   of Goodrich Control Systems Limited.
 *
 *   Build Version       Author          Date                    Change Reference
 *   1.0                 M.E.Bradbury    27-Mar-2007             1. Original - added to C start-up code for
 *                                                                  compatibility with the assembler variant.
 *
 * CM INFORMATION:
 *   CM FILE NAME:-               %name: user_entry.c %
 *   CM FILE VERSION:-            %version: 1 %
 *   CM FILE INSTANCE:-           %instance: 1 %
 *   CM ORIGINAL CHECK-IN DATE:-  %date_created: Wed Apr 18 13:18:39 2007 %
 *   CM ORIGINAL CHECK-IN BY:-    %created_by: asc %
 *   CM MODIFIED CHECK-IN DATE:-  %date_modified: %
 *   CM MODIFIED CHECK-IN BY:-    %derived_by: asc %
 */

/*
 *   This routine is intended to be altered by the user to suit particular
 *   project requirements but the interface to other code items must remain
 *   unaltered.
 */

#include "startup.h"

/* extern void _ScheduleStartUpOperations (void);*/
extern int main();

void
_user_entry (void)
{
  main ();		/* Transfer control to the main C program */
}
