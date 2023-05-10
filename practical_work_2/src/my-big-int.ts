const BITS_PER_BYTE = 8;
const BYTES_PER_BLOCK = 4;
const MAX_NUMBER_STORED_IN_BLOCK = Math.pow(2, BITS_PER_BYTE * BYTES_PER_BLOCK);
const HEX_DIGITS_PER_BYTE = 2;
const BIN_RADIX = 2;
const HEX_RADIX = 16;
const HEX_DIGITS = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f"];

export enum BitShift {
    LEFT,
    RIGHT
}

export class MyBigInt {
    private buffer!: Uint32Array;

    public constructor();
    public constructor(value: string);
    public constructor(blockLength: number);
    public constructor(blocks: number[]);
    public constructor(valueOrBlockLengthOrBlocks: string | number | number[] = 1) {
        if (typeof valueOrBlockLengthOrBlocks === "string") {
            this.setHex(valueOrBlockLengthOrBlocks);
        } else if (typeof valueOrBlockLengthOrBlocks === "number") {
            this.buffer = new Uint32Array(valueOrBlockLengthOrBlocks);
        } else if (
            typeof valueOrBlockLengthOrBlocks === "object"
            &&
            valueOrBlockLengthOrBlocks instanceof Array
        ) {
            this.buffer = new Uint32Array(valueOrBlockLengthOrBlocks);
        } else {
            throw new Error("Wrong type argument passed");
        }
    }

    get lengthInBytes() {
        return this.buffer.byteLength;
    }

    get lengthInBlocks() {
        return this.buffer.length;
    }

    public setHex(hexNumber = "") {
        if (!isValidHexNumber(hexNumber)) {
            throw new Error(`'${hexNumber}' is an invalid hexadecimal number`);
        }

        this.buffer = new Uint32Array(
            Math.ceil(Math.ceil(hexNumber.length / HEX_DIGITS_PER_BYTE) / BYTES_PER_BLOCK),
        );

        const hexDigits = hexNumber.split("");

        const numberOfLeadingZeros = hexDigits.length % (HEX_DIGITS_PER_BYTE * BYTES_PER_BLOCK) === 0
            ?
            0
            :
            (HEX_DIGITS_PER_BYTE * BYTES_PER_BLOCK) - hexDigits.length % (HEX_DIGITS_PER_BYTE * BYTES_PER_BLOCK);

        for (let i = 0; i < numberOfLeadingZeros; ++i) {
            hexDigits.unshift("0");
        }

        for (let i = 0; i < this.buffer.length; ++i) {
            this.buffer[i] = parseInt(hexDigits.slice(
                i * BYTES_PER_BLOCK * HEX_DIGITS_PER_BYTE, (i + 1) * BYTES_PER_BLOCK * HEX_DIGITS_PER_BYTE,
            ).join(""), HEX_RADIX);
        }
    }

    public getHex() {
        const hexDigits: string[] = [];

        for (let i = 0; i < this.buffer.length; ++i) {
            const blockOfHexDigits = this.buffer[i].toString(HEX_RADIX).split("");
            addLeadingZeros(blockOfHexDigits);
            for (const hexDigit of blockOfHexDigits) {
                hexDigits.push(hexDigit);
            }
        }

        while (hexDigits[0] === "0") {
            hexDigits.shift();
        }

        if (hexDigits.length === 0) {
            hexDigits.push("0");
        }

        return hexDigits.join("");
    }

    static INV(value: MyBigInt) {
        const result = new MyBigInt(value.lengthInBlocks);

        for (let i = 0; i < value.buffer.length; ++i) {
            result.buffer[i] = ~value.buffer[i];
        }

        return result;
    }

    public static XOR(first: MyBigInt, second: MyBigInt) {
        return MyBigInt.performBinaryBitwiseLogicalOperation(XOR, first, second);
    }

    public static OR(first: MyBigInt, second: MyBigInt) {
        return MyBigInt.performBinaryBitwiseLogicalOperation(OR, first, second);
    }

    public static AND(first: MyBigInt, second: MyBigInt) {
        return MyBigInt.performBinaryBitwiseLogicalOperation(AND, first, second);
    }

    public static shiftR(value: MyBigInt, size: number) {
        return MyBigInt.performBitwiseShift(value, size, BitShift.RIGHT);
    }

    public static shiftL(value: MyBigInt, size: number) {
        return MyBigInt.performBitwiseShift(value, size, BitShift.LEFT);
    }

    public static ADD(first: MyBigInt, second: MyBigInt) {
        const largerBuffer = first.buffer.length > second.buffer.length ? first.buffer : second.buffer;
        const smallerBuffer = largerBuffer === first.buffer ? second.buffer : first.buffer;
        const offset = largerBuffer.length - smallerBuffer.length;
        const results = new Array<number>(largerBuffer.length);

        let i = largerBuffer.length - 1;
        let carry_digit = 0;
        for (; i >= 0; --i)
        {
            const sum = i >= offset
                ?
                largerBuffer[i] + smallerBuffer[i - offset] + carry_digit
                :
                largerBuffer[i] + carry_digit;
            carry_digit = Math.floor(sum / MAX_NUMBER_STORED_IN_BLOCK);
            results[i] = sum % MAX_NUMBER_STORED_IN_BLOCK;
        }

        if (carry_digit > 0) {
            results.unshift(carry_digit);
        }

        return new MyBigInt(results);
    }

    public static SUB(first: MyBigInt, second: MyBigInt) {
        if (first.lessThan(second)) {
            throw new Error("Subtracting a larger number from a smaller one is not allowed.");
        }

        const offset = first.buffer.length - second.buffer.length;

        const results = new Array<number>(first.buffer.length);

        let i = first.buffer.length - 1;
        for (; i >= 0; --i)
        {
            if (i >= offset) {
                if (first.buffer[i] < second.buffer[i - offset]) {
                    --first.buffer[i - 1];
                    first.buffer[i] += Math.pow(BIN_RADIX, BITS_PER_BYTE * BYTES_PER_BLOCK);
                }
                const difference = first.buffer[i] - second.buffer[i - offset];
                results[i] = difference;
            } else {
                results[i] = first.buffer[i];
            }
        }

        return new MyBigInt(results);
    }

    public lessThan(other: MyBigInt) {
        if (this.buffer.length < other.buffer.length) {
            return true;
        }

        const offset = this.buffer.length - other.buffer.length;

        for (let i = 0; i < this.buffer.length; ++i) {
            if (this.buffer[i + offset] > other.buffer[i]) {
                return false;
            }
        }

        return this.buffer[this.buffer.length - 1] < other.buffer[other.buffer.length - 1];
    }

    public toString() {
        return this.getHex();
    }

    private static performBinaryBitwiseLogicalOperation(
        operation: (first: number, second: number) => number,
        first: MyBigInt, second: MyBigInt,
    ) {
        const largerBuffer = first.buffer.length > second.buffer.length ? first.buffer : second.buffer;
        const smallerBuffer = largerBuffer === first.buffer ? second.buffer : first.buffer;
        const offset = largerBuffer.length - smallerBuffer.length;
        const result = new MyBigInt(largerBuffer.length);

        for (let i = 0; i < largerBuffer.length; ++i) {
            if (i < offset) {
                result.buffer[i] = operation(largerBuffer[i], 0);
            } else {
                result.buffer[i] = operation(largerBuffer[i], smallerBuffer[i - offset]);
            }
        }

        return result;
    }

    private static performBitwiseShift(value: MyBigInt, size: number, where: BitShift) {
        const binaryDigits = parseInt(value.getHex(), HEX_RADIX).toString(BIN_RADIX).split("");

        for (let i = 0; i < size; ++i) {
            if (where === BitShift.LEFT) {
                binaryDigits.push("0");
            } else if (where === BitShift.RIGHT) {
                binaryDigits.pop();
                binaryDigits.unshift("0");
            }
        }
        
        return new MyBigInt(parseInt(binaryDigits.join(""), BIN_RADIX).toString(HEX_RADIX));
    }
}

function isValidHexNumber(value: string) {
    for (const hexDigit of value) {
        if (!HEX_DIGITS.includes(hexDigit.toLowerCase())) {
            return false;
        }
    }

    return true;
}

function addLeadingZeros(blockOfHexDigits: string[]) {
    const count = HEX_DIGITS_PER_BYTE * BYTES_PER_BLOCK - blockOfHexDigits.length;
    for (let i = 0; i < count; ++i) {
        blockOfHexDigits.unshift("0");
    }
}

function XOR(first: number, second: number) {
    return first ^ second;
}

function OR(first: number, second: number) {
    return first | second;
}

function AND(first: number, second: number) {
    return first & second;
}
