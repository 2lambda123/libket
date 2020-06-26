/* MIT License
 * 
 * Copyright (c) 2020 Evandro Chagas Ribeiro da Rosa <evandro.crr@posgrad.ufsc.br>
 * Copyright (c) 2020 Rafael de Santiago <r.santiago@ufsc.br>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "../include/ket"
#include <boost/process.hpp>
#include <boost/process/async.hpp>
#include <boost/asio.hpp>
#include <fstream>

using namespace ket;

inline std::string call(const std::string& command, const std::string& in) {
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

void process::exec() {
    
    if (output_kqasm) {
        std::ofstream out{kqasm_path, std::ofstream::app};
        out << kqasm.str() 
            << "=========================" 
            << std::endl;
        out.close();
    }

    if (execute_kqasm) {
        std::stringstream kbw;
        kbw << kbw_path << " -s " << exec_seed++;
        if (use_plugin) kbw << " -p " << plugin_path;

        std::stringstream result{call(kbw.str(), kqasm.str())};

        size_t int_id;
        std::int64_t val;
        while (result >> int_id >> val) {
            auto& future_tmp = measure_map[int_id];
            *(future_tmp.first) = val;
            *(future_tmp.second) = true;
        }
    } else for (auto &i : measure_map) {
        *(i.second.first) =  0;
        *(i.second.second) = true;
    }
}
