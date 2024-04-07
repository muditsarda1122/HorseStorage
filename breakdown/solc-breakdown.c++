// 0x6080604052348015600e575f80fd5b5060a58061001b5f395ff3fe6080604052348015600e575f80fd5b50600436106030575f3560e01c8063cdfead2e146034578063e026c017146045575b5f80fd5b6043603f3660046059565b5f55565b005b5f5460405190815260200160405180910390f35b5f602082840312156068575f80fd5b503591905056fea26469706673582212208783f480d37c3c93490ffb810a515971dfe264c78ddcf2357a52b1da877bbada64736f6c63430008160033
// 3 sections:
// 1. contract creation bytecode
// 2. runtime bytecode
// 3. metadata

// 1. contract creation bytecode

// free memory pointer 0x40 is the free memory pointer. It gets updated everytime we write to the memory. So always before adding something to the memory we read the free memory pointer first
PUSH1 0x80     // [0x80]
    PUSH1 0x40 // [0x40, 0x80]
    MSTORE     // []        // memory: 0x40 -> 0x80

        // what does this chunk do?
        // if value is not sent in constructor, jump to PC 0x0e. If value is sent, revert.
        // (NOTE: if we want to skip this part of bytecode in our compiled code, we can simply make the constructor payable. This will make the contract more gas efficient. But this check is imp from security sense)
        CALLVALUE              // [msg.value]
            DUP1               // [msg.value, msg.value]
                ISZERO         // [msg.value == 0, msg.value] (if msg.value = 0, then 1 is returned, else 0)
                    PUSH1 0x0e // [0x0e, msg.value == 0, msg.value]
    JUMPI                      // [msg.value] (if msg.value = 0, jump to PC 0x0e)
        PUSH0                  // [0x00, msg.value] (if msg.value != 0, continue)
            DUP1               // [0x00, 0x00, msg.value]
                REVERT         // [msg.value]

                    // jump destination if msg.value = 0(i.e. no value sent in constructor)
                    // stick the runtime code to chain
                    JUMPDEST           // [msg.value]
                        POP            // []
                            PUSH1 0xa5 // [0xa5]
    DUP1                               // [0xa5, 0xa5]
        PUSH2 0x001b                   // [0x001b, 0xa5, 0xa5]
    PUSH0                              // [0x00, 0x001b, 0xa5, 0xa5]
        CODECOPY                       // [0xa5] (take all code from stack 0x001b to 0xa5 and store in memory offset 0x00)
            PUSH0                      // [0x00, 0xa5]
                RETURN                 // []  (return all code from memory offset 0x00 of length 0xa5 to the chain)
                    INVALID            // []  (end of contract creation bytecode)

                        // 2. runtime code

                        // free memory pointer
                        PUSH1 0x80 PUSH1 0x40 MSTORE

                            // checking for msg.value, revert if not zero(as no function in the contract is payable)
                            CALLVALUE              // [msg.value]
                                DUP1               // [msg.value, msg.value]
                                    ISZERO         // [msg.value == 0, msg.value]
                                        PUSH1 0x0e // [0x0e, msg.value == 0, msg.value]
    JUMPI                                          // [msg.value]
                                                   // jump to 'continue!' if msg.value = 0

        PUSH0          // [0x00, msg.value]
            DUP1       // [0x00, 0x00, msg.value]
                REVERT // [msg.value]

                    // if msg.value == 0, start here
                    // continue!
                    // this part is checking if there is even enough calldata for a function selector
                    JUMPDEST           // [msg.value]
                        POP            // []
                            PUSH1 0x04 // [0x04]
    CALLDATASIZE                       // [calldata_size, 0x04]
        LT                             // [calldata_size < 0x04]
            PUSH1 0x30                 // [0x30, calldata_size < 0x04]
    JUMPI                              // []
                                       // if calldata_size < 0x04, jump to 'calldata_jump'

        // function dispatching in solidity
        PUSH0              // [0]
            CALLDATALOAD   // [32 bytes of calldata]
                PUSH1 0xe0 // [0xe0, 32 bytes of calldata] (0xe0 = 224 in decimal)
    SHR                    // [calldata[0:4]] (get first 4 bytes of calldata)

        // dispatching for updateHorseNumber
        DUP1                 // [func_selector, func_selector]
            PUSH4 0xcdfead2e // [0xcdfead2e, func_selector, func_selector] (function selector for updateHorseNumber)
    EQ                       // [func_selector == 0xcdfead2e, func_selector]
        PUSH1 0x34           // [0x34, func_selector == 0xcdfead2e, func_selector]
    JUMPI                    // [func_selector]
                             // if func_selector = 0xcdfead2e, jump to 'set_number_of_horses'

        // dispatching for getNumberOfHorses
        DUP1                 // [func_selector, func_selector]
            PUSH4 0xe026c017 // [0xe026c017, func_selector, func_selector]
    EQ                       // [func_selector == 0xe026c017, func_selector]
        PUSH1 0x45           // [0x45, func_selector == 0xe026c017, func_selector]
    JUMPI                    // [func_selector]
                             // if func_selector = 0xe026c017, jump to 'get_number_of_horses'

        // (if the given function selector does not match with any of our selectors, then this part will execute which will revert the transaction)
        // calldata_jump
        JUMPDEST           // []
            PUSH0          // [0x00]
                DUP1       // [0x00, 0x00]
                    REVERT // []

                        // updateHorseNumber jump dest 1
                        // setup jumping program counters in the stack
                        JUMPDEST       // [func_selector]
                            PUSH1 0x43 // [0x43, func_selector]
    PUSH1 0x3f                         // [0x3f, 0x43, func_selector]
    CALLDATASIZE                       // [calldata_size, 0x3f, 0x43, func_selector]
        PUSH1 0x04                     // [0x04, calldata_size, 0x3f, 0x43, func_selector]
    PUSH1 0x59                         // [0x59, 0x04, calldata_size, 0x3f, 0x43, func_selector]
    JUMP                               // [0x04, calldata_size, 0x3f, 0x43, func_selector]

        // updateHorseNumber dest 4
        // we can finally run a sstore to save value in storage
        // we have done:
        // 1. function dispatch
        // 2. checked for msg.value
        // 3. checked that calldata is long enough
        // 4. received the number to use from the calldata
        JUMPDEST         // [calldata(of numberToUpdate), 0x43, func_selector]
            PUSH0        // [0x00, calldata(of numberToUpdate), 0x43, func_selector]
                SSTORE   // [0x43, func_selector]
                    JUMP // [func_selector]
                         // jump to dest 5

                        // dest 5
                        JUMPDEST // [func_selector]
                            STOP // [func_selector]

                                // getNumberOfHorses
                                JUMPDEST               // [func_selector]
                                    PUSH0              // [0x00, func_selector]
                                        SLOAD          // [num_horses, func_selector] (at storage slot 0, we have the number of horses)
                                            PUSH1 0x40 // [0x40, num_horses, func_selector] (0x40 is the free memory pointer)
    MLOAD                                              // [0x80, num_horses, func_selector]    // memory: [0x40 -> 0x80]
        SWAP1                                          // [num_horses, 0x80, func_selector]
            DUP2                                       // [0x80, num_horses, 0x80, func_selector]
                MSTORE                                 // [0x80, func_selector] // memory: [0x80 -> num_horses]
                    PUSH1 0x20                         // [0x20, 0x80, func_selector] (need to move the free memory pointer to the next slot)
    ADD                                                // [0xa0, func_selector]
        PUSH1 0x40                                     // [0x40, 0xa0, func_selector]
    MLOAD                                              // [0x80, 0xa0, func_selector]
        DUP1                                           // [0x80, 0x80, 0xa0, func_selector]
            SWAP2                                      // [0xa0, 0x80, 0x80, func_selector]
                SUB                                    // [0xa0 - 0x80, 0x80, func_selector]
                    SWAP1                              // [0x80, 0xa0 - 0x80, func_selector]
                                                       // return a value of size 0xa0 - 0x80(i.e. 32 bytes) from memory offset 0x80
                        RETURN                         // [func_selector]

                            // updateHorseNumber jump dest 2
                            // check if there is a value to update the horseNumber to
                            // 4 bytes for the selector + 32 bytes for the data
                            JUMPDEST           // [0x04, calldata_size, 0x3f, 0x43, func_selector]
                                PUSH0          // [0x00, 0x04, calldata_size, 0x3f, 0x43, func_selector]
                                    PUSH1 0x20 // [0x20, 0x00, 0x04, calldata_size, 0x3f, 0x43, func_selector]
    DUP3                                       // [0x04, 0x20, 0x00, 0x04, calldata_size, 0x3f, 0x43, func_selector]
        DUP5                                   // [calldata_size, 0x04, 0x20, 0x00, 0x04, calldata_size, 0x3f, 0x43, func_selector]
            SUB                                // [calldata_size - 0x04, 0x20, 0x00, 0x04, calldata_size, 0x3f, 0x43, func_selector]
                // here we are checking if after function selector in calldata, is there more than 32 bytes ahead(i.e. is there data to work with?)?
                SLT                // [calldata_size - 0x04 < 0x20, 0x00, 0x04, calldata_size, 0x3f, 0x43, func_selector]
                    ISZERO         // [calldata_has_32bytes_data, 0x00, 0x04, calldata_size, 0x3f, 0x43, func_selector]
                        PUSH1 0x68 // [0x68, calldata_has_32bytes_data, 0x00, 0x04, calldata_size, 0x3f, 0x43, func_selector]
    JUMPI                          // [0x00, 0x04, calldata_size, 0x3f, 0x43, func_selector]
                                   // jump to jump destination 3 if calldata size if more than 32 bytes(4 for selector and additional 32 for data)

        // push if there isn't enough calldata
        PUSH0          // [0x00, 0x00, 0x04, calldata_size, 0x3f, 0x43, func_selector]
            DUP1       // [0x00, 0x00, 0x00, 0x04, calldata_size, 0x3f, 0x43, func_selector]
                REVERT // [calldata_size, 0x3f, 0x43, func_selector]

                    // updateHorseNumber jump dest 3
                    JUMPDEST                     // [0x00, 0x04, calldata_size, 0x3f, 0x43, func_selector]
                        POP                      // [0x04, calldata_size, 0x3f, 0x43, func_selector]
                            CALLDATALOAD         // [calldata(of numberToUpdate), calldata_size, 0x3f, 0x43, func_selector]
                                SWAP2            // [0x3f, calldata_size, calldata(of numberToUpdate), 0x43, func_selector]
                                    SWAP1        // [calldata_size, 0x3f, calldata(of numberToUpdate), 0x43, func_selector]
                                        POP      // [0x3f, calldata(of numberToUpdate), 0x43, func_selector]
                                            JUMP // [calldata(of numberToUpdate), 0x43, func_selector]
                                                 // jump to updateHorseNumber dest 4

                                                // Metadata
                                                INVALID
                                                    LOG2
                                                        PUSH5 0x6970667358 INVALID
                                                            SLT
                                                                KECCAK256
                                                                    DUP8
                                                                        DUP4
                                                                            DELEGATECALL
                                                                                DUP1
                                                                                    INVALID
                                                                                        PUSH29 0x3c93490ffb810a515971dfe264c78ddcf2357a52b1da877bbada64736f PUSH13 0x63430008160033