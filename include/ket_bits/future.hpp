#pragma once 
#include "qubit_bit.hpp"
#include "lib.hpp"
#include <functional>
#include <memory>

namespace ket {
    template <class T>
    class Future {
     public:
        Future(const Bit &bit) : bit{std::make_shared<Bit>(bit)}, type{MEASURE} {}

        template <class F>
        Future(const Future<T>& left, const Future<T>& right, F op) :
            left{std::make_shared<Future<T>>(left)}, 
            right{std::make_shared<Future<T>>(right)},
            op{op}, type{RESULT} {}

        Future(const T& data) : data{data}, type{VALUE} {}

        operator T() {
            switch (type) {
                case RESULT:
                    data = op(*left, *right);
                    break;

                case MEASURE:
                    data = to<T>(*bit);
                    break;
                
                default:
                    break;
            }
            type = VALUE;
            return data;
        }

        T get() {
            return (T) (*this);
        }

        operator Bit() {
            switch (type) {
                case MEASURE:
                    return *bit;
                
                default:
                    throw std::runtime_error("This ket::Future does not hold a ket::Bit");
            }
        }

     private:
        T data;
        std::shared_ptr<Bit> bit;
        std::shared_ptr<Future<T>> left, right;
        std::function<T (Future<T>&, Future<T>&)> op;
        enum {MEASURE, RESULT, VALUE} type;
    };

    template <class T>
    Future<T> measure(const Qubit& a) {
        return Future<T>{measure(a)};
    }
}