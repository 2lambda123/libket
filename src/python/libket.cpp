/* 
 * Licensed under the Apache License, Version 2.0;
 * Copyright 2022 Evandro Chagas Ribeiro da Rosa
 */
#include <ket/libket/process.hpp>
#include <ket/libket/future_op.hpp>
#include <boost/python.hpp>

using namespace boost::python;
using namespace ket::libket;

qubit_list_t to_qubit_list(list arg) {
    qubit_list_t qubit_list;
    for (auto i = 0u; i < len(arg); i++) {
        qubit_list.push_back(extract<qubit_t>(arg[i]));
    }
    return qubit_list;
}

BOOST_PYTHON_MODULE(_pyket)
{   
    def("to_qubit_list", to_qubit_list);
    class_<qubit_list_t>("qubit_list_t");

    class_<process_t>("process_t", init<ket::index_t>())
        .def("alloc", &process_t::alloc)
        .def("free", &process_t::free)
        .def("gate", &process_t::gate)
        .def("measure", &process_t::measure)
        .def("new_int", &process_t::new_int)
        .def("plugin", &process_t::plugin)
        .def("ctrl_push", &process_t::ctrl_push)
        .def("ctrl_pop", &process_t::ctrl_pop)
        .def("adj_begin", &process_t::adj_begin)
        .def("adj_end", &process_t::adj_end)
        .def("get_label", &process_t::get_label)
        .def("open_block", &process_t::open_block)
        .def("jump", &process_t::jump)
        .def("breach", &process_t::breach)
        .def("dump", &process_t::dump)
        .def("run", &process_t::run)
        .add_property("exec_time", &process_t::exec_time)
    ;

    class_<qubit_t>("qubit_t")
        .add_property("index", &qubit_t::index)
        .add_property("measured", &qubit_t::measured)
        .add_property("allocated", &qubit_t::allocated)
        .add_property("process_id", &qubit_t::process_id)
    ;

    class_<future_t>("future_t")
        .add_property("value", &future_t::value, &future_t::set)
        .add_property("index", &future_t::index)
        .add_property("available", &future_t::available)
        .add_property("process_id", &future_t::process_id)
        .def(self == self)
        .def(self != self)
        .def(self >  self)
        .def(self >= self)
        .def(self <  self)
        .def(self <= self)
        .def(self +  self)
        .def(self -  self)
        .def(self *  self)
        .def(self /  self)
        .def(self << self)
        .def(self >> self)
        .def(self &  self)
        .def(self |  self)
        .def(self ^  self)
        .def(self == int())
        .def(self != int())
        .def(self >  int())
        .def(self >= int())
        .def(self <  int())
        .def(self <= int())
        .def(self +  int())
        .def(self -  int())
        .def(self *  int())
        .def(self /  int())
        .def(self << int())
        .def(self >> int())
        .def(self &  int())
        .def(self |  int())
        .def(self ^  int())
        .def(int() == self)
        .def(int() != self)
        .def(int() >  self)
        .def(int() >= self)
        .def(int() <  self)
        .def(int() <= self)
        .def(int() +  self)
        .def(int() -  self)
        .def(int() *  self)
        .def(int() /  self)
        .def(int() << self)
        .def(int() >> self)
        .def(int() &  self)
        .def(int() |  self)
        .def(int() ^  self)
    ;

    class_<label_t>("label_t")
        .add_property("index", &label_t::index)
        .add_property("process_id", &label_t::process_id)
    ;

    class_<dump_t>("dump_t")
        .def("states", &dump_t::states, return_internal_reference())
        .def("amplitides", &dump_t::amplitides, return_internal_reference())
        .add_property("index", &dump_t::index)
        .add_property("process_id", &dump_t::process_id)
        .add_property("available", &dump_t::available)
    ;
    
    enum_<gate_t>("gate_t")
        .value("pauli_x", gate_t::pauli_x)    
        .value("pauli_y", gate_t::pauli_y)    
        .value("pauli_z", gate_t::pauli_z)
        .value("rotation_x", gate_t::rotation_x)
        .value("rotation_y", gate_t::rotation_y) 
        .value("rotation_z", gate_t::rotation_z)
        .value("hadamard", gate_t::hadamard)   
        .value("phase", gate_t::phase)
    ;

}