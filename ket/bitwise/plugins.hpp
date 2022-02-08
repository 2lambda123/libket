/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <ket/bitwise/kbw.hpp>

namespace ket {
namespace bitwise {
    extern "C" void pown(quantum_space_t*, qubit_list_t, char*, ctrl_list_t, bool);
}} // ket::bitwise
