use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize)]
pub enum QuantumGate {
    PauliX,
    PauliY,
    PauliZ,
    Hadamard,
    Phase(f64),
    RX(f64),
    RY(f64),
    RZ(f64),
}

#[derive(Serialize, Deserialize)]
pub enum ClassicalOp {
    Eq,
    Neq,
    Gt,
    Geq,
    Lt,
    Leq,
    Add,
    Sub,
    Mul,
    Div,
    Sll,
    Srl,
    And,
    Or,
    Xor,
}

#[derive(Serialize, Deserialize)]
pub enum Instruction {
    Alloc {
        dirty: bool,
        target: u32,
    },
    Free {
        dirty: bool,
        target: u32,
    },
    Gate {
        gate: QuantumGate,
        target: u32,
        control: Vec<u32>,
    },
    Measure {
        qubits: Vec<u32>,
        output: u32,
    },
    Plugin {
        name: String,
        target: Vec<u32>,
        control: Vec<u32>,
        adj: bool,
        args: String,
    },
    Jump {
        addr: u32,
    },
    Branch {
        test: u32,
        then: u32,
        otherwise: u32,
    },
    IntOp {
        op: ClassicalOp,
        result: u32,
        lhs: u32,
        rhs: u32,
    },
    IntSet {
        result: u32,
        value: i64,
    },
    Dump {
        qubits: Vec<u32>,
        output: u32,
    },
    Halt,
}

pub struct CodeBlock {
    instructions: Vec<Instruction>,
    adj_instructions: Vec<Vec<Instruction>>,
    ended: bool,
}

impl CodeBlock {
    pub fn new() -> CodeBlock {
        CodeBlock {
            instructions: Vec::new(),
            adj_instructions: Vec::new(),
            ended: false,
        }
    }

    pub fn add_instruction(&mut self, instruction: Instruction) -> Result<(), &str> {
        if self.ended {
            return Err("Cannot append statement to a terminated block.");
        }

        if self.adj_instructions.is_empty() {
            self.ended = match instruction {
                Instruction::Jump { .. } => true,
                Instruction::Branch { .. } => true,
                Instruction::Halt => true,
                _ => false,
            };

            self.instructions.push(instruction);
        } else {
            self.adj_instructions.last_mut().unwrap().push(instruction);
        }

        Ok(())
    }

    pub fn in_adj(&self) -> bool {
        !self.adj_instructions.is_empty()
    }

    pub fn adj_begin(&mut self) -> Result<(), &str> {
        if self.ended {
            return Err("Cannot begin and inverse block in a terminated block.");
        }
        self.adj_instructions.push(Vec::new());
        Ok(())
    }

    pub fn adj_end(&mut self) -> Result<(), &str> {
        if !self.in_adj() {
            return Err("No inverse block to end.");
        }

        if self.adj_instructions.len() == 1 {
            while let Some(instruction) = self.adj_instructions.last_mut().unwrap().pop() {
                self.instructions.push(instruction);
            }
            self.adj_instructions.pop();
        } else {
            let mut to_pop = self.adj_instructions.pop().unwrap();
            while let Some(instruction) = to_pop.pop() {
                self.adj_instructions.last_mut().unwrap().push(instruction);
            }
        }

        Ok(())
    }

    pub fn instructions(&self) -> &[Instruction] {
        &self.instructions
    }
}

#[cfg(test)]
mod tests {}
