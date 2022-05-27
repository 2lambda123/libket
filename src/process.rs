use serde::{Deserialize, Serialize};
use std::cell::{Ref, RefCell};
use std::rc::Rc;

use crate::code_block::{ClassicalOp, CodeBlock, Instruction, QuantumGate};

trait Pid {
    fn pid(&self) -> u32;
}

#[derive(Debug)]
pub struct Qubit {
    index: u32,
    pid: u32,
    allocated: bool,
    measured: bool,
}

impl Qubit {
    pub fn index(&self) -> u32 {
        self.index
    }

    pub fn pid(&self) -> u32 {
        self.pid
    }

    pub fn allocated(&self) -> bool {
        self.allocated
    }

    pub fn measured(&self) -> bool {
        self.measured
    }
}

impl Pid for &Qubit {
    fn pid(&self) -> u32 {
        self.pid
    }
}

#[derive(Debug)]
pub struct Future {
    index: u32,
    pid: u32,
    value: Rc<RefCell<Option<i64>>>,
}

impl Future {
    pub fn value(&self) -> Ref<Option<i64>> {
        self.value.borrow()
    }

    pub fn index(&self) -> u32 {
        self.index
    }

    pub fn pid(&self) -> u32 {
        self.pid
    }
}

impl Pid for &Future {
    fn pid(&self) -> u32 {
        self.pid
    }
}

#[derive(Debug)]
pub struct Dump {
    value: Rc<RefCell<Option<DumpData>>>,
}

impl Dump {
    pub fn value(&self) -> Ref<Option<DumpData>> {
        self.value.borrow()
    }
}

#[derive(Serialize, Deserialize, Debug)]
pub struct DumpData {
    pub basis_states: Vec<Vec<u64>>,
    pub amplitudes_real: Vec<f64>,
    pub amplitudes_img: Vec<f64>,
}

impl DumpData {
    pub fn basis_states(&self) -> &[Vec<u64>] {
        &self.basis_states
    }

    pub fn amplitudes_real(&self) -> &[f64] {
        &self.amplitudes_real
    }

    pub fn amplitudes_img(&self) -> &[f64] {
        &self.amplitudes_real
    }
}

#[derive(Serialize, Deserialize)]
pub struct QuantumResult {
    pub future: Vec<i64>,
    pub dump: Vec<DumpData>,
    pub exec_time: f64,
}

pub struct Label {
    index: u32,
    pid: u32,
}

impl Label {
    pub fn index(&self) -> u32 {
        self.index
    }

    pub fn pid(&self) -> u32 {
        self.pid
    }
}

impl Pid for &Label {
    fn pid(&self) -> u32 {
        self.pid
    }
}

impl Qubit {
    pub fn not_allocated_err(&self) -> Result<(), &str> {
        if self.allocated {
            Ok(())
        } else {
            Err("Cannot operate with a deallocated qubit.")
        }
    }
}

#[derive(Serialize, Deserialize)]
pub struct Metrics {
    pub max_num_qubit: u32,
    pub qubit_count: u32,
    pub future_count: u32,
    pub dump_count: u32,
    pub label_count: u32,
    pub timeout: Option<u32>,
}

pub struct Process {
    pid: u32,

    num_qubit: u32,
    max_num_qubit: u32,

    qubit_count: u32,
    future_count: u32,
    dump_count: u32,
    label_count: u32,

    blocks: Vec<CodeBlock>,
    current_block: u32,

    ctrl_stack: Vec<Vec<u32>>,

    futures: Vec<Rc<RefCell<Option<i64>>>>,
    dumps: Vec<Rc<RefCell<Option<DumpData>>>>,

    timeout: Option<u32>,
    exec_time: Option<f64>,

    waiting_result: bool,

    quantum_code_json: Option<String>,
    quantum_code_bin: Option<Vec<u8>>,
}

impl Process {
    pub fn new(pid: u32) -> Process {
        Process {
            pid,
            num_qubit: 0,
            max_num_qubit: 0,
            qubit_count: 0,
            future_count: 1,
            dump_count: 0,
            label_count: 0,
            blocks: vec![CodeBlock::new()],
            current_block: 0,
            ctrl_stack: Vec::new(),
            futures: vec![Rc::new(RefCell::new(Some(0)))],
            dumps: Vec::new(),
            timeout: None,
            exec_time: None,
            waiting_result: false,
            quantum_code_json: None,
            quantum_code_bin: None,
        }
    }

    fn get_current_block(&mut self) -> &mut CodeBlock {
        self.blocks.get_mut(self.current_block as usize).unwrap()
    }

    pub fn alloc(&mut self, dirty: bool) -> Result<Qubit, String> {
        self.check_not_ctrl_adj_err()?;

        let index = self.qubit_count;
        self.qubit_count += 1;
        self.num_qubit += 1;
        self.max_num_qubit = if self.num_qubit > self.max_num_qubit {
            self.num_qubit
        } else {
            self.max_num_qubit
        };

        let pid = self.pid;
        let block = self.get_current_block();

        block.add_instruction(Instruction::Alloc {
            dirty,
            target: index,
        })?;

        Ok(Qubit {
            index,
            pid,
            allocated: true,
            measured: false,
        })
    }

    fn match_pid(&self, obj: &impl Pid) -> Result<(), String> {
        if obj.pid() != self.pid {
            Err(format!("Unmatched PID. Maybe you are using an Object from another Process. (PROCESS PID = {}, OBJ PID = {})", self.pid, obj.pid()))
        } else {
            Ok(())
        }
    }

    pub fn free(&mut self, qubit: &mut Qubit, dirty: bool) -> Result<(), String> {
        self.match_pid(&(qubit as &Qubit))?;
        qubit.not_allocated_err()?;

        let block = self.get_current_block();

        block.add_instruction(Instruction::Free {
            dirty,
            target: qubit.index,
        })?;

        qubit.allocated = false;

        Ok(())
    }

    fn get_ctrl_as_vec(&self) -> Vec<u32> {
        let mut tmp_vec = Vec::new();
        for inner_ctrl in self.ctrl_stack.iter() {
            tmp_vec.extend(inner_ctrl.iter());
        }
        tmp_vec
    }

    fn target_in_ctrl_err(target: &Qubit, control: &Vec<u32>) -> Result<(), &'static str> {
        if control.contains(&target.index) {
            Err("A qubit cannot be targeted and controlled at the same time.")
        } else {
            Ok(())
        }
    }

    pub fn apply_gate(&mut self, gate: QuantumGate, target: &Qubit) -> Result<(), String> {
        target.not_allocated_err()?;
        self.match_pid(&target)?;
        let control = self.get_ctrl_as_vec();
        Process::target_in_ctrl_err(target, &control)?;

        let block = self.get_current_block();

        let gate = match gate {
            QuantumGate::Phase(lambda) => {
                QuantumGate::Phase(if block.in_adj() { -lambda } else { lambda })
            }
            QuantumGate::RX(theta) => QuantumGate::RX(if block.in_adj() { -theta } else { theta }),
            QuantumGate::RY(theta) => QuantumGate::RY(if block.in_adj() { -theta } else { theta }),
            QuantumGate::RZ(theta) => QuantumGate::RZ(if block.in_adj() { -theta } else { theta }),
            gate => gate,
        };

        block.add_instruction(Instruction::Gate {
            gate,
            target: target.index,
            control,
        })?;

        Ok(())
    }

    fn check_not_ctrl_adj_err(&self) -> Result<(), &str> {
        if !self.ctrl_stack.is_empty() {
            return Err("Cannot apply this operation inside a Inverse block.");
        }

        match self.blocks.get(self.current_block as usize) {
            Some(block) if block.in_adj() => {
                Err("Cannot apply this operation inside a Control block.")
            }
            _ => Ok(()),
        }
    }

    pub fn measure(&mut self, qubits: &mut [&mut Qubit]) -> Result<Future, String> {
        self.check_not_ctrl_adj_err()?;
        let qubits_index: Vec<u32> = qubits.iter().map(|qubit| qubit.index).collect();

        for qubit in qubits {
            self.match_pid(&(qubit as &Qubit))?;
            qubit.not_allocated_err()?;
            qubit.measured = true;
        }

        let future_index = self.future_count;
        self.future_count += 1;

        let future_value = Rc::new(RefCell::new(None));

        self.futures.push(Rc::clone(&future_value));

        let block = self.get_current_block();

        block.add_instruction(Instruction::Measure {
            qubits: qubits_index,
            output: future_index,
        })?;

        Ok(Future {
            index: future_index,
            pid: self.pid,
            value: future_value,
        })
    }

    pub fn ctrl_push(&mut self, qubits: &[&Qubit]) -> Result<(), String> {
        for ctrl_list in self.ctrl_stack.iter() {
            for qubit in qubits.iter() {
                qubit.not_allocated_err()?;
                self.match_pid(qubit)?;
                if ctrl_list.contains(&qubit.index) {
                    return Err(String::from("Cannot set a qubit as a control twice."));
                }
            }
        }

        self.ctrl_stack
            .push(qubits.iter().map(|qubit| qubit.index).collect());

        Ok(())
    }

    pub fn ctrl_pop(&mut self) -> Result<(), &str> {
        match self.ctrl_stack.pop() {
            Some(_) => Ok(()),
            None => Err("No control block to end."),
        }
    }

    pub fn adj_begin(&mut self) -> Result<(), &str> {
        self.get_current_block().adj_begin()
    }

    pub fn adj_end(&mut self) -> Result<(), &str> {
        self.get_current_block().adj_end()
    }

    pub fn get_label(&mut self) -> Result<Label, &str> {
        if self.waiting_result {
            Err("Cannot create another block while waiting the quantum result.")
        } else {
            let index = self.label_count;
            self.label_count += 1;
            self.blocks.push(CodeBlock::new());
            Ok(Label {
                index,
                pid: self.pid,
            })
        }
    }

    pub fn open_block(&mut self, label: &Label) -> Result<(), String> {
        self.match_pid(&label)?;
        self.current_block = label.index;
        Ok(())
    }

    pub fn jump(&mut self, label: &Label) -> Result<(), String> {
        self.match_pid(&label)?;
        let block = self.get_current_block();
        block.add_instruction(Instruction::Jump { addr: label.index })?;
        Ok(())
    }

    pub fn branch(&mut self, test: &Future, then: &Label, otherwise: &Label) -> Result<(), String> {
        self.match_pid(&test)?;
        self.match_pid(&then)?;
        self.match_pid(&otherwise)?;

        let block = self.get_current_block();
        block.add_instruction(Instruction::Branch {
            test: test.index,
            then: then.index,
            otherwise: otherwise.index,
        })?;

        Ok(())
    }

    pub fn dump(&mut self, qubits: &[&Qubit]) -> Result<Dump, String> {
        self.check_not_ctrl_adj_err()?;
        let qubits_index: Vec<u32> = qubits.iter().map(|qubit| qubit.index).collect();
        for qubit in qubits {
            self.match_pid(qubit)?;
            qubit.not_allocated_err()?;
        }

        let dump_index = self.dump_count;
        self.dump_count += 1;

        let dump_value = Rc::new(RefCell::new(None));
        self.dumps.push(Rc::clone(&dump_value));

        let block = self.get_current_block();
        block.add_instruction(Instruction::Dump {
            qubits: qubits_index,
            output: dump_index,
        })?;

        Ok(Dump { value: dump_value })
    }

    pub fn add_int_op(
        &mut self,
        op: ClassicalOp,
        lhs: &Future,
        rhs: &Future,
    ) -> Result<Future, String> {
        self.check_not_ctrl_adj_err()?;
        self.match_pid(&lhs)?;
        self.match_pid(&rhs)?;

        let result_index = self.future_count;
        self.future_count += 1;

        let result_value = Rc::new(RefCell::new(None));

        self.futures.push(Rc::clone(&result_value));

        let block = self.get_current_block();
        block.add_instruction(Instruction::IntOp {
            op,
            result: result_index,
            lhs: lhs.index,
            rhs: rhs.index,
        })?;

        Ok(Future {
            index: result_index,
            pid: self.pid,
            value: result_value,
        })
    }

    pub fn int_set(&mut self, result: &Future, value: &Future) -> Result<(), String> {
        self.check_not_ctrl_adj_err()?;
        self.match_pid(&result)?;

        let block = self.get_current_block();
        block.add_instruction(Instruction::IntOp {
            op: ClassicalOp::Add,
            result: result.index,
            lhs: 0,
            rhs: value.index,
        })?;

        Ok(())
    }

    pub fn int_new(&mut self, value: i64) -> Result<Future, String> {
        self.check_not_ctrl_adj_err()?;

        let index = self.future_count;
        self.future_count += 1;

        let block = self.get_current_block();
        block.add_instruction(Instruction::IntSet {
            result: index,
            value,
        })?;

        let value = Rc::new(RefCell::new(None));
        self.futures.push(Rc::clone(&value));

        Ok(Future {
            index,
            pid: self.pid,
            value,
        })
    }

    pub fn exec_time(&self) -> Option<f64> {
        self.exec_time
    }

    pub fn get_quantum_code(&mut self) -> Result<Vec<&[Instruction]>, String> {
        self.get_current_block()
            .add_instruction(Instruction::Halt)?;
        self.waiting_result = true;
        let blocks = self
            .blocks
            .iter()
            .map(|block| block.instructions())
            .collect();
        Ok(blocks)
    }

    pub fn get_quantum_code_as_json(&mut self) -> Result<&str, String> {
        if let None = self.quantum_code_json {
            self.quantum_code_json = serde_json::to_string(&self.get_quantum_code()?).ok()
        }
        Ok(self.quantum_code_json.as_ref().unwrap())
    }

    pub fn get_quantum_code_as_bin(&mut self) -> Result<&[u8], String> {
        if let None = self.quantum_code_json {
            self.quantum_code_bin = bincode::serialize(&self.get_quantum_code()?).ok()
        }
        Ok(self.quantum_code_bin.as_ref().unwrap())
    }

    pub fn set_quantum_result(&mut self, mut result: QuantumResult) -> Result<(), &str> {
        if (self.futures.len() != result.future.len()) | (self.dumps.len() != result.dump.len()) {
            Err("The result do not have the expected number of values.")
        } else {
            for (index, value) in result.future.iter().enumerate() {
                *(self.futures.get(index).unwrap().borrow_mut()) = Some(*value);
            }

            for dump in self.dumps.iter_mut().rev() {
                *dump.borrow_mut() = result.dump.pop();
            }

            self.exec_time = Some(result.exec_time);

            Ok(())
        }
    }

    pub fn set_quantum_result_from_json(&mut self, result: &str) -> Result<(), String> {
        let result: QuantumResult = match serde_json::from_str(result) {
            Ok(result) => result,
            Err(msg) => return Err(format!("Fail to parse quantum result JSON: {}", msg)),
        };

        self.set_quantum_result(result)?;

        Ok(())
    }

    pub fn set_quantum_result_from_bin(&mut self, result: Vec<u8>) -> Result<(), String> {
        let result: QuantumResult = match bincode::deserialize(&result) {
            Ok(result) => result,
            Err(msg) => return Err(format!("Fail to parse quantum result BIN: {}", msg)),
        };

        self.set_quantum_result(result)?;

        Ok(())
    }

    pub fn get_metrics(&self) -> Metrics {
        Metrics {
            max_num_qubit: self.max_num_qubit,
            qubit_count: self.qubit_count,
            future_count: self.future_count,
            dump_count: self.dump_count,
            label_count: self.label_count,
            timeout: self.timeout,
        }
    }
}

#[cfg(test)]
mod tests {
    use crate::process::*;

    #[test]
    fn bell_print() {
        let mut p = Process::new(0);
        let mut a = p.alloc(false).unwrap();
        let mut b = p.alloc(false).unwrap();

        p.apply_gate(QuantumGate::Hadamard, &a).unwrap();
        p.ctrl_push(&[&a]).unwrap();
        p.apply_gate(QuantumGate::PauliX, &b).unwrap();
        p.ctrl_pop().unwrap();

        let m = p.measure(&mut [&mut a, &mut b]).unwrap();

        println!("{}", p.get_quantum_code_as_json().unwrap());
        println!("{:#?}", m);
    }
}
