use crate::components::calendar;
use crate::structs::{Color, Entry};
use dioxus::prelude::*;
use enum_iterator::all;
use sir::{css, global_css};

use std::str::FromStr;

#[inline_props]
pub fn dosage<'a>(cx: Scope<'a>, id: usize, entries: &'a UseState<Vec<Entry>>) -> Element<'a> {
    global_css!(
        "
        .compound-select {
            -webkit-appearance: menulist-button;  
        }
    "
    );
    let dosage_container = css!(
        "
        display: flex;
        flex-direction: row;
        flex-wrap: nowrap;
        justify-content: center;
        align-items: center;
        gap: 5px;
        border-top: 1px solid black;
        padding: 20px 0;
        width: 100%;
        font-family: sans-serif;
    "
    );
    let dosage_input = css!(
        "
        padding: 0;
        height: 25px;
        border-width: 1px;
        padding-left: 5px;
    "
    );
    let dosage_amount = css!(
        "
        width: 40px;
    "
    );
    cx.render(rsx!(
        div { class: "{dosage_container}",
            label { "Compound:" }
            input { class: "{dosage_input}", r#type: "text" }
            label { "Amount:" }
            input {
                class: "{dosage_amount} {dosage_input}",
                r#type: "number",
                onchange: move |e| {
                    entries
                        .with_mut(|v| {
                            v.get_mut(*id).unwrap().unit_value = e.value.parse().unwrap_or_default();
                        })
                }
            }
            label { "Dispenser Chamber:" }
            dispenser_picker { id: *id, entries: entries }
            label { "Color:" }
            color_picker { id: *id, entries: entries }
        }
        div { calendar::calendar { id: *id, entries: entries } }
    ))
}

#[inline_props]
pub fn dispenser_picker<'a>(
    cx: Scope<'a>,
    id: usize,
    entries: &'a UseState<Vec<Entry>>,
) -> Element<'a> {
    use_memo(cx, (), |_| {
        entries.with_mut(|v| v.get_mut(*id).unwrap().dispenser_id = *id as u8 + 1)
    });
    cx.render(rsx!(
        select {
            class: "compound-select",
            onchange: move |e| {
                entries
                    .with_mut(|v| {
                        v.get_mut(*id).unwrap().dispenser_id = e.value.parse().unwrap_or_default();
                    })
            },
            value: "{entries.get()[*id].dispenser_id}",
            for val in (1..=entries.len()) {
                dispenser_entry { key: "{val}", num: val }
            }
        }
    ))
}

#[inline_props]
pub fn dispenser_entry(cx: Scope, num: usize) -> Element {
    cx.render(rsx!( option { value: "{num}", "{num}" } ))
}

#[inline_props]
pub fn color_picker<'a>(
    cx: Scope<'a>,
    id: usize,
    entries: &'a UseState<Vec<Entry>>,
) -> Element<'a> {
    let colors = all::<Color>().map(|c| rsx!(color_entry { color: c }));
    let color = entries.get()[*id].color;
    let dot = css!(
        "
        height: 25px;
        width: 25px;
        border-radius: 50%;
        display: inline-block;
        border: 1px solid black;
    "
    );
    cx.render(rsx!(
        select {
            class: "compound-select",
            onchange: move |e| {
                entries
                    .with_mut(|v| {
                        v.get_mut(*id).unwrap().color = Color::from_str(e.value.as_str()).unwrap();
                    })
            },
            colors
        }
        span { class: "{dot}", style: "background-color: {color}" }
    ))
}

#[inline_props]
pub fn color_entry(cx: Scope, color: Color) -> Element {
    cx.render(rsx!( option { value: "{color}", "{color}" } ))
}
