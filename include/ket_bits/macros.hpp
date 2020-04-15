#pragma once
#include "base.hpp"

extern "C" ket::base::handler ket_hdl;
#define ket_init ket::base::handler ket_hdl
#define ket_main(...) ket_init; int main() { ket::begin(); __VA_ARGS__ ket::end();} 

#define IF(cond) ket_hdl.if_then(cond.get_base_i64(),
#define THEN(...) [&] { __VA_ARGS__ } 
#define END )
#define ELSE(...) , [&] { __VA_ARGS__ });

namespace ket {
    void begin();
    void end();
}