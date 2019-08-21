module C = Configurator.V1

let () =
  C.main ~name:"gst-editing-services-1.0" (fun c ->
      let default : C.Pkg_config.package_conf = {libs= []; cflags= []} in
      let conf =
        match C.Pkg_config.get c with
        | None -> default
        | Some pc ->
            let ges =
              match
                C.Pkg_config.query pc ~package:"gst-editing-services-1.0"
              with
              | None -> default
              | Some deps -> deps
            in
            let pangocairo =
              match C.Pkg_config.query pc ~package:"pangocairo" with
              | None -> default
              | Some deps -> deps
            in
            { cflags= ges.cflags @ pangocairo.cflags
            ; libs= ges.libs @ pangocairo.libs }
      in
      let fout = open_out "cflags.txt" in
      List.iter (fun f -> Printf.fprintf fout "%s\n" f) conf.cflags ;
      close_out fout ;
      let fout = open_out "libs.txt" in
      List.iter (fun f -> Printf.fprintf fout "%s\n" f) conf.libs ;
      close_out fout ;
      Configurator.V1.Flags.write_sexp "c_flags.sexp" conf.cflags ;
      Configurator.V1.Flags.write_sexp "c_library_flags.sexp" conf.libs )
