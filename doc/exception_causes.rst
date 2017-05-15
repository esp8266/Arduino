Exception Causes (EXCCAUSE)
===========================

+--------+------------+-----------------------------------------+-----------+--------+
| EXCCAU | Cause Name | Cause Description                       | Required  | EXCVAD |
| SE     |            |                                         | Option    | DR     |
| Code   |            |                                         |           | Loaded |
+========+============+=========================================+===========+========+
| 0      | IllegalIns | Illegal instruction                     | Exception | No     |
|        | tructionCa |                                         |           |        |
|        | use        |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 1      | SyscallCau | SYSCALL instruction                     | Exception | No     |
|        | se         |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 2      | Instructio | Processor internal physical address or  | Exception | Yes    |
|        | nFetchErro | data error during instruction fetch     |           |        |
|        | rCause     |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 3      | LoadStoreE | Processor internal physical address or  | Exception | Yes    |
|        | rrorCause  | data error during load or store         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 4      | Level1Inte | Level-1 interrupt as indicated by set   | Interrupt | No     |
|        | rruptCause | level-1 bits in the INTERRUPT register  |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 5      | AllocaCaus | MOVSP instruction, if caller’s          | Windowed  | No     |
|        | e          | registers are not in the register file  | Register  |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 6      | IntegerDiv | QUOS, QUOU, REMS, or REMU divisor       | 32-bit    | No     |
|        | ideByZeroC | operand is zero                         | Integer   |        |
|        | ause       |                                         | Divide    |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 7      | Reserved   |                                         |           |        |
|        | for        |                                         |           |        |
|        | Tensilica  |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 8      | Privileged | Attempt to execute a privileged         | MMU       | No     |
|        | Cause      | operation when CRING != 0               |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 9      | LoadStoreA | Load or store to an unaligned address   | Unaligned | Yes    |
|        | lignmentCa |                                         | Exception |        |
|        | use        |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 10..11 | Reserved   |                                         |           |        |
|        | for        |                                         |           |        |
|        | Tensilica  |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 12     | InstrPIFDa | PIF data error during instruction fetch | Processor | Yes    |
|        | taErrorCau |                                         | Interface |        |
|        | se         |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 13     | LoadStoreP | Synchronous PIF data error during       | Processor | Yes    |
|        | IFDataErro | LoadStore access                        | Interface |        |
|        | rCause     |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 14     | InstrPIFAd | PIF address error during instruction    | Processor | Yes    |
|        | drErrorCau | fetch                                   | Interface |        |
|        | se         |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 15     | LoadStoreP | Synchronous PIF address error during    | Processor | Yes    |
|        | IFAddrErro | LoadStore access                        | Interface |        |
|        | rCause     |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 16     | InstTLBMis | Error during Instruction TLB refill     | MMU       | Yes    |
|        | sCause     |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 17     | InstTLBMul | Multiple instruction TLB entries        | MMU       | Yes    |
|        | tiHitCause | matched                                 |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 18     | InstFetchP | An instruction fetch referenced a       | MMU       | Yes    |
|        | rivilegeCa | virtual address at a ring level less    |           |        |
|        | use        | than CRING                              |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 19     | Reserved   |                                         |           |        |
|        | for        |                                         |           |        |
|        | Tensilica  |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 20     | InstFetchP | An instruction fetch referenced a page  | Region    | Yes    |
|        | rohibitedC | mapped with an attribute that does not  | Protectio |        |
|        | ause       | permit instruction fetch                | n         |        |
|        |            |                                         | or MMU    |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 21..23 | Reserved   |                                         |           |        |
|        | for        |                                         |           |        |
|        | Tensilica  |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 24     | LoadStoreT | Error during TLB refill for a load or   | MMU       | Yes    |
|        | LBMissCaus | store                                   |           |        |
|        | e          |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 25     | LoadStoreT | Multiple TLB entries matched for a load | MMU       | Yes    |
|        | LBMultiHit | or store                                |           |        |
|        | Cause      |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 26     | LoadStoreP | A load or store referenced a virtual    | MMU       | Yes    |
|        | rivilegeCa | address at a ring level less than CRING |           |        |
|        | use        |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 27     | Reserved   |                                         |           |        |
|        | for        |                                         |           |        |
|        | Tensilica  |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 28     | LoadProhib | A load referenced a page mapped with an | Region    | Yes    |
|        | itedCause  | attribute that does not permit loads    | Protectio |        |
|        |            |                                         | n         |        |
|        |            |                                         | or MMU    |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 29     | StoreProhi | A store referenced a page mapped with   | Region    | Yes    |
|        | bitedCause | an attribute that does not permit       | Protectio |        |
|        |            | stores                                  | n         |        |
|        |            |                                         | or MMU    |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 30..31 | Reserved   |                                         |           |        |
|        | for        |                                         |           |        |
|        | Tensilica  |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 32..39 | Coprocesso | Coprocessor n instruction when cpn      | Coprocess | No     |
|        | rnDisabled | disabled. n varies 0..7 as the cause    | or        |        |
|        |            | varies 32..39                           |           |        |
+--------+------------+-----------------------------------------+-----------+--------+
| 40..63 | Reserved   |                                         |           |        |
+--------+------------+-----------------------------------------+-----------+--------+

Infos from Xtensa Instruction Set Architecture (ISA) Reference Manual
