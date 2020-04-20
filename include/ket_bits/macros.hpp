#pragma once
#include "base.hpp"

extern "C" ket::process* ket_hdl;
#define ket_init ket::process* ket_hdl

#define IF(cond) ket_hdl->if_then(cond.get_base_i64(),
#define THEN(...) [&] { __VA_ARGS__ } 
#define END )
#define ELSE(...) , [&] { __VA_ARGS__ });

namespace ket {
    void begin();
    void end();
    
    template <class F, class... Args>
    auto run(F func, Args... args) {
        auto *backup = ket_hdl;
        begin();
        auto ret =  func(args...);
        end();
        ket_hdl = backup;
        return ret;
    }

}