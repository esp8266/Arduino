Exception Causes (EXCCAUSE)
===========================================

| EXC-CAUSE Code | Cause Name                 | Cause Description                                                                                           | Required Option          | EXC-VADDR Loaded |
|:--------------:|:---------------------------|:------------------------------------------------------------------------------------------------------------|:-------------------------|:----------------:|
| 0              | IllegalInstructionCause    | Illegal instruction                                                                                         | Exception                | No               |
| 1              | SyscallCause               | SYSCALL instruction                                                                                         | Exception                | No               |
| 2              | InstructionFetchErrorCause | Processor internal physical address or data error during instruction fetch                                  | Exception                | Yes              |
| 3              | LoadStoreErrorCause        | Processor internal physical address or data error during load or store                                      | Exception                | Yes              |
| 4              | Level1InterruptCause       | Level-1 interrupt as indicated by set level-1 bits in the INTERRUPT register                                | Interrupt                | No               |
| 5              | AllocaCause                | MOVSP instruction, if caller’s registers are not in the register file                                       | Windowed Register        | No               |
| 6              | IntegerDivideByZeroCause   | QUOS, QUOU, REMS, or REMU divisor operand is zero                                                           | 32-bit Integer Divide    | No               |
| 7              | Reserved for Tensilica     |                                                                                                             |                          |                  |
| 8              | PrivilegedCause            | Attempt to execute a privileged operation when CRING ? 0                                                    | MMU                      | No               |
| 9              | LoadStoreAlignmentCause    | Load or store to an unaligned address                                                                       | Unaligned Exception      | Yes              |
| 10..11         | Reserved for Tensilica     |                                                                                                             |                          |                  |
| 12             | InstrPIFDataErrorCause     | PIF data error during instruction fetch                                                                     | Processor Interface      | Yes              |
| 13             | LoadStorePIFDataErrorCause | Synchronous PIF data error during LoadStore access                                                          | Processor Interface      | Yes              |
| 14             | InstrPIFAddrErrorCause     | PIF address error during instruction fetch                                                                  | Processor Interface      | Yes              |
| 15             | LoadStorePIFAddrErrorCause | Synchronous PIF address error during LoadStore access                                                       | Processor Interface      | Yes              |
| 16             | InstTLBMissCause           | Error during Instruction TLB refill                                                                         | MMU                      | Yes              |
| 17             | InstTLBMultiHitCause       | Multiple instruction TLB entries matched                                                                    | MMU                      | Yes              |
| 18             | InstFetchPrivilegeCause    | An instruction fetch referenced a virtual address at a ring level less than CRING                           | MMU                      | Yes              |
| 19             | Reserved for Tensilica     |                                                                                                             |                          |                  |
| 20             | InstFetchProhibitedCause   | An instruction fetch referenced a page mapped with an attribute that does not permit instruction fetch      | Region Protection or MMU | Yes              |
| 21..23         | Reserved for Tensilica     |                                                                                                             |                          |                  |
| 24             | LoadStoreTLBMissCause      | Error during TLB refill for a load or store                                                                 | MMU                      | Yes              |
| 25             | LoadStoreTLBMultiHitCause  | Multiple TLB entries matched for a load or store                                                            | MMU                      | Yes              |
| 26             | LoadStorePrivilegeCause    | A load or store referenced a virtual address at a ring level less than CRING                                | MMU                      | Yes              |
| 27             | Reserved for Tensilica     |                                                                                                             |                          |                  |
| 28             | LoadProhibitedCause        | A load referenced a page mapped with an attribute that does not permit loads                                | Region Protection or MMU | Yes              |
| 29             | StoreProhibitedCause       | A store referenced a page mapped with an attribute that does not permit stores                              | Region Protection or MMU | Yes              |
| 30..31         | Reserved for Tensilica     |                                                                                                             |                          |                  |
| 32..39         | CoprocessornDisabled       | Coprocessor n instruction when cpn disabled. n varies 0..7 as the cause varies 32..39                       | Coprocessor              | No               |
| 40..63         | Reserved                   |                                                                                                             |                          |                  |


Infos from Xtensa Instruction Set Architecture (ISA) Reference Manual