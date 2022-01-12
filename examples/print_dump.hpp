#pragma once 
#include <iostream>
#include <iomanip>

template <int size, class DUMP>
void print_dump(DUMP dump) {
    auto state_it = dump.states().begin();
    auto amp_it = dump.amplitides().begin();
    while (state_it != dump.states().end()) {
        std::cout << std::bitset<size>((*state_it)[0])
                  << " " << (*amp_it)[0] << std::endl;
        ++state_it;
        ++amp_it;
    }
}