/* Copyright 2020, 2021 Evandro Chagas Ribeiro da Rosa <evandro.crr@posgrad.ufsc.br>
 * Copyright 2020, 2021 Rafael de Santiago <r.santiago@ufsc.br>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../include/ket"
#include <algorithm>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/serialization/complex.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <cmath>
#include <fstream>

using namespace ket;

template <class T>
inline boost::array<char, sizeof(T)> to_char(T value) {
    boost::array<char, sizeof(T)> buffer;
    std::memcpy(&buffer[0], &value, sizeof(T));
    return buffer;
}

template <class T>
inline T from_char(boost::array<char, sizeof(T)> buffer) {
    T value;
    std::memcpy(&value, &buffer[0], sizeof(T));
    return value;
}

inline auto conect(const std::string& addr, int port) {
    boost::asio::io_service ios;
    boost::asio::ip::tcp::endpoint server(boost::asio::ip::address::from_string(addr), port);
    boost::asio::ip::tcp::socket socket(ios);

    socket.connect(server);
    
    return socket;
}

inline auto buffer_str(const std::string& input) {
    std::vector<char> buffer;
    for (char i : input) buffer.push_back(i);
    return buffer;
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
        
        auto kqasm_buffer = buffer_str(kqasm.str());
        auto kqasm_size = to_char<uint32_t>(kqasm_buffer.size());
        
        char ack[1];

        auto socket = conect(kbw_addr, kbw_port);

        // Send KQASM size
        socket.send(boost::asio::buffer(kqasm_size));
        socket.receive(boost::asio::buffer(ack));

        // Send KQASM
        socket.send(boost::asio::buffer(kqasm_buffer));
        socket.receive(boost::asio::buffer(ack));
        
        // Get results
        auto get_command = to_char<char>(1);
        for (auto &i : measure_map) {
            socket.send(boost::asio::buffer(get_command));
            socket.receive(boost::asio::buffer(ack));
         
            auto get_idx = to_char<uint64_t>(i.first);
            socket.send(boost::asio::buffer(get_idx));
            socket.receive(boost::asio::buffer(get_idx));

            *(i.second.first) = from_char<uint64_t>(get_idx);
            
            *(i.second.second) = true;
        }
        
        // Get dumps
        auto dump_command = to_char<char>(2);
        boost::array<char, 8> buffer_dump_size;
        for (auto &i : dump_map) {
            // Send dump command
            socket.send(boost::asio::buffer(dump_command));
            socket.receive(boost::asio::buffer(ack));

            auto get_idx = to_char<uint64_t>(i.first);
            socket.send(boost::asio::buffer(get_idx));

            socket.receive(boost::asio::buffer(buffer_dump_size));

            auto dump_size = from_char<uint64_t>(buffer_dump_size);

            boost::array<char, 2048> buffer;
            std::vector<char> dump_file;

            size_t bytes_transferred;

            do {
                bytes_transferred = socket.receive(boost::asio::buffer(buffer));
                dump_file.insert(dump_file.end(), buffer.begin(), buffer.begin()+bytes_transferred);
            } while (dump_file.size() < dump_size);

            std::stringstream stream_out;
            stream_out.write(dump_file.data(), dump_file.size());

            boost::archive::binary_iarchive iarchive{stream_out};

            iarchive >> *i.second.first; 

            *i.second.second = true;
        } 

        auto exit_command = to_char<char>(0);
        socket.send(boost::asio::buffer(exit_command));
        socket.receive(boost::asio::buffer(ack));

        socket.close();
    } else for (auto &i : measure_map) {
        *(i.second.first) =  0;
        *(i.second.second) = true;
    }

    executed = true;
}

void ket::exec_quantum() {
    process_stack.top()->exec();

    process_stack.pop();

    *process_on_top_stack.top() = false;
    process_on_top_stack.pop();

    process_stack.push(std::make_shared<process>());
    process_on_top_stack.push(std::make_shared<bool>(true));
}
