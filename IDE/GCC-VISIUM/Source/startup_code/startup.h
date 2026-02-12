/*
 *   C Start-up code header file.
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
 *   1.4                 M.E.Bradbury    18-Apr-2007             1. Error register format corrected.
 *   1.3                 M.E.Bradbury    27-Mar-2007             1. Superfluous commentary deleted.
 *                                                               2. Data formats changed to structures.
 *                                                                  Ref: Review Report TBD
 *   1.2                 J Speer         06-Oct-2006             1. Updated for Trent BS3 usage	1mS SRC etc.
 *   1.1                 M.E.Bradbury    04-Oct-2006             1. WDT and assembler enumerated types and
 *                                                                  object references moved to header file.
 *   1.0                 M.E.Bradbury    18-Jul-2006             1. Original
 *
 *
 */

/*
 *   NB Since this header file could be used in conjunction with routines
 *   which are used prior to the elaboration process, care must be taken to
 *   ensure no declarations use the .bss and .data areas.
 *
 *   The values in this header file are intended to be altered by the user
 *   to suit particular project requirements but the interface to other code
 *   items must remain unaltered.
 */

/*
 * The format of addresses from the assembler preamble.
 * NB This format must be retained for compatibility with the start-up assembler code.
 */
struct addr_array_fmt
{
  char *sidata;				/* Address of the rom copy of section .data */
  char *sdata;				/* Start address in ram of section .data */
  char *edata;				/* End address in ram of section .data */
  char *sbss; 				/* Start address in ram of section .bss */
  char *ebss;				/* End address in ram of section .bss */
};

/*
 * WDT buffer data area.
 * NB This format must be retained for compatibility with other utilities.
 */
struct wdt_buffer_fmt
{
  unsigned long int src_del;		/* Sample rate clock refresh value */
  unsigned long int prim_wid;		/* Primary watchdog window width */
  unsigned long int prim_del;		/* Primary watchdog delay */
  unsigned long int prim_dat;		/* Primary watchdog data */
  unsigned long int sec_wid;		/* Secondary watchdog window width */
  unsigned long int sec_del;		/* Secondary watchdog delay */
  unsigned long int sec_dat;		/* Secondary watchdog data (n/a for MCM WDT) */
  unsigned long int n_serves;		/* Number of interrupt services performed */
  unsigned long int m_servesr;		/* True when min. number of WDT services performed */
  unsigned long int wdt_low_limit;	/* Min. no. of WDT services before starting application */
  unsigned long int wdt_supr_limit;	/* Max. no. of WDT services for initn. to complete */
  unsigned long int ada_ready_to_go;	/* True when ready to start application */
  unsigned long int event1;		/* Event 1 has occurred (element was previously not used) */
  unsigned long int err_reg;		/* Storage for error register */
  unsigned long int ram_prep;		/* Address of RAM preparation routine */
  unsigned long int src_wdt_ser;	/* Address of SRC/WDT service routine */
};

/*
 * Format of the MCM's interrupt mechanism.
 */
struct int_reg_fmt
{
  unsigned long int mask;
  unsigned long int source;
  unsigned long int soft_source;
  unsigned long int int_proof_timer;
  unsigned long int int_count;
  unsigned long int brat;
};

/*
 * Format of the FPGA WDT on the serial i/o bus (i.e. the 'primary' WDT).
 * It is serviced by writing 0x2f51 (fixed data) to element 'service'.
 */
struct fpga_wdt_fmt
{
  unsigned long int service;
  unsigned long int delay;
  unsigned long int window;
};

/*
 * Format of the MCM's WDT (i.e. the 'secondary' WDT).
 */
struct mcm_wdt_fmt
{
  unsigned long int service;
  unsigned long int window;
};

/*
 * Format of the MCM's error registers.
 */
struct mcm_err_reg_fmt
{
  unsigned long int error;
  unsigned long int x1,x2;
  unsigned long int error_cv_addr;
  unsigned long int error_ci_addr;
  unsigned long int error_cv_data;
};

/*
 * Values for WDT buffer data area to suit SRC & WDT timings (&c.) required.
 */
#define SRCDELVAL     0x7f05f			/* Sample rate clock refresh value (2 ms) */

#define PRIMWIDVAL    0x13			/* Primary watchdog window width (0.2 ms) */
#define PRIMDELVAL    0x12			/* Primary watchdog delay (1.9 ms) */
#define PRIMDATVAL    0x2f51			/* Primary watchdog data (fixed value - J464/0602) */

#define SECWIDVAL     0x7cf			/* Secondary watchdog window width (1 ms) */
#define SECDELVAL     0xbb7			/* Secondary watchdog delay (1.5 ms) */

#define WDTLOWLIMIT   3				/* Min. no. of WDT services before starting application */
#define WDTSUPRLIMIT  800			/* Max. no. of WDT services for initn. to complete */

/*
 * Hardware addresses.
 */
#define SAMPLE_RATE_CLOCK    0x20002000
#define INTERRUPT_REGS       0x20002200
#define INTERRUPT_VECTOR     0x2000231c
#define WDT1_ADDR            0x2002D000
#define WDT2_ADDR            0x20003ff8
#define ERROR_REGISTERS      0x20002314
#define SIO_CONT_10_ADDR     0x2002e040

/*
 * Trent 1000 specific definitions.
 */
#define SERIO_10_NOT_TX      0x00000006;	/* Serial i/o 10 not transmitting */
#define SERIO_10_TX          0x00000026;	/* Serial i/o 10 transmitting */

/*
 * Interrupt mask to enable the SRC.
 */
#define INTERRUPT_MASK_SRC 2

/*
 * GPS scheduler.
 * If required, use '#define GPS_SCHEDULER 1'
 * otherwise use '#undef GPS_SCHEDULER'.
 */
#undef GPS_SCHEDULER				/* Trent 1000 control & safety do not use GPS */
