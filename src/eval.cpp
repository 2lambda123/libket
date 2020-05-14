#include "../include/ket_bits/base.hpp"
#include "../include/ket"
#include <boost/process.hpp>
#include <boost/process/async.hpp>
#include <boost/asio.hpp>
#include <iostream>

using namespace ket::base;

std::string call(const std::string& command, const std::string& in) {
    boost::asio::io_service ios;

    std::future<std::string> outdata;

    boost::process::async_pipe qasm(ios);
    boost::process::child c(command+std::string{},
                            boost::process::std_out > outdata,
                            boost::process::std_in < qasm, ios);

    boost::asio::async_write(qasm, boost::process::buffer(in),
                            [&](boost::system::error_code, size_t) { qasm.close(); });

    ios.run();

    return outdata.get();
}

void _process::eval() {
    end_block("end");

    std::stringstream ss;
    for (auto &i : measurement_map) 
        i.second->eval(ss);

    if (ket_kqasm_path) {
        std::ofstream out{ket_kqasm_path, std::ofstream::app};
        out << ss.str() 
            << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - -" 
            << std::endl;
        out.close();
    }

    if (not ket_no_execute) {
        std::stringstream kbw;
        kbw << ket_kbw_path << " -s " << ket_seed++;
        if (ket_plugin_path) kbw << " -p " << ket_plugin_path;
        std::stringstream result{call(kbw.str(), ss.str())};

        size_t i64_idx;
        std::int64_t val;
        while (result >> i64_idx >> val) measurement_map[i64_idx]->set_value(val);
    } else for (auto &i : measurement_map) {
        i.second->set_value(0);
    }
        
}