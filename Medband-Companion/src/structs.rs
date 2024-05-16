use std::{fmt::Display, str::FromStr};

use enum_iterator::Sequence;

#[derive(Default, Clone, Copy)]
pub enum Unit {
    #[default]
    Pills = 0,
    MilliGrams = 1,
    Grams = 2,
}

#[derive(Default, Clone, Copy, Sequence, PartialEq, Eq)]
pub enum Color {
    #[default]
    Green = 0,
    Red = 1,
    Blue = 2,
}

impl Display for Color {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Color::Red => write!(f, "Red"),
            Color::Blue => write!(f, "Blue"),
            Color::Green => write!(f, "Green"),
        }
    }
}

impl FromStr for Color {
    type Err = String;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        match s.to_lowercase().trim() {
            "red" => Ok(Color::Red),
            "blue" => Ok(Color::Blue),
            "green" => Ok(Color::Green),
            _ => Err(format!("wrong color string {}", s)),
        }
    }
}

#[derive(Default, Clone, Copy)]
pub struct Day {
    val: u8,
}

impl Day {
    pub fn set(&mut self, time: usize) {
        match time % 3 {
            0 => self.set_morning(),
            1 => self.set_afternoon(),
            _ => self.set_evening(),
        }
    }
    pub fn unset(&mut self, time: usize) {
        match time % 3 {
            0 => self.unset_morning(),
            1 => self.unset_afternoon(),
            _ => self.unset_evening(),
        }
    }
    pub fn toggle(&mut self, time: usize) {
        match time % 3 {
            0 => self.toggle_morning(),
            1 => self.toggle_afternoon(),
            _ => self.toggle_evening(),
        }
    }
    pub fn get(&self, time: usize) -> bool {
        match time % 3 {
            0 => self.get_morning(),
            1 => self.get_afternoon(),
            _ => self.get_evening(),
        }
    }
    pub fn new(morning: bool, afternoon: bool, evening: bool) -> Day {
        let mut d = Day::default();
        if morning {
            d.set_morning();
        }
        if afternoon {
            d.set_afternoon();
        }
        if evening {
            d.set_evening();
        }
        d
    }
    pub fn toggle_morning(&mut self) {
        self.val ^= 0b001;
    }
    pub fn toggle_afternoon(&mut self) {
        self.val ^= 0b010;
    }
    pub fn toggle_evening(&mut self) {
        self.val ^= 0b100;
    }
    pub fn set_morning(&mut self) {
        self.val |= 0b001;
    }
    pub fn set_afternoon(&mut self) {
        self.val |= 0b010;
    }
    pub fn set_evening(&mut self) {
        self.val |= 0b100;
    }
    pub fn unset_morning(&mut self) {
        self.val &= !0b001;
    }
    pub fn unset_afternoon(&mut self) {
        self.val &= !0b010;
    }
    pub fn unset_evening(&mut self) {
        self.val &= !0b100;
    }
    pub fn get_morning(&self) -> bool {
        self.val & 0b001 > 0
    }
    pub fn get_afternoon(&self) -> bool {
        self.val & 0b010 > 0
    }
    pub fn get_evening(&self) -> bool {
        self.val & 0b100 > 0
    }
}

#[derive(Clone)]
pub struct Entry {
    pub color: Color,
    pub dispenser_id: u8,
    pub unit: Unit,
    pub unit_value: u8,
    pub times: Vec<Day>,
}

impl Default for Entry {
    fn default() -> Self {
        Self {
            color: Default::default(),
            dispenser_id: Default::default(),
            unit: Default::default(),
            unit_value: Default::default(),
            times: vec![Day::default(); 7],
        }
    }
}

impl Entry {
    pub fn to_bytes(&self) -> Vec<u8> {
        let total = 4 + self.times.len() as u8;
        let mut res = Vec::with_capacity(total as usize);
        res.push(total);
        res.push(self.color as u8);
        res.push(self.dispenser_id);
        res.push(self.unit as u8);
        res.push(self.unit_value);
        for time in self.times.iter() {
            res.push(time.val);
        }
        res
    }
}
