//SPDX-License-Identifier:MIT
pragma solidity ^0.8.18;

import {BaseTestV1, IHorseStore} from "./BaseTestV1.t.sol";
import {HorseStoreYul} from "../../src/horseStoreV1/HorseStoreYul.sol";

contract TestHorseStoreYul is BaseTestV1 {
    function setUp() public override {
        horseStore = IHorseStore(address(new HorseStoreYul()));
    }
}
