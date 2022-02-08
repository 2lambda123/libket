/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <ket/util/types.hpp>

namespace ket {
namespace libket {

    class label_t {
    public:
        label_t();

        index_t index() const;
        index_t process_id() const;

    private:
        label_t(index_t index, index_t process_id);

        index_t index_;
        index_t pid;
        
        bool valid = false;

        friend class process_t;
    };

}} // ket::libket