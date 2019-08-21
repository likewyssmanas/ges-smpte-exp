let prefix = "foo_stub"

let prologue = "

"

let () =
  print_endline prologue;
  Cstubs.Types.write_c Format.std_formatter (module Ges_c_binding_types.M)
