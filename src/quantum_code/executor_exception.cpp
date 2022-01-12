/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <ket/quantum_code/execution_exception.hpp>

using namespace ket::quantum_code;

const char* feature_not_available_error::what() const noexcept {
    return "backend missing one or more features needed for the quantum execution";
}

const char* quantum_execution_timeout::what() const noexcept {
    return "quantum execution timeout";
}