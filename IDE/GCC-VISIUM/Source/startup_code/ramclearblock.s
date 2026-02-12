!************************************************************************
!*									*
!*				   Goodrich				*
!*			    Engine Control Systems			*
!*									*
!************************************************************************
!*
!*	TITLE:- 	Block RAM clear
!*	IDENTITY:-	RAMCLEARBLOCK01A.S
!*	WRITTEN BY:-	P.A.Banting
!*	MODIFIED BY:-	P.A.Banting
!*	DATE:-		19-MAY-04
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
!*	Calling Sequences for Procedure RAMCLEARBLOCK			*
!*									*
!************************************************************************
!*
!*	Entry point:	_tstrama
!*
!*	Inputs: 	R1 = Bottom of address range to be cleared
!*			     (unsigned longword)
!*			R2 = Top of address range to be cleared
!*			     (unsigned longword)
!*			     (this address is not included in the clear)
!*
!*	Return value:	R1 contains 0 (longword)
!*
!*	R21 is used as the link register.
!*
!*	All values are represented as unsigned longwords.
!*
!************************************************************************
!*									*
!*	Functional Specification of Procedure RAMCLEARBLOCK		*
!*									*
!************************************************************************
!*
!*	This function clears the RAM within the specified address range,
!*	which extends from R1 (lowest address) up to but not including
!*	R2 (highest address). The clear is performed in blocks of 32
!*	longwords at a time (for efficiency).
!*
!*	It is assumed that the addresses passed in R1 and R2 are aligned
!*	on 32-longword boundaries and that R2 is at least one block
!*	higher than R1.
!*
!*	A value of 0 is returned in R1.
!*
!*	Note that the entry point has this name so that it can be called
!*	directly from 'gnat_ada_start'.
!*
!************************************************************************
!*									*
!*	Change History of Procedure RAMCLEARBLOCK			*
!*									*
!************************************************************************
!*
!*	Version 	Author		Date		Change
!*							Description
!*
!*	00A		P.A.Banting	19-MAY-04	1. Original
!*	01A		P.A.Banting	19-MAY-04	1. Corrected loop branch destination
!*
	.sbttl	"Block RAM clear"

.global 	_tstramb
	.align	4
_tstrama:

!	Perform the clear a block at a time

	sub.l	r3,r2,r1		! Calculate number of bytes to be cleared

ram_clear_loop:
	write.l (r1),r0 		! Clear a block of 32 consecutive longwords
	write.l 1(r1),r0
	write.l 2(r1),r0
	write.l 3(r1),r0
	write.l 4(r1),r0
	write.l 5(r1),r0
	write.l 6(r1),r0
	write.l 7(r1),r0
	write.l 8(r1),r0
	write.l 9(r1),r0
	write.l 10(r1),r0
	write.l 11(r1),r0
	write.l 12(r1),r0
	write.l 13(r1),r0
	write.l 14(r1),r0
	write.l 15(r1),r0
	write.l 16(r1),r0
	write.l 17(r1),r0
	write.l 18(r1),r0
	write.l 19(r1),r0
	write.l 20(r1),r0
	write.l 21(r1),r0
	write.l 22(r1),r0
	write.l 23(r1),r0
	write.l 24(r1),r0
	write.l 25(r1),r0
	write.l 26(r1),r0
	write.l 27(r1),r0
	write.l 28(r1),r0
	write.l 29(r1),r0
	write.l 30(r1),r0
	write.l 31(r1),r0
	subi	r3,4*32 		! Decrement byte count by one block
	brr	ne,ram_clear_loop	! Repeat loop until all locations cleared
	addi	r1,4*32 		! Advance to next block of locations (delay slot)

!	Clearing complete. Exit with 0 returned in R1

	bra	tr,r21,r0		! Return
	moviq	r1,0			! Set R1 to 0 as return value (delay slot)
.end

