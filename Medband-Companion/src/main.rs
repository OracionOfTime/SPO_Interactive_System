use components::dosage;
use dioxus::prelude::*;
use sir::{css, AppStyle};
use structs::Entry;

#[cfg(target_arch = "wasm32")]
fn main() {
    // init debug tool for WebAssembly
    wasm_logger::init(wasm_logger::Config::default());
    console_error_panic_hook::set_once();
    dioxus_web::launch(app);
}

#[cfg(not(target_arch = "wasm32"))]
fn main() {
    use dioxus_desktop::{Config, WindowBuilder};

    let conf = dioxus_hot_reload::Config::new().with_rebuild_command("cargo run");
    hot_reload_init!(conf);
    dioxus_desktop::launch_cfg(
        app,
        Config::new().with_window(WindowBuilder::new().with_title("Medband Companion")),
    );
}

mod components {
    pub mod calendar;
    pub mod dosage;
}

pub mod structs;

static INITIAL_ENTRIES: usize = 2;

fn app(cx: Scope) -> Element {
    let dosages = use_state(cx, || {
        Vec::from_iter((0..INITIAL_ENTRIES).map(|_| (Entry::default())))
    });
    let add_button = css!(
        "
    background-color: #4CAF50;
    border: none;
    color: white;
    font-size: 25px;
    cursor: pointer;
    border-radius: 15px;
        "
    );
    let container = css!(
        "
        display: flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        "
    );

    #[cfg(not(target_arch = "wasm32"))]
    let deployer = rsx!( deploy_nav { entries: dosages.get() } );
    #[cfg(target_arch = "wasm32")]
    let deployer: Element = None;

    cx.render(rsx! (
        AppStyle {}
        div { class: "{container}",
            for id in 0..dosages.len() {
                dosage::dosage { id: id, entries: dosages }
            }
            button {
                onclick: move |_| {
                    dosages
                        .with_mut(|v| {
                            v.insert(v.len(), Entry::default());
                        })
                },
                class: "{add_button}",
                "Add Compound"
            }
        }
        deployer
    ))
}

#[cfg(not(target_arch = "wasm32"))]
#[inline_props]
fn deploy_nav<'a>(cx: Scope, entries: &'a Vec<Entry>) -> Element<'a> {
    let ports = use_memo(cx, (), |_| {
        serialport::available_ports().unwrap_or_default()
    });
    let selected_port = use_state(cx, || 0);
    let deploy_button = css!(
        "
    background-color: black;
    border: none;
    color: white;
    text-align: center;
    text-decoration: none;
    display: inline-block;
    font-size: 25px;
    cursor: pointer;
    border-radius: 10px;
    padding: 10px;
        "
    );
    let deploy_div = css!(
        "
    display: flex;
    position: fixed;
    bottom: 10px;
    gap: 10px;
        "
    );
    cx.render(rsx!(
        div { class: "{deploy_div}",
            select {
                class: "compound-select",
                style: "heigh: 45px",
                onchange: move |e| selected_port.set(e.value.parse().unwrap_or_default()),
                for (num , port) in ports.iter().enumerate() {
                    option { value: "{num}", "{port.port_name}" }
                }
            }
            button {
                onclick: move |_| deploy(entries, &ports[*selected_port.get()]),
                class: "{deploy_button}",
                "Deploy"
            }
        }
    ))
}

#[cfg(not(target_arch = "wasm32"))]
fn deploy(entries: &Vec<Entry>, p: &serialport::SerialPortInfo) {
    use std::time::Duration;

    println!("{:?}", p);
    let mut port = serialport::new(p.port_name.clone(), 600_000)
        .timeout(Duration::from_millis(1000))
        .open()
        .expect("Failed to open port");
    let mut output = Vec::new();
    for e in entries {
        output.extend(e.to_bytes());
    }
    println!("sending {:?}...", output);
    let n = port.write(output.as_slice()).unwrap();
    println!("written {n} bytes");
    //port.write_all(output.as_slice()).unwrap();
}
