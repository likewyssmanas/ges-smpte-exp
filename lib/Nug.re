open Ctypes;
open GstEditingServices;
open Printf;

module Guard = {
  let safe = (~f_sym) =>
    fun
    | true => ()
    | false => {
        Printf.printf("%s failed. Exiting...", f_sym);
        exit(-1);
      };
};

module Void_ptr = {
  let create = x => to_voidp(x);
};

module Uri = {
  let create = p => "file://" ++ p;
};

let print_and_fail = m => {
  print_endline(m);
  exit(-1);
};

module Uri_clip = {
  let create = uri =>
    switch (Uri_clip.create(uri)) {
    | Some(uc) => uc
    | None =>
      print_and_fail(sprintf("Media could not be created from %s", uri))
    };

  let to_clip = uc => Ges_c.to_ges_clip @@ to_voidp(uc);
};

module Timeline_element = {
  let set_name = (timeline_element_as_void_ptr, name) => {
    let timeline_element =
      from_voidp(Timeline_element.t_typ, timeline_element_as_void_ptr);

    Guard.safe(~f_sym="Timeline.set_name failed") @@
    Timeline_element.set_name(timeline_element, Some(name));
  };
};

module Gst = {
  let deinit = () => print_endline("TODO: Write Gst.deinit stub");
};

module Ges = {
  let deinit = () => print_endline("TODO: Write GES.deinit stub");
};

let signal_connect = (obj_as_void_ptr, event, event_handler, data_opt) =>
  Ges_c.signal_connect(
    obj_as_void_ptr,
    event,
    event_handler,
    switch (data_opt) {
    | Some(data) => to_voidp @@ addr(data)
    | None => null
    },
  );
