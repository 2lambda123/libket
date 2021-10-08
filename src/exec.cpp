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
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/container/map.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/complex.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <cmath>
#include <fstream>
#include <libssh/libsshpp.hpp>
#include <poll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

using namespace ket;
using tcp = boost::asio::ip::tcp;    
namespace http = boost::beast::http; 

inline std::string urlencode(std::string str) {
    const boost::container::map<std::string, std::string> replace_map = {
	    {" ",  "%20"},
	    {"\t", "%09"},
	    {"\n", "%0A"},
	    {"=" , "%3D"},
	    {"!" , "%21"},
	    {">" , "%3E"},
	    {"<" , "%3C"},
	    {"+" , "%2B"},
	    {"*" , "%2A"},
	    {"/" , "%2F"},
	    {"@" , "%40"},
	    {"(" , "%28"},
	    {")" , "%29"},
    };

    for (auto pair : replace_map) boost::replace_all(str,  pair.first, pair.second);
    return str;
}

void begin_session(ssh::Session& session, const char* user, const char* host, int ssh_port) {
    session.setOption(SSH_OPTIONS_HOST, host);
    session.setOption(SSH_OPTIONS_USER, user);
    session.setOption(SSH_OPTIONS_PORT, ssh_port);

    session.connect();
    if(session.userauthPublickeyAuto() != SSH_AUTH_SUCCESS)
        throw std::runtime_error(std::string{"Libket could not establish SSH connection to "}+user+"@"+host+" on port "+std::to_string(ssh_port));
}

auto create_server_socket() {
    auto server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) throw std::runtime_error("Libket could not create socket for SSH forwarding");

    sockaddr_in addr{
        .sin_family = AF_INET,
        .sin_port = 0,
        .sin_addr{INADDR_ANY}
    };

    socklen_t sizeof_addr = sizeof(addr);
    
    if (int opt = 1; setsockopt(server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        throw std::runtime_error("Libket: error setting SSH forwarding socket options");
    
    if (bind(server, reinterpret_cast<sockaddr*>(&addr), sizeof_addr) == -1) 
        throw std::runtime_error("Libket could not bind SSH forwarding socket");

    if (getsockname(server, reinterpret_cast<sockaddr*>(&addr), &sizeof_addr) == -1) 
        throw std::runtime_error("Libket could not get SSH forwarding socket port");

    if (listen(server, 1) == -1) 
        throw std::runtime_error("Libket: error listen SSH forwarding socket");

    return std::make_pair(server, addr);
}

void start_listen(ssh::Channel* channel, int server, sockaddr_in addr, int host_port) {
    try {
        channel->openForward("127.0.0.1", host_port, "127.0.0.1", ntohs(addr.sin_port));
    } catch (ssh::SshException) {
        throw std::runtime_error{"Libket could not open the SSH forwarding chanel "+std::to_string(ntohs(addr.sin_port))+":127.0.0.1:"+std::to_string(host_port)};
    }

    pollfd fd{
        .fd = server,
        .events = POLLIN,
    };
    while (poll(&fd, 1, 1000) == 0) if (not channel->isOpen() or channel->isEof()) return;
    
    socklen_t sizeof_addr = sizeof(addr);
    auto connection = accept(server, reinterpret_cast<sockaddr*>(&addr), &sizeof_addr);
    if (connection == -1) {
        throw std::runtime_error("Libket: connection error on SSH forwarding socket");
    }

    char buffer[4096];
    int read_size, send_size;

    while (channel->isOpen() and not channel->isEof()) {
        read_size = recv(connection, buffer, sizeof(buffer), MSG_DONTWAIT);
        if (read_size > 0) {
            send_size = 0;
            while (send_size < read_size) {
                send_size += channel->write(buffer+send_size, read_size-send_size);
            }
        } else if (read_size == -1 and not (errno == EAGAIN or errno == EWOULDBLOCK)) {
            throw std::runtime_error("Libket could not read from SSH forwarding socket");
        }

        read_size = channel->readNonblocking(buffer, sizeof(buffer), false);
        send_size = 0;
        while (send_size < read_size) {
            auto send_size_ = send(connection, buffer+send_size, read_size-send_size, 0);
            if (send_size_ == -1) {
                throw std::runtime_error("Libket could not send from SSH forwarding socket");
            } 
            send_size += send_size_;
        }        
    }

    close(connection);
    close(server);  
}

void process::exec() {
    if (output_kqasm) {
        std::ofstream out{kqasm_path, std::ofstream::app};
        out << kqasm 
            << "=========================" 
            << std::endl;
        out.close();
    }

    if (execute_kqasm) {
        auto use_kbw_port = kbw_port;
        auto use_kbw_addr = kbw_addr;
        ssh::Session session;
        ssh::Channel* channel;
        std::thread server_thd;

        if (use_ssh) {
            begin_session(session, ssh_user.c_str(), use_kbw_addr.c_str(), ssh_port);
            use_kbw_addr = "127.0.0.1";
            auto [socket, addr] = create_server_socket();
            use_kbw_port = std::to_string(ntohs(addr.sin_port));
            channel = new ssh::Channel{session};
            server_thd = std::thread{start_listen, channel, socket, addr, std::atoi(kbw_port.c_str())};    
        }

        auto kqasm_file = urlencode(kqasm);

        boost::asio::io_context ioc;
        tcp::resolver resolver{ioc};
        tcp::socket socket{ioc};

        try {
            auto const results = resolver.resolve(use_kbw_addr, use_kbw_port);
            boost::asio::connect(socket, results.begin(), results.end());
        } catch (const boost::system::system_error&) {
            throw std::runtime_error{"Libket could not connect to "+use_kbw_addr+":"+use_kbw_port};
        }

        std::string param{"/api/v1/run?"};
        if (dump_to_fs) param += "&dump2fs=1";
        if (send_seed) param += "&seed=" + std::to_string(std::rand());
        param += api_args;
        for (auto arg : api_args_map) param += "&" + arg.first + "=" + urlencode(arg.second);

        http::request<http::string_body> req{http::verb::get, param, 11};
        req.set(http::field::host, use_kbw_addr);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/x-www-form-urlencoded");

        std::string body{};

        body += "kqasm="             + kqasm_file
             +  "&n_blocks="         + std::to_string(n_blocks)
             +  "&n_qubits="         + std::to_string(used_qubits)
             +  "&max_alloc_qubits=" + std::to_string(max_allocated_qubits)
             +  "&has_plugins="      + std::to_string((plugins_sum == 0? 0 : 1))
             +  "&has_free="         + std::to_string((free_qubits == 0? 0 : 1))
             +  "&has_dump="         + std::to_string((n_dumps == 0? 0 : 1))
             +  "&has_set="          + std::to_string((n_set_inst == 0? 0 : 1));

        req.body() = body;
        req.prepare_payload();
        
        http::write(socket, req);
        
        boost::beast::flat_buffer buffer;
        http::response_parser<http::dynamic_body> res;
        res.body_limit(std::numeric_limits<std::uint64_t>::max());
        
        try {
            http::read(socket, buffer, res);
        } catch (const boost::system::system_error&) {
            throw std::runtime_error{"Libket: the server closed during the execution"};
        }

        boost::system::error_code ec;
        socket.shutdown(tcp::socket::shutdown_both, ec);
 
        if(ec && ec != boost::system::errc::not_connected)
            throw boost::system::system_error{ec};

        std::stringstream json_file;
        json_file << boost::beast::make_printable(res.get().body().data());
        result = json_file.str();

        boost::property_tree::ptree pt;
        boost::property_tree::read_json(json_file, pt);

        if (pt.count("error")) {
            throw std::runtime_error("Liket: the execution server returned with the error: \""+pt.get<std::string>("error")+"\"");
        } 

        for (auto result : pt.get_child("int")) {
            auto i = std::stol(result.first);
            *(measure_map[i].first) = std::stol(result.second.get_value<std::string>());
            *(measure_map[i].second) = true;
        }

        auto dump2fs = pt.count("dump2fs")? pt.get<std::string>("dump2fs") == "1" : false;

        if (dump2fs) for (auto result : pt.get_child("dump")) {
            auto i = std::stol(result.first);
            auto path = result.second.get_value<std::string>();

            std::ifstream stream_out{path};
            boost::archive::binary_iarchive iarchive{stream_out};

            iarchive >> *(dump_map[i].first); 

            *(dump_map[i].second) = true;

        } else for (auto result : pt.get_child("dump")) {
            auto i = std::stol(result.first);
            auto b64_bin = result.second.get_value<std::string>();

            using iter = boost::archive::iterators::transform_width<boost::archive::iterators::binary_from_base64<std::string::const_iterator>, 8, 6>;
            auto bin = std::string(iter(b64_bin.begin()), iter(b64_bin.end()));
            
            std::stringstream stream_out;
            stream_out.write(bin.c_str(), bin.size());
            
            boost::archive::binary_iarchive iarchive{stream_out};

            iarchive >> *(dump_map[i].first); 

            *(dump_map[i].second) = true;
        
        } 
        
        if (use_ssh) {
            channel->sendEof();
            channel->close();

            server_thd.join();
            delete channel;
        }
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
