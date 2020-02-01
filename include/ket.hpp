#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <memory>
#include <sstream>

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
          Qubit_alloc(size_t qubit_index);

        private:
          boost::unordered_set<size_t> qubit_index;
          std::stringstream circuit;
          boost::unordered_map<size_t, std::shared_ptr<Result>> measurement_return;

          friend class Handler;
      };

      Handler(const std::string& out_path, const std::string& kbw_path, size_t seed);
      ~Handler();
      
      Qubits alloc(size_t size);
      void add_gate(std::string gate, const Qubits& qubits);
      Bits measure(const Qubits& qubits);
      void __run(const Bits& bits);

    private:

      std::stringstream& merge(const Qubits& qubits);

      bool out_to_file;
      std::string out_path;
      std::ofstream out_file;
      std::string kbw_path;
      size_t quantum_counter;
      size_t classical_counter;
      boost::unordered_map<size_t, std::shared_ptr<Qubit_alloc>> allocations;
      boost::unordered_map<size_t, size_t> measure_map;
      size_t seed;
    };
}

namespace ket {

    static std::unique_ptr<base::Handler> hdr;

    void init(int argc, char* argv[]);

    class Bit;
    class Qubit {
     public:
        Qubit(size_t size=1);
        Qubit operator() (size_t index) const;
        Qubit operator() (size_t begin, size_t end) const;
        size_t size() const;

     private:
        Qubit(const base::Handler::Qubits& qubits);

        base::Handler::Qubits qubits;

        friend void x(const Qubit& q);
        friend void y(const Qubit& q);
        friend void x(const Qubit& q);
        friend void h(const Qubit& q);
        friend void s(const Qubit& q);
        friend void sd(const Qubit& q);
        friend void t(const Qubit& q);
        friend void td(const Qubit& q);
        friend void cnot(const Qubit& ctrl, const Qubit& target);
        friend Qubit operator+(const Qubit& a, const Qubit& b);
        friend Bit measure(const Qubit& q);
    };

    class Bit {
     public:
        Bit operator() (size_t index) const;
        Bit operator() (size_t begin, size_t end) const;

        int operator[] (size_t index);

     private:
        Bit(const base::Handler::Bits& bits);
        
        base::Handler::Bits bits;

        friend Bit measure(const Qubit& q);
    };

    void x(const Qubit& q);
    void y(const Qubit& q);
    void x(const Qubit& q);
    void h(const Qubit& q);
    void s(const Qubit& q);
    void sd(const Qubit& q);
    void t(const Qubit& q);
    void td(const Qubit& q);
    void cnot(const Qubit& ctrl, const Qubit& target);
    Qubit operator+(const Qubit& a, const Qubit& b);
    Bit measure(const Qubit& q);

}