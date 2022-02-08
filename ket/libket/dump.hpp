/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <ket/util/types.hpp>

namespace ket {
namespace libket {

    class process_t;

    class dump_t {
    public:
        dump_t();
        
        const dump::states_t& states() ;
        const dump::amplitudes_t& amplitudes();

        index_t index()      const;
        index_t process_id() const;
        bool    available()  const;

    private:
        void check_valid() const;
        void run();

        dump_t(index_t index, process_t*, index_t process_id, bool_ptr alive);

        index_t index_;

        dump::dump_ptr data;
        bool_ptr available_;

        process_t* ps;
        index_t pid;
        bool_ptr ps_alive;

        friend class process_t;
    };

}} // ket::libket