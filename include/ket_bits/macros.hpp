#pragma once
#include "base.hpp"

extern "C" ket::process* ket_hdl;
extern "C" char* ket_kbw_path;
extern "C" int ket_no_execute;
extern "C" size_t ket_seed;
extern "C" char* ket_kqasm_path;
extern "C" int ket_out;              
extern "C" char* ket_plugin_path;
#define ket_init ket::process* ket_hdl = nullptr; char* ket_kbw_path = nullptr; int ket_no_execute; size_t ket_seed; char* ket_kqasm_path = nullptr; int ket_out; char* ket_plugin_path = nullptr


#define IF(cond) ket_hdl->if_then(cond.get_base_i64(),
#define THEN(...) [&] { __VA_ARGS__ } 
#define END )
#define ELSE(...) , [&] { __VA_ARGS__ });

namespace ket {
    void begin(int argc, char* argv[]);
    void end();
    
    template <class F, class... Args>
    auto run(F func, Args... args) {
        auto *backup = ket_hdl;
        ket_hdl = new ket::process;
        ket_hdl->begin_block("entry");
        auto ret =  func(args...);
        delete ket_hdl;
        ket_hdl = backup;
        return ret;
    }

}