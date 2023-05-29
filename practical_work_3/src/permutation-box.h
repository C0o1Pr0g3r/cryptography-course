#ifndef PERMUTATION_BOX_H_INCLUDED
#define PERMUTATION_BOX_H_INCLUDED

#include <array>
#include "definitions.h"

namespace PermutationBox {
    using namespace std;
    
    using Table = array<Block, BITS_PER_BYTE>;

    const Table table {{5, 7, 2, 3, 6, 1, 0, 4}};

    Block getBitByIndex(const Block& block, size_t index) {
        return block >> index & 1;
    }

    Block setBitByIndex(const Block& originalBlock, const Block& value, size_t index) {
        const Block MASK = ~(1 << index);
        
        return originalBlock & MASK | value << index;
    }

    Table getReverseTable(const Table& table) {
        Table reverseTable;

        for (size_t i = 0; i < table.size(); ++i) {
            reverseTable[table[i]] = i;
        }

        return reverseTable;
    }

    Block convertBlock(const Block& block, const Table& table) {
        Block result = 0;

        for (size_t i = 0; i < BITS_PER_BYTE; ++i) {
            result = setBitByIndex(result, getBitByIndex(block, i), table[i]);
        }

        return result;
    }

    Block encrypt(const Block& block) {
        return convertBlock(block, table);
    }

    Block decrypt(const Block& block) {
        return convertBlock(block, getReverseTable(table));
    }
}

#endif // PERMUTATION_BOX_H_INCLUDED
