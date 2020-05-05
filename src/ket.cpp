#include "../include/ket"
#include "../include/ket_bits/quant.hpp"
#include "../include/ket_bits/future.hpp"
#include "../include/ket_bits/base.hpp"

using namespace ket;

quant::quant(const std::shared_ptr<void> &quant_ptr) : quant_ptr{quant_ptr} {}

quant quant::operator()(size_t idx) const {
    auto *ptr = new ket::_quant{(*static_cast<ket::_quant*>(quant_ptr.get()))(idx)};
    return quant{{ptr, [](auto ptr){ delete static_cast<_quant*>(ptr);}}};
}

quant quant::operator|(quant other) const {
    auto *ptr = new ket::_quant{(*static_cast<ket::_quant*>(quant_ptr.get()))|(*static_cast<ket::_quant*>(other.quant_ptr.get()))};
    return quant{{ptr, [](auto ptr){ delete static_cast<_quant*>(ptr);}}};
}

quant quant::invert() const {
    auto *ptr = new ket::_quant{static_cast<ket::_quant*>(quant_ptr.get())->invert()};
    return quant{{ptr, [](auto ptr){ delete static_cast<_quant*>(ptr);}}};
}

size_t quant::len() const {
    return static_cast<ket::_quant*>(quant_ptr.get())->len();
}

future::future(const std::shared_ptr<void> &future_ptr) : future_ptr{future_ptr} {}

int64_t future::get() {
    return static_cast<ket::_future*>(future_ptr.get())->get();
}

process::process() : ps{new base::_process, [](auto ptr){ delete static_cast<base::_process*>(ptr); }} {} 

label::label(const std::string& label_name, process& _ps) : ps{_ps}, name{label_name+std::to_string(count++)} {} 

label::operator std::string() const {
    return name;
}

void label::begin() {
    auto psb = static_cast<base::_process*>(ps.ps.get());
    psb->begin_block(name);
}

void ket::jump(label& label_name) {
    auto ps = static_cast<base::_process*>(label_name.ps.ps.get());
    ps->end_block(label_name);
}

void ket::branch(future _cond, label& label_true, label& label_false) {
    auto ps = static_cast<base::_process*>(label_true.ps.ps.get());
    auto cond = static_cast<ket::_future*>(_cond.future_ptr.get());

    ps->end_block(label_true, label_false, cond->get_base_i64());
}

void ket::ctrl_begin(quant _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    q->get_ps()->ctrl_begin(q->get_base_qubits());
}

void ket::ctrl_end(quant _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    q->get_ps()->ctrl_end();
}

void ket::adj_begin(process &_ps) {
    auto ps = static_cast<base::_process*>(_ps.ps.get());
    ps->adj_begin();
}

void ket::adj_end(process &_ps) {
    auto ps = static_cast<base::_process*>(_ps.ps.get());
    ps->adj_begin();
}

void ket::ctrl_adj_begin(quant _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    q->get_ps()->ctrl_begin(q->get_base_qubits());
    q->get_ps()->adj_begin();
}

void ket::ctrl_adj_end(quant _q) {
    auto *q = static_cast<_quant*>(_q.quant_ptr.get());
    q->get_ps()->adj_end();
    q->get_ps()->ctrl_end();
}