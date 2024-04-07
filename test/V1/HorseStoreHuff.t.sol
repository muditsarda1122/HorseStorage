//SPDX-License-Identifier:MIT
pragma solidity ^0.8.18;

import {BaseTestV1, IHorseStore} from "./BaseTestV1.t.sol";
import {HuffDeployer} from "foundry-huff/HuffDeployer.sol";
import {HorseStore} from "../../src/horseStoreV1/HorseStore.sol";

contract HorseStoreHuff is BaseTestV1 {
    // HuffDeployer library assumes that everything is inside src and we also don't need to add .huff extension
    string public constant HORSE_STORE_HUFF_LOCATION = "horseStoreV1/HorseStore";

    function setUp() public override {
        horseStore = IHorseStore((HuffDeployer.config().deploy(HORSE_STORE_HUFF_LOCATION)));
    }
}
