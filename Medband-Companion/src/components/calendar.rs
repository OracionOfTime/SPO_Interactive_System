use dioxus::prelude::*;
use sir::css;

use crate::structs::{Day, Entry};

#[inline_props]
pub fn calendar<'a>(cx: Scope, id: usize, entries: &'a UseState<Vec<Entry>>) -> Element {
    let calendar_container = css!(
        "display: flex; gap: 10px; align-items: center; flex-direction: column; margin-bottom: 20px;"
    );
    let num_days = 7;
    cx.render(rsx!(
        div { class: "{calendar_container}",
            calendar_table { days: num_days, id: *id, entries: entries }
        }
    ))
}

fn num_to_time(num: usize) -> &'static str {
    match num % 3 {
        0 => "Morning",
        1 => "Afternoon",
        _ => "Evening",
    }
}

#[inline_props]
pub fn calendar_cell<'a>(
    cx: Scope,
    id: usize,
    day_id: usize,
    daytime: usize,
    entries: &'a UseState<Vec<Entry>>,
) -> Element<'a> {
    let selected = entries.get().get(*id).unwrap().times[*day_id].get(*daytime);

    let cell = css!(
        "
            &.selected {
                background-color: green;
            }
        "
    );
    let class = format!("{cell} {}", if selected { "selected" } else { "" });
    cx.render(rsx!(td {
        class: "{class}",
        onclick: move |_| entries
            .with_mut(|v| v.get_mut(*id).unwrap().times[*day_id].toggle(*daytime))
    }))
}

static WEEKDAYS: [&str; 7] = [
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday",
];

#[inline_props]
pub fn calendar_table<'a>(
    cx: Scope,
    days: usize,
    id: usize,
    entries: &'a UseState<Vec<Entry>>,
) -> Element {
    let calendar_table = css!(
        "
        border-collapse: collapse;
        td, th {
            border: 1px solid black;
            text-align: left;
            width: 50px;
        }
        "
    );
    let denom_cells = css!(
        "
        padding: 8px;
    "
    );
    let day_row = WEEKDAYS
        .iter()
        .map(|day| rsx!(td {key: "{day}", class: "{denom_cells}", "{day}"}));
    let rows = (0..3).map(|x| {
        let entries = (0..*days).map(move |y| {
            rsx!(calendar_cell {
                key: "{y}",
                id: *id,
                day_id: y,
                daytime: x,
                entries: entries
            })
        });
        rsx!(
            tr { key: "{x}",
                td {class: "{denom_cells}", "{num_to_time(x)}" }
                entries
            }
        )
    });
    cx.render(rsx!(table {
        class: "{calendar_table}",
        tr{
            td{class: "{denom_cells}"},
            day_row
        },
        rows
    }))
}
