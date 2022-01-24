/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <ket/quantum_code/quantum_code.hpp>
#include <stack>
#include <string>
#include <vector>
#include <exception>

namespace ket::libket {
    using quantum_code::intruction_t;

    class block_t {
        using instructions_t = std::vector<quantum_code::intruction_t>;
        using adj_instructions_t = std::stack<std::stack<quantum_code::intruction_t>>;

    public:
        void  add_instruction(intruction_t);
        char* add_string(std::string);

        bool adj() const;
        bool not_adj() const;

        void adj_begin();
        void adj_end();

        void end();
        bool ended() const;

        size_t size() const;
        size_t write(quantum_code::block_t* header, char* begin, char* pos) const;

        std::string str() const;

    private:
        void check_ended() const;

        instructions_t     instructions;
        adj_instructions_t adj_instructions;

        std::string args;
        bool ended_ = false;
    };

    class not_in_adj : public std::exception {
    public:
        virtual const char* what() const noexcept;
    };

    class ended_block : public std::exception {
    public:
        virtual const char* what() const noexcept;
    };

} // ket::libket