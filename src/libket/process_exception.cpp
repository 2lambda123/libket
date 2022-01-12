/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <ket/libket/process_exception.hpp>

using namespace ket::libket;

const char* not_allowed_in_ctrl_adj::what() const noexcept {
    return "operation not allowed inside ctrl or adj scope";
}

const char* free_qubit::what() const noexcept {
    return "trying to operate with free qubit";
}

const char* process_id_not_match::what() const noexcept {
    return "trying to operate with object from another process";
}

const char* qubit_in_ctrl::what() const noexcept {
    return "trying to operate with qubit set as controll";
}

const char* open_ctrl::what() const noexcept {
    return "trying close adj scope with open ctrl scope within";
}


const char* not_in_ctrl::what() const noexcept {
    return "trying to end an nonexistent ctrl scope";
}

const char* dead_process::what() const noexcept {
    return "trying to operate with deallocated process";
}

const char* invalid_object::what() const noexcept {
    return "this object does not have a valid process associated with it";
}

