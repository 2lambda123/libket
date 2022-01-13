/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <ket/libket/future_op.hpp>
#include <ket/libket/process.hpp>

using namespace ket::libket;
using namespace ket;

future_t libket::operator==(future_t a, future_t b) {
    return a.get_ps()->int_op(int_op_t::eq, a, b);
}

future_t libket::operator!=(future_t a, future_t b) {
    return a.get_ps()->int_op(int_op_t::neq, a, b);
}

future_t libket::operator> (future_t a, future_t b) {
    return a.get_ps()->int_op(int_op_t::gt, a, b);
}

future_t libket::operator>=(future_t a, future_t b) {
    return a.get_ps()->int_op(int_op_t::geq, a, b);
}

future_t libket::operator< (future_t a, future_t b) {
    return a.get_ps()->int_op(int_op_t::lt, a, b);
}

future_t libket::operator<=(future_t a, future_t b) {
    return a.get_ps()->int_op(int_op_t::leq, a, b);
}

future_t libket::operator+ (future_t a, future_t b) {
    return a.get_ps()->int_op(int_op_t::add, a, b);
}

future_t libket::operator- (future_t a, future_t b) {
    return a.get_ps()->int_op(int_op_t::sub, a, b);
}

future_t libket::operator* (future_t a, future_t b) {
    return a.get_ps()->int_op(int_op_t::mul, a, b);
}

future_t libket::operator/ (future_t a, future_t b) {
    return a.get_ps()->int_op(int_op_t::div, a, b);
}

future_t libket::operator<<(future_t a, future_t b) {
    return a.get_ps()->int_op(int_op_t::sll, a, b);
}

future_t libket::operator>>(future_t a, future_t b) {
    return a.get_ps()->int_op(int_op_t::srl, a, b);
}

future_t libket::operator& (future_t a, future_t b) {
    return a.get_ps()->int_op(int_op_t::and_, a, b);
}

future_t libket::operator| (future_t a, future_t b) {
    return a.get_ps()->int_op(int_op_t::or_, a, b);
}

future_t libket::operator^ (future_t a, future_t b) {
    return a.get_ps()->int_op(int_op_t::xor_, a, b);
}

future_t libket::operator==(future_t a, long b) {
    return a.get_ps()->int_op(int_op_t::eq, a, a.get_ps()->new_int(b));
}

future_t libket::operator!=(future_t a, long b) {
    return a.get_ps()->int_op(int_op_t::neq, a, a.get_ps()->new_int(b));
}

future_t libket::operator> (future_t a, long b) {
    return a.get_ps()->int_op(int_op_t::gt, a, a.get_ps()->new_int(b));
}

future_t libket::operator>=(future_t a, long b) {
    return a.get_ps()->int_op(int_op_t::geq, a, a.get_ps()->new_int(b));
}

future_t libket::operator< (future_t a, long b) {
    return a.get_ps()->int_op(int_op_t::lt, a, a.get_ps()->new_int(b));
}

future_t libket::operator<=(future_t a, long b) {
    return a.get_ps()->int_op(int_op_t::leq, a, a.get_ps()->new_int(b));
}

future_t libket::operator+ (future_t a, long b) {
    return a.get_ps()->int_op(int_op_t::add, a, a.get_ps()->new_int(b));
}

future_t libket::operator- (future_t a, long b) {
    return a.get_ps()->int_op(int_op_t::sub, a, a.get_ps()->new_int(b));
}

future_t libket::operator* (future_t a, long b) {
    return a.get_ps()->int_op(int_op_t::mul, a, a.get_ps()->new_int(b));
}

future_t libket::operator/ (future_t a, long b) {
    return a.get_ps()->int_op(int_op_t::div, a, a.get_ps()->new_int(b));
}

future_t libket::operator<<(future_t a, long b) {
    return a.get_ps()->int_op(int_op_t::sll, a, a.get_ps()->new_int(b));
}

future_t libket::operator>>(future_t a, long b) {
    return a.get_ps()->int_op(int_op_t::srl, a, a.get_ps()->new_int(b));
}

future_t libket::operator& (future_t a, long b) {
    return a.get_ps()->int_op(int_op_t::and_, a, a.get_ps()->new_int(b));
}

future_t libket::operator| (future_t a, long b) {
    return a.get_ps()->int_op(int_op_t::or_, a, a.get_ps()->new_int(b));
}

future_t libket::operator^ (future_t a, long b) {
    return a.get_ps()->int_op(int_op_t::xor_, a, a.get_ps()->new_int(b));
}

future_t libket::operator==(long a, future_t b) {
    return b.get_ps()->int_op(int_op_t::eq, b.get_ps()->new_int(a), b);
}

future_t libket::operator!=(long a, future_t b) {
    return b.get_ps()->int_op(int_op_t::neq, b.get_ps()->new_int(a), b);
}

future_t libket::operator> (long a, future_t b) {
    return b.get_ps()->int_op(int_op_t::gt, b.get_ps()->new_int(a), b);
}

future_t libket::operator>=(long a, future_t b) {
    return b.get_ps()->int_op(int_op_t::geq, b.get_ps()->new_int(a), b);
}

future_t libket::operator< (long a, future_t b) {
    return b.get_ps()->int_op(int_op_t::lt, b.get_ps()->new_int(a), b);
}

future_t libket::operator<=(long a, future_t b) {
    return b.get_ps()->int_op(int_op_t::leq, b.get_ps()->new_int(a), b);
}

future_t libket::operator+ (long a, future_t b) {
    return b.get_ps()->int_op(int_op_t::add, b.get_ps()->new_int(a), b);
}

future_t libket::operator- (long a, future_t b) {
    return b.get_ps()->int_op(int_op_t::sub, b.get_ps()->new_int(a), b);
}

future_t libket::operator* (long a, future_t b) {
    return b.get_ps()->int_op(int_op_t::mul, b.get_ps()->new_int(a), b);
}

future_t libket::operator/ (long a, future_t b) {
    return b.get_ps()->int_op(int_op_t::div, b.get_ps()->new_int(a), b);
}

future_t libket::operator<<(long a, future_t b) {
    return b.get_ps()->int_op(int_op_t::sll, b.get_ps()->new_int(a), b);
}

future_t libket::operator>>(long a, future_t b) {
    return b.get_ps()->int_op(int_op_t::srl, b.get_ps()->new_int(a), b);
}

future_t libket::operator& (long a, future_t b) {
    return b.get_ps()->int_op(int_op_t::and_, b.get_ps()->new_int(a), b);
}

future_t libket::operator| (long a, future_t b) {
    return b.get_ps()->int_op(int_op_t::or_, b.get_ps()->new_int(a), b);
}

future_t libket::operator^ (long a, future_t b) {
    return b.get_ps()->int_op(int_op_t::xor_, b.get_ps()->new_int(a), b);
}

