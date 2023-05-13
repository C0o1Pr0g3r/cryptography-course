import { HEX_RADIX, MyBigInt } from "./my-big-int.js";

const SUCCESSFUL_MESSAGE = "Test passed";
const FAILURE_MESSAGE = "Test failed";

main();

function main() {
    const bigInts = [
        BigInt("0x33ced2c76b26cae94e162c4c0d2c0ff7c13094b0185a3c122e732d5ba77efebc"),
        BigInt("0x0"),
        BigInt("0x7c000e4a5f001d1c"),
        BigInt("0x206eff8bb36a5ae1a1b448c78c228245f36acffc291ff0d03df117e29a913af108c49c2bd33fcab1c727adeb8ce68423"),
        BigInt("0x94bc9b23b94f01093e3300173beffb72cb1d75bd7fbf9fc9351920338521e20a795203a8925242946357b934668839915302c3b6937eee99d93d3a347d6a2bf18d69a3cd436ca72bfe71e8e04e56a9dfb74b5d194682f3bdcd74c736519accf0"),
        BigInt("0x26ed30de213844da9a6ec122169dd4735765d278e825def825a01168e685f6b3"),
        BigInt("0xc62f1c49c52b5aa04a68a9e65444778e7be5947d5c46da1568437ec85c087f9a313aa307"),
        BigInt("0xb90ea61443336e54f9d40953ba564dcdb2be12e7c5a655f1"),
        BigInt("0xbe5e54bba744e6ae1b1737b1febd5d49b34825e535be49516126a254a92b177165131d21b88b2e2744b259489304f289cd101efcab08bdb3243903b75c0338e1b2e34da1"),
        BigInt("0xbabc72f16b2f0ac85e888853ad549074f669c4af53c3e693ba551ebae1c3ecc18676934ebe1adcf202e2aa8721409245a54c3793f9434d9faa40b8d5b0ade176e8b862647f20e2ec6006fa4e119134edf222f2b14ceb1a111148ba8255ff2e0973f78aec0be0e322f9767505f557060d07daf59345a1da7a45c36cb4a89cc03b"),
        BigInt("0x5435acf34a534bb53453453444094b014352543453453422e732d5ba775345cf"),
        BigInt("0x22e962951cb6cd2ce279ab0e2095825c141d48ef3ca9dabf253e38760b57fe03"),
    ];

    console.log("1. Testing the operation of setting and getting a hexadecimal string.");

    let bigInt = bigInts[0];
    let myBigInt = new MyBigInt();
    myBigInt.setHex(bigInt.toString(HEX_RADIX));
    console.log(
        new MyBigInt(bigInt.toString(HEX_RADIX)).getHex()
        ===
        bigInt.toString(HEX_RADIX) ?
            SUCCESSFUL_MESSAGE : FAILURE_MESSAGE,
    );

    bigInt = bigInts[7];
    console.log(
        new MyBigInt(bigInt.toString(HEX_RADIX)).getHex()
        ===
        bigInt.toString(HEX_RADIX) ?
            SUCCESSFUL_MESSAGE : FAILURE_MESSAGE,
    );

    console.log();
    console.log();

    console.log("2. Testing bitwise operations.");

    console.log("Unary bitwise operations.");

    const hexNumber = "d40e714617acb3d1e204f8d97a99790863584ede4b811ddf878d8757e1fc466b96b0d858";
    const invertedHexNumber = "2bf18eb9e8534c2e1dfb0726856686f79ca7b121b47ee220787278a81e03b994694f27a7";
    myBigInt = new MyBigInt(hexNumber);
    console.log(
        MyBigInt.INV(new MyBigInt(hexNumber)).getHex()
        ===
        invertedHexNumber ?
            SUCCESSFUL_MESSAGE : FAILURE_MESSAGE,
    );

    console.log();

    console.log("Binary logical bitwise operations.");

    const binaryLogicalBitwiseOperations = {
        BigInt: [
            (a: bigint, b: bigint) => a ^ b,
            (a: bigint, b: bigint) => a | b,
            (a: bigint, b: bigint) => a & b,
        ],
        MyBigInt: [
            MyBigInt.XOR,
            MyBigInt.OR,
            MyBigInt.AND,
        ],
    };

    bigInt = bigInts[3];
    let bigInt2 = bigInts[9];

    for (let i = 0; i < binaryLogicalBitwiseOperations.BigInt.length; ++i) {
        console.log(
            binaryLogicalBitwiseOperations.MyBigInt[i](
                new MyBigInt(
                    bigInt.toString(HEX_RADIX)),
                new MyBigInt(bigInt2.toString(HEX_RADIX),
                ),
            ).getHex()
            ===
            binaryLogicalBitwiseOperations.BigInt[i](bigInt, bigInt2).toString(HEX_RADIX) ?
                SUCCESSFUL_MESSAGE : FAILURE_MESSAGE,
        );
    }

    console.log();

    console.log("Bitwise shift operations.");
    
    const bitwiseShiftOperations = {
        BigInt: [
            (a: bigint, b: bigint) => a << b,
            (a: bigint, b: bigint) => a >> b,
        ],
        MyBigInt: [
            MyBigInt.shiftL,
            MyBigInt.shiftR,
        ],
    };

    const bitShiftSizes = [16, 48];

    bigInt = bigInts[3];

    for (let i = 0; i < bitwiseShiftOperations.BigInt.length; ++i) {
        console.log(
            bitwiseShiftOperations.MyBigInt[i](
                new MyBigInt(bigInt.toString(HEX_RADIX)), bitShiftSizes[i],
            ).getHex()
            ===
            bitwiseShiftOperations.BigInt[i](
                bigInt, BigInt(bitShiftSizes[i]),
            ).toString(HEX_RADIX) ?
                SUCCESSFUL_MESSAGE : FAILURE_MESSAGE,
        );
    }

    console.log();
    console.log();

    console.log("3. Testing arithmetic operations.");

    const arithmeticOperations = {
        BigInt: [
            (a: bigint, b: bigint) => a + b,
            (a: bigint, b: bigint) => a - b,
        ],
        MyBigInt: [
            MyBigInt.ADD,
            MyBigInt.SUB,
        ],
    };

    bigInt = bigInts[3];
    bigInt2 = bigInts[5];

    for (let i = 0; i < arithmeticOperations.BigInt.length; ++i) {
        console.log(
            arithmeticOperations.MyBigInt[i](
                new MyBigInt(bigInt.toString(HEX_RADIX)),
                new MyBigInt(bigInt2.toString(HEX_RADIX)),
            ).getHex()
            ===
            arithmeticOperations.BigInt[i](bigInt, bigInt2).toString(HEX_RADIX) ?
                SUCCESSFUL_MESSAGE : FAILURE_MESSAGE,
        );
    }
}