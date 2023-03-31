Exception Causes (EXCCAUSE)
===========================


+----------+--------------------------------+-----------------------------------------+-------------+----------+
| EXCCAUSE | Cause Name                     | Cause Description                       | Required    | EXCVADDR |
| Code     |                                |                                         | Option      | Loaded   |
+==========+================================+=========================================+=============+==========+
| 0        | IllegalInstructionCause        | Illegal instruction                     | Exception   | No       |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 1        | SyscallCause                   | SYSCALL instruction                     | Exception   | No       |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 2        | InstructionFetchErrorCause     | Processor internal physical address or  | Exception   | Yes      |
|          |                                | data error during instruction fetch     |             |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 3        | LoadStoreErrorCause            | Processor internal physical address or  | Exception   | Yes      |
|          |                                | data error during load or store         |             |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 4        | Level1InterruptCause           | Level-1 interrupt as indicated by set   | Interrupt   | No       |
|          |                                | level-1 bits in the INTERRUPT register  |             |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 5        | AllocaCause                    | MOVSP instruction, if caller’s          | Windowed    | No       |
|          |                                | registers are not in the register file  | Register    |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 6        | IntegerDivideByZeroCause       | QUOS, QUOU, REMS, or REMU divisor       | 32-bit      | No       |
|          |                                | operand is zero                         | Integer     |          |
|          |                                |                                         | Divide      |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 7        | Reserved for Tensilica         |                                         |             |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 8        | PrivilegedCause                | Attempt to execute a privileged         | MMU         | No       |
|          |                                | operation when CRING != 0               |             |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 9        | LoadStoreAlignmentCause        | Load or store to an unaligned address   | Unaligned   | Yes      |
|          |                                |                                         | Exception   |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 10..11   | Reserved for Tensilica         |                                         |             |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 12       | InstrPIFDateErrorCause         | PIF data error during instruction fetch | Processor   | Yes      |
|          |                                |                                         | Interface   |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 13       | LoadStorePIFDataErrorCause     | Synchronous PIF data error during       | Processor   | Yes      |
|          |                                | LoadStore access                        | Interface   |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 14       | InstrPIFAddrErrorCause         | PIF address error during instruction    | Processor   | Yes      |
|          |                                | fetch                                   | Interface   |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 15       | LoadStorePIFAddrErrorCause     | Synchronous PIF address error during    | Processor   | Yes      |
|          |                                | LoadStore access                        | Interface   |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 16       | InstTLBMissCause               | Error during Instruction TLB refill     | MMU         | Yes      |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 17       | InstTLBMultiHitCause           | Multiple instruction TLB entries        | MMU         | Yes      |
|          |                                | matched                                 |             |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 18       | InstFetchPrivilegeCause        | An instruction fetch referenced a       | MMU         | Yes      |
|          |                                | virtual address at a ring level less    |             |          |
|          |                                | than CRING                              |             |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 19       | Reserved for Tensilica         |                                         |             |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 20       | InstFetchProhibitedCause       | An instruction fetch referenced a page  | Region      | Yes      |
|          |                                | mapped with an attribute that does not  | Protection  |          |
|          |                                | permit instruction fetch                | or MMU      |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 21..23   | Reserved for Tensilica         |                                         |             |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 24       | LoadStoreTLBMissCause          | Error during TLB refill for a load or   | MMU         | Yes      |
|          |                                | store                                   |             |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 25       | LoadStoreTLBMultiHitCause      | Multiple TLB entries matched for a load | MMU         | Yes      |
|          |                                | or store                                |             |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 26       | LoadStorePrivilegeCause        | A load or store referenced a virtual    | MMU         | Yes      |
|          |                                | address at a ring level less than CRING |             |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 27       | Reserved for Tensilica         |                                         |             |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 28       | LoadProhibitedCause            | A load referenced a page mapped with an | Region      | Yes      |
|          |                                | attribute that does not permit loads    | Protection  |          |
|          |                                |                                         | or MMU      |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 29       | StoreProhibitedCause           | A store referenced a page mapped with   | Region      | Yes      |
|          |                                | an attribute that does not permit       | Protection  |          |
|          |                                |                                         | or MMU      |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 30..31   | Reserved for Tensilica         |                                         |             |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 32..39   | CoprocessornDisabled           | Coprocessor n instruction when cpn      | Coprocessor | No       |
|          |                                | disabled. n varies 0..7 as the cause    |             |          |
|          |                                | varies 32..39                           |             |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+
| 40..63   | Reserved                       |                                         |             |          |
+----------+--------------------------------+-----------------------------------------+-------------+----------+

Infos from Xtensa Instruction Set Architecture (ISA) Reference Manual
