/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <ket/libket/qubit.hpp>
#include <ket/libket/process_exception.hpp>
#include <boost/smart_ptr/make_shared.hpp>

using namespace ket::libket;
using namespace ket;

qubit_t::qubit_t() {}

qubit_t::qubit_t(index_t index, index_t process_id) :
    index_{index}, measured_{boost::make_shared<bool>(false)}, allocated_{boost::make_shared<bool>(true)}, pid{process_id}
    {}

index_t qubit_t::index() const{
    check_valid();
    return index_;
}

bool qubit_t::measured() const{
    check_valid();
    return *measured_;
}

bool qubit_t::allocated() const{
    check_valid();
    return *allocated_;
}

index_t qubit_t::process_id() const{
    check_valid();
    return pid;
}

void qubit_t::check_valid() const {
    if (not allocated_) {
        throw invalid_object{};
    }
}
