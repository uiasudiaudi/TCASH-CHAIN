use crate::fmt::Formatter;
use crate::{Add, Display, Div, Mul, Sub};

#[derive(Default, Debug, Clone, Copy, Eq, PartialEq, Ord, PartialOrd)]
pub struct tcashGas(u64);

impl Display for tcashGas {
    fn fmt(&self, f: &mut Formatter<'_>) -> crate::fmt::Result {
        self.0.fmt(f)
    }
}

impl Sub<tcashGas> for tcashGas {
    type Output = tcashGas;

    fn sub(self, rhs: tcashGas) -> Self::Output {
        Self(self.0 - rhs.0)
    }
}

impl tcashGas {
    /// Constructs a new `tcashGas` with a given u64 value.
    pub const fn new(gas: u64) -> tcashGas {
        Self(gas)
    }

    /// Consumes `tcashGas` and returns the underlying type.
    pub fn as_u64(self) -> u64 {
        self.0
    }
}

#[derive(Default, Debug, Clone, Copy, Eq, PartialEq, Ord, PartialOrd)]
/// Ethereum gas type which wraps an underlying u64.
pub struct EthGas(u64);

impl Display for EthGas {
    fn fmt(&self, f: &mut Formatter<'_>) -> crate::fmt::Result {
        self.0.fmt(f)
    }
}

impl EthGas {
    /// Constructs a new `EthGas` with a given u64 value.
    pub const fn new(gas: u64) -> EthGas {
        Self(gas)
    }

    /// Consumes `EthGas` and returns the underlying type.
    pub fn as_u64(self) -> u64 {
        self.0
    }
}

impl Add<EthGas> for EthGas {
    type Output = EthGas;

    fn add(self, rhs: EthGas) -> Self::Output {
        EthGas(self.0 + rhs.0)
    }
}

impl Div<usize> for EthGas {
    type Output = EthGas;

    fn div(self, rhs: usize) -> Self::Output {
        EthGas(self.0 / rhs as u64)
    }
}

impl Mul<EthGas> for u32 {
    type Output = EthGas;

    fn mul(self, rhs: EthGas) -> Self::Output {
        EthGas(self as u64 * rhs.0)
    }
}

impl Mul<u32> for EthGas {
    type Output = EthGas;

    fn mul(self, rhs: u32) -> Self::Output {
        EthGas(self.0 * rhs as u64)
    }
}

impl Mul<usize> for EthGas {
    type Output = EthGas;

    fn mul(self, rhs: usize) -> Self::Output {
        EthGas(self.0 * rhs as u64)
    }
}

impl Mul<EthGas> for u64 {
    type Output = EthGas;

    fn mul(self, rhs: EthGas) -> Self::Output {
        EthGas(self * rhs.0)
    }
}
