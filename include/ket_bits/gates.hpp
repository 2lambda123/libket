#pragma once
#include "base.hpp"
#include "quant.hpp"
#include "future.hpp"

namespace ket {
    future measure(const quant& q, bool wait = false);
    void wait(const quant& q);
    void x(const quant& q);
    void y(const quant& q);
    void z(const quant& q);
    void h(const quant& q);
    void s(const quant& q);
    void t(const quant& q);
    void u1(double lambda, const quant& q);
    void u2(double phi, double lambda, const quant& q);
    void u3(double theta, double phi, double lambda, const quant& q);

    template <class F, class... Args>
    void ctrl(const quant& q, F gate, Args... args) {
        ket_hdl.ctrl_begin(q.get_base_qubits());
        gate(args...);
        ket_hdl.ctrl_end();
    }
    
    template <class F, class... Args>
    void adj(F gate, Args... args) {
        ket_hdl.adj_begin();
        gate(args...);
        ket_hdl.adj_end();
    }
    
    template <class F, class... Args>
    void ctrl_adj(const quant& q, F gate, Args... args) {
        ket_hdl.ctrl_begin(q.get_base_qubits());
        ket_hdl.adj_begin();
        gate(args...);
        ket_hdl.adj_end();
        ket_hdl.ctrl_end();
    }
}