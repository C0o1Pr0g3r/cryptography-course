import { BitShift, MyBigInt } from "./my-big-int.js";


main();

function main() {
    testSetHexAndGetHexMethods();
    testInvMethod();
    testXorOrAndMethods();
    testBitShifts();
    testAddAndSubMethods();
}

function testSetHexAndGetHexMethods() {
    console.log("1. Testing the 'setHex' and 'getHex' methods.");

    const hexNumbers = [
        "33ced2c76b26cae94e162c4c0d2c0ff7c13094b0185a3c122e732d5ba77efebc",
        "0",
        "7c000e4a5f00001d1c",
    ];

    const bigInts = [
        new MyBigInt(hexNumbers[0]),
        new MyBigInt(),
        new MyBigInt([124, 936543, 7452]),
    ];

    console.log("1.1. Testing the 'getHex' method.");
    console.log();
    for (let i = 0; i < hexNumbers.length; ++i) {
        const result = bigInts[i].getHex();
        console.log("Result:  ", result);
        console.log("Expected:", hexNumbers[i]);
        console.log(result === hexNumbers[i] ? "Test passed" : "Test failed");
        console.log();
    }

    console.log("1.2. Testing the 'setHex' method.");
    const hexNumber = "97cba82bfa7c";
    const bigInt = new MyBigInt(hexNumber);
    
    const result = bigInt.getHex();
    console.log("Result:  ", result);
    console.log("Expected:", hexNumber);
    console.log(result === hexNumber ? "Test passed" : "Test failed");
    console.log();
    console.log();
}

function testInvMethod() {
    console.log("2. Testing the 'INV' method.");

    const hexNumbers = [
        "c4c6b154c6a4ff9324fd7754233db7",
        "44413f87c4829125fa66e5",
    ];

    const invertedHexNumbers = [
        "ff3b394eab395b006cdb0288abdcc248",
        "ffbbbec0783b7d6eda05991a",
    ];

    const bigInts = hexNumbers.map(hexNumber => new MyBigInt(hexNumber));

    for (let i = 0; i < hexNumbers.length; ++i) {
        const result = MyBigInt.INV(bigInts[i]).getHex();
        console.log("Result:  ", result);
        console.log("Expected:", invertedHexNumbers[i]);
        console.log(result === invertedHexNumbers[i] ? "Test passed" : "Test failed");
        console.log();
    }
    console.log();
}

function testXorOrAndMethods() {
    console.log("3. Testing the 'XOR', 'OR', 'AND' methods.");

    const hexNumbers = [
        "51bf608414ad5726a3c1bec098f77b1b54ffb2787f8d528a74c1d7fde6470ea4",
        "403db8ad88a3932a0b7e8189aed9eeffb8121dfac05c3512fdb396dd73f6331c",
    ];

    const expectedResults = [
        "1182d8299c0ec40ca8bf3f49362e95e4ecedaf82bfd167988972412095b13db8",
        "51bff8ad9cafd72eabffbfc9befffffffcffbffaffdd779afdf3d7fdf7f73fbc",
        "403d208400a113220340808088d16a1b10121078400c1002748196dd62460204",
    ];

    const logicalFunctions = [MyBigInt.XOR, MyBigInt.OR, MyBigInt.AND];

    const bigInts = hexNumbers.map(hexNumber => new MyBigInt(hexNumber));

    const results = new Array<string>(logicalFunctions.length);

    for (let i = 0; i < logicalFunctions.length; ++i) {
        results[i] = logicalFunctions[i](bigInts[0], bigInts[1]).getHex();
        console.log(logicalFunctions[i].name);
        console.log("Result:  ", results[i]);
        console.log("Expected:", expectedResults[i]);
        console.log(results[i] === expectedResults[i] ? "Test passed" : "Test failed");
        console.log();
    }
    console.log();
}

function testBitShifts() {
    console.log("4. Testing the 'shiftR' and 'shiftL' methods.");

    const hexNumbers = [
        "77f4db377ecae90c",
        "ad63c52a31682b2a5119acc30c16b956005b943c",
    ];

    const bitShiftSizes = [36, 16];
    const bitShiftFunctions = [MyBigInt.shiftL, MyBigInt.shiftR];

    const expectedResults = [
        "77f4db377ecae90c000000000",
        "15ac78a5462d05654a233598618",
    ];

    const bigInts = hexNumbers.map(hexNumber => new MyBigInt(hexNumber));

    const results = new Array<string>(hexNumbers.length);

    for (let i = 0; i < bitShiftFunctions.length; ++i) {
        results[i] = bitShiftFunctions[i](bigInts[i], bitShiftSizes[i]).getHex();
        console.log(bitShiftFunctions[i].name);
        console.log("Result:  ", results[i]);
        console.log("Expected:", expectedResults[i]);
        console.log(results[i] === expectedResults[i] ? "Test passed" : "Test failed");
        console.log();
    }
    console.log();
}

function testAddAndSubMethods() {
    console.log("5. Testing the 'shiftR' and 'shiftL' methods.");

    const hexNumbers = [
        "6d34762d7750c134c6af7787e1ccb6e90e9715b6021bc93e03dc88b12530bcd329ea9d2268db750f53bbbdc7856cef745c96677cdff8e56dfcbf364692307a78e7ff6410abca37e40b6f80635b7b7a3f9e81877a62b7d427a95c1f331fe0b014",
        "2e81142eb81f54d496c464b0e1ae8eadcaf06336cd18bde2b15fd7abfbdc1d8107b5b20713239047e63d34cdeea429440efa4b14d968a6c1dc374adc49773db2",
        "33ced2c76b26cae94e162c4c0d2c0ff7c13094b0185a3c122e732d5ba77efebc",
        "22e962951cb6cd2ce279ab0e2095825c141d48ef3ca9dabf253e38760b57fe03",
    ];

    const arithmeticFunctions = [MyBigInt.ADD, MyBigInt.SUB];

    const expectedResults = [
        "6d34762d7750c134c6af7787e1ccb6e90e9715b6021bc93e03dc88b12530bcd3586bb15120fac9e3ea802278671b7e222786cab3ad11a350ae1f0df28e0c97f9efb51617beedc82bf1acb5314a1fa383ad7bd28f3c207ae985936a0f6957edc6",
        "10e570324e6ffdbc6b9c813dec968d9bad134bc0dbb061530934f4e59c2700b9",
    ];

    const bigInts = hexNumbers.map(hexNumber => new MyBigInt(hexNumber));

    const results = new Array<string>(arithmeticFunctions.length);

    for (let i = 0; i < arithmeticFunctions.length; ++i) {
        results[i] = arithmeticFunctions[i](bigInts[i * arithmeticFunctions.length], bigInts[i * arithmeticFunctions.length + 1]).getHex();
        console.log(arithmeticFunctions[i].name);
        console.log("Result:  ", results[i]);
        console.log("Expected:", expectedResults[i]);
        console.log(results[i] === expectedResults[i] ? "Test passed" : "Test failed");
        console.log();
    }
    console.log();
}
