/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <ket/libket/label.hpp>
#include <ket/libket/process_exception.hpp>

using namespace ket::libket;
using namespace ket;

label_t::label_t() {}

label_t::label_t(index_t index, index_t process_id) :
    index_{index}, pid{process_id}, valid{true}
    {}

index_t label_t::index() const {
    if (not valid) throw invalid_object();
    return index_;
}

index_t label_t::process_id() const {
    if (not valid) throw invalid_object();
    return pid;
}

