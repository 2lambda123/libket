#pragma once
#include "handler.hpp"

extern "C" ket::base::Handler* ket_handle;
#define ket_init ket::base::Handler* ket_handle

namespace ket {
    void begin(int argc, char* argv[]);
    void end();
}
