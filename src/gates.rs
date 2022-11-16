use std::{cell::RefCell, ops::RangeBounds, rc::Rc, result};

use crate::{error::KetError, Dump, Future, Process, Qubit};

#[derive(Debug)]

pub struct Quant {
    pub(crate) qubits: Vec<Qubit>,
    pub(crate) process: Rc<RefCell<Process>>,
}

pub type Result<T> = result::Result<T, KetError>;

impl Quant {
    pub fn new(process: &Rc<RefCell<Process>>, size: usize) -> Result<Quant> {
        let mut qubits = Vec::with_capacity(size);
        for _ in 0..size {
            qubits.push(process.borrow_mut().allocate_qubit(false)?);
        }

        Ok(Quant {
            qubits,
            process: Rc::clone(process),
        })
    }

    pub fn slice<R: RangeBounds<i32> + std::slice::SliceIndex<[Qubit], Output = Vec<Qubit>>>(
        &self,
        range: R,
    ) -> Quant {
        Quant {
            process: Rc::clone(&self.process),
            qubits: Vec::from_iter(self.qubits[range].iter().cloned()),
        }
    }

    pub fn cat(quants: &[Quant]) -> Result<Quant> {
        let process = &quants
            .first()
            .expect("\"quants\" must have length >= 1")
            .process;

        let pid = process.borrow().pid();

        let mut qubits = Vec::new();
        for quant in quants {
            if quant.process.borrow().pid() != pid {
                return Err(KetError::UnmatchedPid);
            }
            qubits.extend(quant.qubits.iter().cloned());
        }

        Ok(Quant {
            qubits,
            process: Rc::clone(process),
        })
    }

    pub fn at(&self, index: usize) -> Quant {
        Quant {
            process: Rc::clone(&self.process),
            qubits: vec![self.qubits[index].clone()],
        }
    }
}

pub fn x(qubits: &Quant) -> Result<()> {
    for target in &qubits.qubits {
        qubits
            .process
            .borrow_mut()
            .apply_gate(crate::QuantumGate::PauliX, target)?
    }

    Ok(())
}

pub fn y(qubits: &Quant) -> Result<()> {
    for target in &qubits.qubits {
        qubits
            .process
            .borrow_mut()
            .apply_gate(crate::QuantumGate::PauliY, target)?
    }

    Ok(())
}

pub fn z(qubits: &Quant) -> Result<()> {
    for target in &qubits.qubits {
        qubits
            .process
            .borrow_mut()
            .apply_gate(crate::QuantumGate::PauliZ, target)?
    }

    Ok(())
}

pub fn h(qubits: &Quant) -> Result<()> {
    for target in &qubits.qubits {
        qubits
            .process
            .borrow_mut()
            .apply_gate(crate::QuantumGate::Hadamard, target)?
    }

    Ok(())
}

pub fn phase(lambda: f64, qubits: &Quant) -> Result<()> {
    for target in &qubits.qubits {
        qubits
            .process
            .borrow_mut()
            .apply_gate(crate::QuantumGate::Phase(lambda), target)?
    }

    Ok(())
}

pub fn rx(theta: f64, qubits: &Quant) -> Result<()> {
    for target in &qubits.qubits {
        qubits
            .process
            .borrow_mut()
            .apply_gate(crate::QuantumGate::RX(theta), target)?
    }

    Ok(())
}

pub fn ry(theta: f64, qubits: &Quant) -> Result<()> {
    for target in &qubits.qubits {
        qubits
            .process
            .borrow_mut()
            .apply_gate(crate::QuantumGate::RY(theta), target)?
    }

    Ok(())
}

pub fn rz(theta: f64, qubits: &Quant) -> Result<()> {
    for target in &qubits.qubits {
        qubits
            .process
            .borrow_mut()
            .apply_gate(crate::QuantumGate::RZ(theta), target)?
    }

    Ok(())
}

pub fn ctrl<F, T>(control: &Quant, gate: F) -> Result<T>
where
    F: FnOnce() -> T,
{
    control
        .process
        .borrow_mut()
        .ctrl_push(&control.qubits.iter().collect::<Vec<&Qubit>>())?;

    let result = gate();

    control.process.borrow_mut().ctrl_pop()?;

    Ok(result)
}

pub fn adj<F, T>(process: &Rc<RefCell<Process>>, gate: F) -> Result<T>
where
    F: FnOnce() -> T,
{
    process.borrow_mut().adj_begin()?;

    let result = gate();

    process.borrow_mut().adj_end()?;

    Ok(result)
}

pub fn around<Outer, Inner, T>(
    process: &Rc<RefCell<Process>>,
    outer: Outer,
    inner: Inner,
) -> Result<T>
where
    Outer: Fn() -> (),
    Inner: FnOnce() -> T,
{
    outer();

    let result = inner();

    adj(process, outer)?;

    Ok(result)
}

pub fn measure(qubits: &mut Quant) -> Result<Future> {
    qubits
        .process
        .borrow_mut()
        .measure(&mut qubits.qubits.iter_mut().collect::<Vec<&mut Qubit>>())
}

pub fn dump(qubits: &Quant) -> Result<Dump> {
    qubits
        .process
        .borrow_mut()
        .dump(&qubits.qubits.iter().collect::<Vec<&Qubit>>())
}

#[cfg(test)]
mod tests {
    use crate::Process;

    use super::{ctrl, h, measure, x, Quant};

    #[test]
    fn test_bell() {
        let p = Process::new_ptr();

        let mut q = Quant::new(&p, 2).unwrap();
        h(&q.at(0)).unwrap();
        ctrl(&q.at(0), || x(&q.at(1)).unwrap()).unwrap();

        let _m = measure(&mut q).unwrap();

        p.borrow_mut().prepare_for_execution().unwrap();

        println!("{:#?}", p.borrow_mut().serialize_quantum_code(crate::serialize::DataType::JSON));
    }
}
