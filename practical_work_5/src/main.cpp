#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <tuple>
#include <iomanip>
#include <chrono>
#include <openssl/sha.h>
#include "own-sha1.h"

using namespace std::chrono;

const size_t HEX_DIGITS_PER_BYTE = 2;

const string SUCCESSFUL_MESSAGE = "Test passed";
const string FAILURE_MESSAGE = "Test failed";

ifstream::pos_type getFileSize(ifstream& file) {
    const ifstream::pos_type initialPosition = file.tellg();
    file.seekg(0, ios_base::end);
    const ifstream::pos_type size = file.tellg();
    file.seekg(initialPosition, std::ios_base::beg);
    return size;
}

string convertHashToHexString(const Hash& hash) {
    stringstream ss;
    for (int i = 0; i < 20; ++i) {
        ss << setfill('0') << setw(HEX_DIGITS_PER_BYTE) << hex << static_cast<Word>(hash[i]);
    }
    return ss.str();
}

vector<string> splitString(const string& str, char delimiter) {
    vector<string> partsOfString;

    size_t beginningOfCurrentPart = 0;
    size_t i = 0;
    while (i < str.length()) {
        if (str[i] == delimiter) {
            partsOfString.push_back(str.substr(beginningOfCurrentPart, i - beginningOfCurrentPart));
            beginningOfCurrentPart = i;
            while (str[i] == delimiter) {
                ++i;
            }
            partsOfString.push_back(str.substr(beginningOfCurrentPart, i - beginningOfCurrentPart));
            beginningOfCurrentPart = i;
        }
        ++i;
    }
    partsOfString.push_back(str.substr(beginningOfCurrentPart, str.length()));

    return partsOfString;
}

tuple<string, size_t> calculateLibraryHashAndMeasureElapsedTime(
    const OwnSHA1::RawMessage& message
) {
    auto start = high_resolution_clock::now();
    Hash hash;
    SHA1(reinterpret_cast<const unsigned char*>(message.data), message.length, hash.data());
    auto end = high_resolution_clock::now();
    return tuple<string, size_t>(convertHashToHexString(hash), (end - start).count());
}

tuple<string, size_t> calculateOwnHashAndMeasureElapsedTime(
    const OwnSHA1::RawMessage& message
) {
    auto start = high_resolution_clock::now();
    OwnSHA1 sha1;
    sha1.update(message);
    const Hash hash = sha1.result();
    auto end = high_resolution_clock::now();
    return tuple<string, size_t>(convertHashToHexString(hash), (end - start).count());
}

tuple<string, size_t> calculateOwnHashAndMeasureElapsedTime(
    const vector<string>& partsOfMessage
) {
    auto start = high_resolution_clock::now();
    OwnSHA1 sha1;
    for (size_t i = 0; i < partsOfMessage.size(); ++i) {
        sha1.update(partsOfMessage[i]);
    }
    const Hash hash = sha1.result();
    auto end = high_resolution_clock::now();
    return tuple<string, size_t>(convertHashToHexString(hash), (end - start).count());
}

void testHashingOfFilesOfDifferentSizes() {
    const size_t NUMBER_OF_TEST_FILES = 3;
    const array<string, 3> filenames {{
        "tests/test1.txt",
        "tests/test2.txt",
        "tests/test3.txt"
    }};

    cout << "1. Testing hashing of files of different sizes." << endl;
    for (size_t i = 0; i < NUMBER_OF_TEST_FILES; ++i) {
        const string filename = filenames[i];
        std::ifstream file(filename);

        if (!file.is_open()) {
            cerr << "Failed to open '" << filename << "' file" << endl;
            continue;
        }

        const ifstream::pos_type length = getFileSize(file);
        Byte* data = new Byte[length];

        file.seekg(0, std::ios_base::beg);

        file.read(reinterpret_cast<char*>(data), length);

        cout << "Test " << i + 1 << ":" << endl << endl;
        array<string, 2> hexHashes;
        array<size_t, 2> elapsedTime;
        std::tie(hexHashes[0], elapsedTime[0]) = calculateLibraryHashAndMeasureElapsedTime(OwnSHA1::RawMessage(data, length));
        std::tie(hexHashes[1], elapsedTime[1]) = calculateOwnHashAndMeasureElapsedTime(OwnSHA1::RawMessage(data, length));
        cout << "\t\tHash" << "\t\t\t\t\t\tCalculation time" << endl;
        cout << "Library\t\t" << hexHashes[0] << "\t" << elapsedTime[0] << endl;
        cout << "Own\t\t" << hexHashes[1] << "\t" << elapsedTime[1] << endl;
        cout <<  (hexHashes[0] ==  hexHashes[1] ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE) << endl;
        cout << endl;

        delete data;
        file.close();
    }
    cout << endl;
}

void testHashingInParts() {
    const string message = "A hash function is any function that can be used to map data of arbitrary size to fixed-size values, though there are some hash functions that support variable length output.";
    const vector<string> partsOfMessage = splitString(message, ' ');

    cout << "2. Testing hash in parts." << endl << endl;

    cout << "Message: '" << message << "'" << endl;
    cout << "Parts of message:" << endl;
    for (const auto& part: partsOfMessage) {
        cout << "'" << part << "'" << endl;
    }
    cout << endl;


    array<string, 2> hexHashes;
    array<size_t, 2> elapsedTime;
    std::tie(hexHashes[0], elapsedTime[0]) = calculateLibraryHashAndMeasureElapsedTime(OwnSHA1::RawMessage(reinterpret_cast<const Byte*>(message.c_str()), message.length()));
    std::tie(hexHashes[1], elapsedTime[1]) = calculateOwnHashAndMeasureElapsedTime(partsOfMessage);
    cout << "\t\tHash" << "\t\t\t\t\t\tCalculation time" << endl;
    cout << "Library\t\t" << hexHashes[0] << "\t" << elapsedTime[0] << endl;
    cout << "Own\t\t" << hexHashes[1] << "\t" << elapsedTime[1] << endl;
    cout <<  (hexHashes[0] ==  hexHashes[1] ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE) << endl;
    cout << endl;
}

using namespace std;

int main() {
    testHashingOfFilesOfDifferentSizes();
    testHashingInParts();

    return 0;
}
