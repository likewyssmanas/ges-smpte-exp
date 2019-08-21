open Foreign

module M (F : Ctypes.FOREIGN) = struct
  let foreign = F.foreign

  module C = struct
    include Ctypes

    let ( @-> ) = F.( @-> )

    let returning = F.returning
  end

  open GstEditingServices

  (* type gd_type
   * let gdouble : gd_type typ = C.int *)
  let gdouble = C.double

  (* typedef "gdouble" C.double *)
  let gulong = C.typedef C.uint64_t "gulong"

  let track_type_enum = C.typedef C.uint32_t "GESTrackType"

  let gcallback_fn =
    Ctypes.(ptr void @-> ptr void @-> ptr void @-> returning void)

  let gcallback = funptr gcallback_fn

  let element_to_bin =
    foreign "caml_element_to_bin"
      C.(ptr Gstreamer.Element.t_typ @-> returning (ptr Gstreamer.Bin.t_typ))

  let elements_of_bin =
    foreign "caml_elements_of_bin"
      C.(ptr Gstreamer.Bin.t_typ @-> returning (ptr Gstreamer.List.t_typ))

  let to_ges_clip =
    foreign "caml_to_ges_clip" C.(ptr void @-> returning (ptr Clip.t_typ))

  let to_ges_clip_from_void =
    foreign "caml_to_ges_clip_from_void"
      C.(ptr void @-> returning (ptr Clip.t_typ))

  let to_ges_timeline_element =
    foreign "caml_to_ges_timeline_element"
      C.(ptr Effect.t_typ @-> returning (ptr Timeline_element.t_typ))

  let to_ges_track_element =
    foreign "caml_to_ges_track_element"
      C.(ptr void @-> returning (ptr Track_element.t_typ))

  let to_ges_container =
    foreign "caml_to_ges_container"
      C.(ptr Clip.t_typ @-> returning (ptr Container.t_typ))

  let ges_get_track_type =
    foreign "caml_ges_get_track_type"
      C.(ptr Track.t_typ @-> returning track_type_enum)

  let gst_element_name =
    foreign "caml_gst_element_name"
      C.(ptr Gstreamer.Element.t_typ @-> returning string)

  let ges_source_name =
    foreign "caml_ges_source_name" C.(ptr void @-> returning string)

  let set_interpolation_cubic_mode =
    foreign "caml_set_interpolation_cubic_mode"
      C.(ptr Gstreamer.Control_source.t_typ @-> returning void)

  let get_control_binding_source =
    foreign "caml_get_control_binding_source"
      C.(ptr void @-> returning (ptr Gstreamer.Control_source.t_typ))

  let interpolate =
    foreign "caml_interpolate"
      C.(ptr void @-> float @-> float @-> float @-> float @-> returning void)

  let get_control_binding_name =
    foreign "caml_get_control_binding_name" C.(ptr void @-> returning string)

  let ges_set_track_restriction_caps =
    foreign "caml_ges_set_track_restriction_caps"
      C.(ptr void @-> string @-> returning void)

  let ges_title_clip_set_child_properties =
    foreign "caml_ges_title_clip_set_child_properties"
      C.(ptr void @-> uint32_t @-> int @-> int @-> returning void)

  let get_effect_bin_description =
    foreign "caml_get_effect_bin_description" C.(ptr void @-> returning string)

  let test = foreign "caml_test" C.(ptr void @-> returning void)

  let test2 = foreign "caml_test2" C.(void @-> returning (ptr Clip.t_typ))

  let test3 = foreign "caml_test3" C.(ptr void @-> ptr void @-> returning void)

  let test_gulong = foreign "caml_test_gulong" C.(void @-> returning gulong)

  let set_clip_property =
    foreign "caml_set_clip_property"
      C.(ptr Clip.t_typ @-> string @-> float @-> returning void)

  let mute_clip =
    foreign "caml_mute_clip"
      C.(ptr Uri_clip.t_typ @-> returning void)

  let mute_source =
    foreign "caml_mute_source"
     C.(ptr void @-> returning void)    

  let set_priority =
    foreign "caml_set_clip_priority" C.(ptr void @-> int @-> returning void)

  let run_ges_pipeline =
    foreign "caml_run_ges_pipeline"
      C.(ptr Pipeline.t_typ @-> string @-> int @-> int @-> int @-> returning void)

  let get_media_dimensions =
    foreign "caml_get_media_dimensions" C.(string @-> returning (ptr uint32_t))

  let get_text_dimensions =
    foreign "caml_get_text_dimensions" C.(string @-> returning (ptr uint32_t))

  let free_media_dimensions =
    foreign "caml_free_media_dimensions" C.(ptr uint32_t @-> returning void)

  let free_text_dimensions =
    foreign "caml_free_text_dimensions" C.(ptr uint32_t @-> returning void)

  let get_effect_description =
    foreign "caml_get_effect_description"
      C.(ptr Effect.t_typ @-> returning string)

  let signal_connect =
    foreign "caml_signal_connect"
      C.(ptr void @-> string @-> gcallback @-> ptr void @-> returning gulong)

  let signal_disconnect =
    foreign "caml_signal_disconnect" C.(ptr void @-> gulong @-> returning void)

  let core_init_pipeline =
    foreign "core_init_pipeline" C.(void @-> returning int)

  let core_pipeline_add_media =
    foreign "core_pipeline_add_media"
      C.(
        int @-> string @-> float @-> float @-> float @-> int @-> string
        @-> returning void)

  let core_pipeline_add_transition =
    foreign "core_pipeline_add_transition"
      C.(int @-> string @-> float @-> float @-> returning void)

  let core_pipeline_add_text_overlay =
    foreign "core_pipeline_add_text_overlay"
      C.(
        int @-> string @-> float @-> float @-> float @-> int @-> int @-> int
        @-> int @-> float @-> float @-> string @-> returning void)

  let core_pipeline_render =
    foreign "core_pipeline_render" C.(int @-> string @-> returning void)
end
