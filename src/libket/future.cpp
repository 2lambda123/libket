/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <ket/libket/future.hpp>
#include <ket/libket/process.hpp>
#include <ket/libket/process_exception.hpp>
#include <boost/smart_ptr/make_shared.hpp>

using namespace ket::libket;
using namespace ket;

future_t::future_t() {}

future_t::future_t(index_t index, process_t* ps, index_t process_id, bool_ptr alive) :
    index_{index}, value_{boost::make_shared<int_t>(0)}, available_{boost::make_shared<bool>(false)}, pid{process_id}, ps{ps}, ps_alive{alive} 
    {}

int_t future_t::value() {
    check_valid();

    if (not available()) {
        if (not *ps_alive) {
            throw dead_process{};
        } else {
            ps->run();
        }
    }
    return *value_;
}

void future_t::set(future_t future) {
    check_valid();

    if (not *ps_alive) {
        throw dead_process{};
    } else {
        ps->int_set(*this, future);
    }
}


index_t future_t::index() const {
    check_valid();
    return index_;
}

bool future_t::available() const {
    check_valid();
    return *available_;
}

index_t future_t::process_id() const {
    check_valid();
    return pid;
}

void future_t::check_valid() const {
    if (not ps_alive) {
        throw invalid_object{};
    }
}

process_t* future_t::get_ps() const {
    check_valid();
    return ps;
}

