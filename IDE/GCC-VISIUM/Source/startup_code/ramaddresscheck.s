!************************************************************************
!*									*
!*				   Goodrich				*
!*			    Engine Control Systems			*
!*									*
!************************************************************************
!*
!*	TITLE:- 	RAM address decoder test
!*	IDENTITY:-	RAMADDRESSCHECK01A.S
!*	WRITTEN BY:-	P.A.Banting
!*	MODIFIED BY:-	P.A.Banting
!*	DATE:-		14-MAY-04
!*
!*
!*
!*	©2004 Goodrich Control Systems Limited. Copyright in this document belongs to
!*	Goodrich Control Systems Limited and all rights are reserved. No reproduction
!*	of all or part of this document shall be made without the prior written consent
!*	of Goodrich Control Systems Limited. This document contains information that
!*	may be confidential and its disclosure to others requires the written consent
!*	of Goodrich Control Systems Limited.
!*
!************************************************************************
!*									*
!*	Calling Sequences for Procedure RAMADDRESSCHECK 		*
!*									*
!************************************************************************
!*
!*	Entry point:	_tstrama
!*
!*	Inputs: 	R1 = Bottom of address range to be tested
!*			     (unsigned longword)
!*			R2 = Top of address range to be tested
!*			     (unsigned longword)
!*			     (this address is not included in the test)
!*
!*	Return value:	R1 is non-zero if an error is detected, otherwise 0
!*			     (longword)
!*
!*	All values are represented as unsigned longwords.
!*
!************************************************************************
!*									*
!*	Functional Specification of Procedure RAMADDRESSCHECK		*
!*									*
!************************************************************************
!*
!*	This function performs a RAM address decoder test and also clears the
!*	RAM within the specified address range, which extends from R1 (lowest
!*	address) up to but not including R2 (highest address).
!*
!*	The address decoder test is performed in three passes through the
!*	address range (in ascending address order) as follows:
!*
!*	1. Each RAM location within the address range is loaded with the LS
!*	word of its address.
!*	2. The data loaded to each location on the first pass is read back and
!*	verified, and replaced with the one's complement of the location's
!*	address.
!*	3. The data loaded to each location on the second pass is read back and
!*	verified, and replaced with zero.
!*
!*	If all the data verifies correctly, a value of 0 (no failure) is returned
!*	in R1. If a failure is detected at any point, the procedure is immediately
!*	aborted and a value of 1 (failure) is returned in R1.
!*
!*	If R1 and R2 are equal (no locations to be tested), 0 is returned in R1
!*	and no RAM address decoder test is performed. If R2 is less than R1
!*	(unsigned), 1 is returned in R1 to indicate an invalid address range
!*	and no RAM address decoder test is performed.
!*
!*	All RAM operations are performed as words.
!*
!*	It is assumed that the addresses passed in R1 and R2 are even.
!*
!************************************************************************
!*									*
!*	Change History of Procedure RAMADDRESSCHECK			*
!*									*
!************************************************************************
!*
!*	Version 	Author		Date		Change
!*							Description
!*
!*	00A		P.A.Banting	07-MAY-04	1. Original (derived from project specific variants)
!*	00B		P.A.Banting	13-MAY-04	1. Documentation changes for STAC G2171
!*	01A		P.A.Banting	14-MAY-04	1. Documentation changes for STAC G2185
!*
	.sbttl	"RAM address test"

.global 	_tstrama
	.align	4
_tstrama:

!	Perform the test. The exit code expects the success/failure status in r6.

	sub.l	r6,r2,r1		! r6 = Number of locations to test (in bytes)
	brr	eq,exit 		! Just exit with no failure if no locations to test
	nop				! Delay slot (no flags affected)
	brr	cs,exit 		! Exit with failure if maximum address below minimum
	lsr.l	r2,r6,1 		! Convert to number of words (delay slot)
	move.l	r4,r1			! r4 = Copy of start address
	move.l	r3,r2			! r3 = Copy of number of words

!	r1 = Start address
!	r2 = Number of word locations
!	r3 = Number of word locations (loop count)
!	r4 = Current address (initialised with start address)
!	r6 <> 0 (will be returned as status unless all tests pass)

!	Write to each location, using LS word of location's address as data.

write1: write.w (r4),r4 		! Write location's own address
	subi	r3,1			! Decrement loop count
	brr	ne,write1
	addi	r4,2			! Advance to next location (delay slot)

!	Read back from each location and verify the data.
!	Write one's complement of address (LSW) to the location.

	move.l	r4,r1			! r4 = Copy of start address
	move.l	r3,r2			! r3 = Copy of number of words

read1:	read.w	r5,(r4) 		! Read back from current location
	sub.w	r0,r4,r5		! Failure if doesn't match address
	brr	ne,exit 		! r6 non-zero
	not.w	r5,r4			! Invert the address (delay slot)
	write.w (r4),r5 		! Write inverted address
	subi	r3,1			! Decrement loop count
	brr	ne,read1
	addi	r4,2			! Advance to next location (delay slot)

!	Read back from each location and verify the data.
!	Write zero to the location.
!	Use r1 as current location and r2 as loop count
!	(no more need to preserve them).

read2:
	read.w	r5,(r1) 		! Read back from current location
	not.w	r5,r5			! Invert the data
	sub.w	r0,r1,r5		! Failure if doesn't match address
	brr	ne,exit 		! r6 non-zero
	write.w (r1),r0 		! Clear the location (delay slot)
	subi	r2,1			! Decrement loop count
	brr	ne,read2
	addi	r1,2			! Advance to next location (delay slot)

!	Test complete without errors

	moviq	r6,0			! Return success

exit:
	bra	tr,r21,r0		! Return
	move.l	r1,r6			! Copy return value (delay slot)
.end

