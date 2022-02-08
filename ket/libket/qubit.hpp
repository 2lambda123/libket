/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <cstdint>
#include <ket/util/types.hpp>
#include <boost/shared_ptr.hpp>

namespace ket {
namespace libket {

    class qubit_t {
    public:
        qubit_t();

        index_t index() const;
        bool    measured() const;
        bool    allocated() const;
        index_t process_id() const;

    private:
        void check_valid() const;

        qubit_t(index_t index, index_t process_id);

        index_t index_;
        bool_ptr measured_;
        bool_ptr allocated_;

        index_t pid;
        friend class process_t;
    };

}} // ket::libket