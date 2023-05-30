#include <iostream>
#include <variant>
#include "encryption-and-decryption-library.h"

using namespace std;

const string SUCCESSFUL_MESSAGE = "Test passed";
const string FAILURE_MESSAGE = "Test failed";

int main() {
    const array<SubstitutionOrPermutationBox, 2> boxes {
        SubstitutionOrPermutationBox(
            SubstitutionBox::encrypt, SubstitutionBox::decrypt
        ),
        SubstitutionOrPermutationBox(
            PermutationBox::encrypt, PermutationBox::decrypt
        )
    };

    const Number num = 79834852;
    const String str = "SP-network is a series of linked mathematical operations";
    array<variant<Number, String>, 2> data {
        variant<Number, String>(num),
        variant<Number, String>(str)
    };

    for (const auto& box: boxes) {
        for (const auto& elem: data) {
            if (get_if<Number>(&elem)) {
                cout << "Initial: " << get<Number>(elem) << endl;
                cout << "Encrypted: " << box.encrypt(get<Number>(elem)) << endl;
                cout << "Decrypted: " << box.decrypt(box.encrypt(get<Number>(elem))) << endl;
                cout << (get<Number>(elem) == box.decrypt(box.encrypt(get<Number>(elem))) ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE) << endl;
            } else {
                cout << "Initial: " << get<String>(elem) << endl;
                cout << "Encrypted: " << box.encrypt(get<String>(elem)) << endl;
                cout << "Decrypted: " << box.decrypt(box.encrypt(get<String>(elem))) << endl;
                cout << (get<String>(elem) == box.decrypt(box.encrypt(get<String>(elem))) ? SUCCESSFUL_MESSAGE : FAILURE_MESSAGE) << endl;
            }
            cout << endl;
        }
    }

    return 0;
}
