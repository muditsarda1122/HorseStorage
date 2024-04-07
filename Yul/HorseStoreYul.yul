// 'object'in Yul is the quivalent to 'contract' in solidity
object "HorseStoreYul" {
    // sticks the runtime code to chain (contract creation code)
    code {
        // contract deployment 
        // huff automatically gives us contract deployment but we need to deploy manually in yul
        datacopy(0, dataoffset("runtime"), datasize("runtime")) // 'datacopy' is yul special opcode which is similar to 'codecopy' in huff. Says, start from offset "dataoffset("runtime")" and copy code of size "datasize("runtime")" and stick to memory slot 0.
        // return the data of size "datasize("runtime")" from memory slot 0
        return(0, datasize("runtime"))
    }
    object "runtime" {
        // this will be the runtime code
        code{
            // first we need to define a function dispatcher
            switch selector()
            // updateHorseNumber(uint256)
            case 0xcdfead2e {
                storeNumber(decodeAsUint(0))
            }
            // readNumberOfHorses()
            case 0xe026c017 {
                returnUint(readNumber())
            }
            default {
                revert(0,0)
            }

            function storeNumber(newNumber) {
                sstore(0, newNumber)
            }

            function readNumber() -> storedNumber {
                storedNumber := sload(0)
            }

            /* -- decoding functions -- */

            // doing this returns the function selector
            function selector() -> s {
                s := div(calldataload(0), 0x100000000000000000000000000000000000000000000000000000000)
            }

            function decodeAsUint(offset) -> v {
                // mul(0, 0x20) = 0
                // add(4, 0) = 4
                let positionInCalldata := add(4, mul(offset, 0x20))
                // if the incoming calldata is less than 4(func sel)+32(data), then revert
                if lt(calldatasize(), add(positionInCalldata, 0x20)) {
                    revert(0,0)
                }
                // with positionInCalldata = 4, the below code will remove function selector from the calldata and load all else(data). That means this will give us the value to update.
                v := calldataload(positionInCalldata)
            }

            function returnUint(v) {
                mstore(0, v)
                return(0, 0x20)
            }
        }
    }
}