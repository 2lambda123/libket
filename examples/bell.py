import _pyket as libket

if __name__ == '__main__':
    ps = libket.process_t(0)

    a = ps.alloc(False)
    b = ps.alloc(False)

    def bell(a, b):
        ps.gate(libket.gate_t.hadamard, a, 0.0)
        ps.ctrl_push(libket.to_qubit_list([a]))
        ps.gate(libket.gate_t.pauli_x, b, 0.0)
        ps.ctrl_pop()

    bell(a, b)

    bell_dump = ps.dump(libket.to_qubit_list([a, b]))
    result = ps.measure(libket.to_qubit_list([a, b]))

    print(f"Bell: result={result.value}; expected=0or3")
    #std::cout << "Bell state:" << std::endl;
    #print_dump<2>(bell_dump);
