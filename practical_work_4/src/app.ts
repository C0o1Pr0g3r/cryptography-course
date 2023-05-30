class NumericalRange {
    private min: number;
    private max: number;

    public constructor(min: number, max: number) {
        this.min = min;
        this.max = max;
    }

    public within(value: number) {
        return value >= this.min && value <= this.max;
    }
}

type Sequence = Uint8Array;
const Sequence = Uint8Array;

const BIN_RADIX = 2;
const BITS_PER_BYTE = 8;
const SEQUENCE_LENGTH = 2500;
const MAXIMUM_LENGTH_OF_SERIES = 36;
const MONOBIT_TEST_RANGE = new NumericalRange(9654, 10346);
const POKER_TEST_RANGE = new NumericalRange(1.03, 57.4);
const SERIES_LENGTH_TEST = new Map([
    ["1", new NumericalRange(2267, 2733)],
    ["2", new NumericalRange(1079, 1421)],
    ["3", new NumericalRange(502, 748)],
    ["4", new NumericalRange(223, 402)],
    ["5", new NumericalRange(90, 223)],
    ["6+", new NumericalRange(90, 223)],
]);

const SUCCESSFUL_MESSAGE = "Test passed";
const FAILURE_MESSAGE = "Test failed";

main();

function main() {
    const sequence = generateSequence();

    console.log(monobitTest(sequence) ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE);
    console.log(longRunTest(sequence) ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE);
    console.log(PokerTest(sequence) ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE);
    console.log(RunsTest(sequence) ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE);
}

function generateSequence() {
    const sequence = new Sequence(SEQUENCE_LENGTH);

    for (let i = 0; i < sequence.length; ++i) {
        sequence[i] = Math.floor(Math.random() * Math.pow(BIN_RADIX, BITS_PER_BYTE));
    }

    return sequence;
}

function monobitTest(sequence: Sequence) {
    let numberOfOnes = 0;

    for (let i = 0; i < sequence.length; ++i) {
        for (let j = 0; j < BITS_PER_BYTE; ++j) {
            if ((sequence[i] >> j & 1) === 1) {
                ++numberOfOnes;
            }
        }
    }

    return MONOBIT_TEST_RANGE.within(numberOfOnes);
}

function longRunTest(sequence: Sequence) {
    const maximumLengthOfSeries: [number, number] = [0, 0];
    let currentLengthOfSeries = 0;
    let currentSeries = 0;

    for (let i = sequence.length - 1; i >= 0; --i) {
        for (let j = 0; j < BITS_PER_BYTE; ++j) {
            const value = sequence[i] >> j & 1;
            if (value === currentSeries) {
                ++currentLengthOfSeries;
            } else {
                if (currentLengthOfSeries > maximumLengthOfSeries[currentSeries]) {
                    maximumLengthOfSeries[currentSeries] = currentLengthOfSeries;
                }
                currentSeries = value;
                currentLengthOfSeries = 1;
            }
        }
    }

    return maximumLengthOfSeries[0] <= MAXIMUM_LENGTH_OF_SERIES
        && maximumLengthOfSeries[1] <= MAXIMUM_LENGTH_OF_SERIES;
}

function PokerTest(sequence: Sequence) {
    const Y = sequence.length * BITS_PER_BYTE;
    const m = 4;
    const k = Math.floor(Y / m);
    const numberOfPatterns = Math.floor(Math.pow(BIN_RADIX, m));
    const numberOfBlockOccurrences = new Array<number>(numberOfPatterns);

    for (let i = 0; i < numberOfBlockOccurrences.length; ++i) {
        numberOfBlockOccurrences[i] = 0;
    }

    for (let i = 0; i < sequence.length; ++i) {
        for (let j = 0; j < Math.floor(BITS_PER_BYTE / m); ++j) {
            ++numberOfBlockOccurrences[sequence[i] >> j * m & numberOfPatterns - 1];
        }
    }

    const sum = numberOfBlockOccurrences.reduce(
        (sum, count) => sum + Math.pow(count, BIN_RADIX), 0,
    );

    const X = numberOfPatterns / k * sum - k;

    return POKER_TEST_RANGE.within(X);
}

function RunsTest(sequence: Sequence) {
    const lengthOfSeries: [Map<string, number>, Map<string, number>] =
        [new Map<string, number>(), new Map<string, number>()];
    let currentLengthOfSeries = 0;
    let currentSeries = 0;

    for (let i = sequence.length - 1; i >= 0; --i) {
        for (let j = 0; j < BITS_PER_BYTE; ++j) {
            const value = sequence[i] >> j & 1;
            if (value === currentSeries) {
                ++currentLengthOfSeries;
            } else {
                if (currentLengthOfSeries > 0) {
                    const key = currentLengthOfSeries < SERIES_LENGTH_TEST.size
                        ? currentLengthOfSeries.toString()
                        : SERIES_LENGTH_TEST.size.toString() + "+";
                    lengthOfSeries[currentSeries].set(
                        key,
                        lengthOfSeries[currentSeries].has(key)
                            ? lengthOfSeries[currentSeries].get(key)! + 1
                            : 1,
                    );
                }
                currentSeries = value;
                currentLengthOfSeries = 1;
            }
        }
    }

    for (let i = 0; i < lengthOfSeries.length; ++i) {
        for (const key of lengthOfSeries[i].keys()) {
            if (!SERIES_LENGTH_TEST.get(key)!.within(lengthOfSeries[i].get(key)!)) {
                return false;
            }
        }
    }

    return true;
}
