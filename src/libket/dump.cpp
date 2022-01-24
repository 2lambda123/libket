/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <ket/libket/dump.hpp>
#include <ket/libket/process.hpp>
#include <ket/libket/process_exception.hpp>
#include <boost/make_shared.hpp>

using namespace ket::libket;
using namespace ket;

dump_t::dump_t() {}

dump_t::dump_t(index_t index, process_t* ps, index_t process_id, bool_ptr alive) :
    index_{index}, 
    data{boost::make_shared<dump::dump_t>()}, 
    available_{boost::make_shared<bool>(false)}, 
    ps{ps}, 
    pid{process_id}, 
    ps_alive{alive} 
    {}

void dump_t::run() {
    check_valid();
    if (not *ps_alive) {
        throw dead_process();
    } else {
        ps->run();
    }
}

const dump::states_t& dump_t::states() {
    if (not available()) run();
    return data->first;
}

const dump::amplitudes_t& dump_t::amplitudes() {
    if (not available()) run();
    return data->second;
}

index_t dump_t::index() const {
    check_valid();
    return index_;
}

index_t dump_t::process_id() const {
    check_valid();
    return pid;
}

bool dump_t::available()  const {
    check_valid();
    return *available_;
}

void dump_t::check_valid() const {
    if (not ps_alive) {
        throw invalid_object{};
    }
}