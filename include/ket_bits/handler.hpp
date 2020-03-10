#pragma once
#include <vector>
#include <memory>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <fstream>

namespace ket::base {
    class Handler {
    public:
      enum Result {
          ZERO = 0,
          ONE = 1,
          NONE = 3,
      };

      class Qubits {
        public:
          Qubits(const std::vector<Qubits>& bits); 
          Qubits operator[](size_t index) const;
          size_t size() const;
          const std::vector<size_t>::const_iterator begin() const;
          const std::vector<size_t>::const_iterator end() const;

        private:
          Qubits(const std::vector<size_t>& qubits);

          std::vector<size_t> qubits;

          friend class Handler;
      };

      class Bits {
        public:
          Bits(const std::vector<Bits>& list);
          Bits operator[](size_t index) const;

          size_t size() const;
          Handler::Result get(size_t index);      

        private:
          Bits(Handler& handler, const std::vector<size_t>& bits, const std::vector<std::shared_ptr<Result>>& measurement);

          Handler &handler;
          std::vector<size_t> bits;
          std::vector<std::shared_ptr<Result>> measurement;

          friend class Handler;
      };
    
      class Qubit_alloc {
        public:
          Qubit_alloc(size_t qubit_index, bool dirty=false);

        private:
          boost::unordered_set<size_t> qubit_index;
          std::stringstream circuit;
          boost::unordered_map<size_t, std::shared_ptr<Result>> measurement_return;

          friend class Handler;
      };

      Handler(const std::string& out_path, const std::string& kbw_path, const std::string& kcq_path, size_t seed, bool no_execute, bool no_optimise);
      ~Handler();
      
      Qubits alloc(size_t size, bool dirty=false);
      void add_gate(const std::string& gate, const Qubits& qubits, const std::vector<double>& args={});
      void __add_gate(const std::string& gate, const Qubits& qubits, const std::vector<double>& args, bool adj, const std::vector<Qubits>& _qctrl, const std::vector<Bits>& _cctrl);
      void add_oracle(const std::string& gate, const Qubits& qubits);
      Bits measure(const Qubits& qubits);
      void free(const Qubits& qubits);
      void free_dirty(const Qubits& qubits);
      void ctrl_begin();
      void add_ctrl(const Qubits& qubits);
      void add_ctrl(const Bits& qubits);
      void ctrl_end();
      void adj_begin();
      void adj_end();
      void __run(const Bits& bits);

    private:

      bool adj_or_ctrl();

      std::stringstream& merge(const Qubits& qubits);

      bool out_to_file;
      std::string out_path;
      std::ofstream out_file;
      std::string kbw_path;
      std::string kqc_path;
      size_t quantum_counter;
      size_t classical_counter;
      boost::unordered_map<size_t, std::shared_ptr<Qubit_alloc>> allocations;
      boost::unordered_map<size_t, size_t> measure_map;
      size_t seed;
      bool no_execute;
      bool no_optimise;
      std::vector<Qubits> qctrl;
      std::vector<Bits> cctrl;
      std::vector<bool> qctrl_b;
      std::vector<bool> cctrl_b;
      std::vector<std::vector<std::function<void()>>> adj_call;
    };
}