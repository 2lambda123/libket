/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <cstdint>
#include <ket/util/types.hpp>

namespace ket::libket {

    class process_t;
    
    enum class int_op_t {
        eq,   neq, gt,   geq,
        lt,   leq, add,  sub,
        mul,  div, sll,  srl,
        and_, or_, xor_, set,
    };

    class future_t {
    public:
        future_t();

        int_t value();
        void  set(future_t);

        index_t index()      const;
        bool    available()  const;
        index_t process_id() const;

        process_t* get_ps() const;

    private:
        void check_valid() const;

        future_t(index_t index, process_t*, index_t process_id, bool_ptr alive);

        index_t index_;

        int_ptr  value_;
        bool_ptr available_;

        index_t pid;
        process_t* ps;
        bool_ptr ps_alive;

        friend class process_t;
    };
} // ket::libket