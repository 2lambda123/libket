/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#pragma once
#include <ket/libket/future.hpp>

namespace ket::libket { 
    future_t operator==(future_t a, future_t b);
    future_t operator!=(future_t a, future_t b);
    future_t operator> (future_t a, future_t b);
    future_t operator>=(future_t a, future_t b);
    future_t operator< (future_t a, future_t b);
    future_t operator<=(future_t a, future_t b);
    future_t operator+ (future_t a, future_t b);
    future_t operator- (future_t a, future_t b);
    future_t operator* (future_t a, future_t b);
    future_t operator/ (future_t a, future_t b);
    future_t operator<<(future_t a, future_t b);
    future_t operator>>(future_t a, future_t b);
    future_t operator& (future_t a, future_t b);
    future_t operator| (future_t a, future_t b);
    future_t operator^ (future_t a, future_t b);
    future_t operator==(future_t a, long b);
    future_t operator!=(future_t a, long b);
    future_t operator> (future_t a, long b);
    future_t operator>=(future_t a, long b);
    future_t operator< (future_t a, long b);
    future_t operator<=(future_t a, long b);
    future_t operator+ (future_t a, long b);
    future_t operator- (future_t a, long b);
    future_t operator* (future_t a, long b);
    future_t operator/ (future_t a, long b);
    future_t operator<<(future_t a, long b);
    future_t operator>>(future_t a, long b);
    future_t operator& (future_t a, long b);
    future_t operator| (future_t a, long b);
    future_t operator^ (future_t a, long b);
    future_t operator==(long a, future_t b);
    future_t operator!=(long a, future_t b);
    future_t operator> (long a, future_t b);
    future_t operator>=(long a, future_t b);
    future_t operator< (long a, future_t b);
    future_t operator<=(long a, future_t b);
    future_t operator+ (long a, future_t b);
    future_t operator- (long a, future_t b);
    future_t operator* (long a, future_t b);
    future_t operator/ (long a, future_t b);
    future_t operator<<(long a, future_t b);
    future_t operator>>(long a, future_t b);
    future_t operator& (long a, future_t b);
    future_t operator| (long a, future_t b);
    future_t operator^ (long a, future_t b);
} // ket::libket