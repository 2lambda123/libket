#pragma once
#include "base.hpp"
#include "quant.hpp"
#include "future.hpp"

namespace ket {
    quant alloc(size_t size, process *ps = ket_ps);
    quant alloc_dirty(size_t size, process *ps = ket_ps);
    void free(const quant& q);
    void free_dirty(const quant& q);
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
    void dump(const quant& q);
    
    template <class F, class... Args>
    void ctrl(const quant& q, F gate, Args... args) {
        q.get_ps()->ctrl_begin(q.get_base_qubits());
        gate(args...);
        q.get_ps()->ctrl_end();
    }
    
    template <class F, class... Args>
    void adj(F gate, Args... args, process* ps = ket_ps) {
        ps->adj_begin();
        gate(args...);
        ps->adj_end();
    }
    
    template <class F, class... Args>
    void ctrl_adj(const quant& q, F gate, Args... args) {
        q.get_ps()->ctrl_begin(q.get_base_qubits());
        q.get_ps()->adj_begin();
        gate(args...);
        q.get_ps()->adj_end();
        q.get_ps()->ctrl_end();
    }
}