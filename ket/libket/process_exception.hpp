/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <exception>

namespace ket::libket {

    class not_allowed_in_ctrl_adj : public std::exception {
    public:
        virtual const char* what() const noexcept;
    };

    class free_qubit : public std::exception {
    public:
        virtual const char* what() const noexcept;
    };

    class process_id_not_match : public std::exception {
    public:
        virtual const char* what() const noexcept;
    };

    class qubit_in_ctrl : public std::exception {
    public:
        virtual const char* what() const noexcept;
    };

    class not_in_ctrl : public std::exception {
    public:
        virtual const char* what() const noexcept;
    };

    class open_ctrl : public std::exception {
    public:
        virtual const char* what() const noexcept;
    };


    class dead_process : public std::exception {
    public:
        virtual const char* what() const noexcept;
    };

    class invalid_object : public std::exception {
    public:
        virtual const char* what() const noexcept;
    };

} // ket::libket