#include "../include/ket_bits/macros.hpp"
#include "../include/ket_bits/future.hpp"
#include <boost/process.hpp>
#include <boost/process/async.hpp>
#include <boost/asio.hpp>
#include <iostream>

using namespace ket;

future::future(const std::shared_ptr<base::i64>& bits) : 
    bits{bits} 
    {}

const std::shared_ptr<base::i64>& future::get_base_i64() const {
    return bits;
}

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

std::int64_t future::get() {
    if (not bits->has_value()) {
        auto next_label = "entry"+std::to_string(ket_hdl->get_label_count());
        ket_hdl->end_block(next_label);
        std::stringstream ss;
        bits->eval(ss);
        ket_hdl->begin_block(next_label);
        
        if (ket_out) {
            std::ofstream out{ket_kqasm_path, std::ofstream::app};
            out << ss.str();
            out.close();
        }

        if (not ket_no_execute) {
            std::stringstream kbw;
            kbw << ket_kbw_path << " -s" << ket_seed++;
            std::stringstream result{call(kbw.str(), ss.str())};

            size_t i64_idx;
            std::int64_t val;
            while (result >> i64_idx >> val) ket_hdl->set_value(i64_idx, val);
            
        } else {
            bits->set_value(0);
        }
    }

    return bits->get_value();
}
