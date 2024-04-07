//SPDX-License-Identifier:MIT
pragma solidity ^0.8.18;

contract HorseStoreYul {
    uint256 numberOfHorses;

    function updateHorseNumber(uint256 newNumberOfHorses) external {
        // numberOfHorses = newNumberOfHorses;
        assembly {
            sstore(numberOfHorses.slot, newNumberOfHorses)
        }
    }

    function readNumberOfHorses() external view returns (uint256) {
        // return numberOfHorses;
        assembly {
            let num := sload(numberOfHorses.slot)
            mstore(0, num)
            return(0, 0x20)
        }
    }
}
