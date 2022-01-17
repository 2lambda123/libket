/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <exception>

namespace ket::quantum_code {

    class feature_not_available_error : public std::exception {
    public:
        virtual const char* what() const noexcept;
    };

    class quantum_execution_timeout : public std::exception {
    public:
        virtual const char* what() const noexcept;
    };

    class qubit_number_error : public std::exception {
    public:
        virtual const char* what() const noexcept;
    };


} // ket::quantum_code