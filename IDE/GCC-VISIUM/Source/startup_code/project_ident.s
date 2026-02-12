!-----------------------------------------------------------------------------------------------------------------------
!--   (c)2008 Goodrich Control Systems Limited. Copyright in this document belongs to Goodrich Control Systems Limited
!--   and all rights are reserved. No reproduction of all or part of this document shall be made without the prior
!--   written consent of Goodrich Control Systems Limited. This document contains information that may be confidential
!--   and its disclosure to others requires the written consent of Goodrich Control Systems Limited.
!-----------------------------------------------------------------------------------------------------------------------
!--   Assembler Unit Name(s)
!--   ----------------------
!--     projidentcore
!--     projidentdvs
!--
!--     Unit Description : This file sets up the project identity areas in the Core and DV SRE files.
!--
!--     
!-----------------------------------------------------------------------------------------------------------------------
!--Change History:
!--      Version         Date            Author          Description
!--      -------         ----            ------          -----------
!--      1.0             24-MAR-2008     I.ELLIS         Original, BR725 Control Software.
!-----------------------------------------------------------------------------------------------------------------------

        _pident_init = 0xFF09FF09      ! Contents of the Project Ident location
        _invpident_init = 0x00F600F6   ! Contents of the Inverse Project Ident location

   .section	.projidentcore,"a"
 	.long (_pident_init)
 	.long (_invpident_init)

   .section	.projidentdvs,"a"
 	.long (_pident_init)
 	.long (_invpident_init)

.end
