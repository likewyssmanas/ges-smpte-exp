module GstreamerCore = Gstreamer.Core;
module GstEditingServicesCore = GstEditingServices.Core_raw;

open GstEditingServices;
open Ctypes;
open Printf;

let create_effect = effect_bin_description =>
  switch (Effect.create(effect_bin_description)) {
  | None =>
    print_endline("Error creating effect");
    exit(-1);
  | Some(e) => e
  };

let get_object_name = (container_ptr, timeline_element_ptr) => {
  let container = from_voidp(Container.t_typ, container_ptr);
  Ges_c.ges_source_name(timeline_element_ptr);
};

let get_timeline_element_name = timeline_element_ptr =>
  switch (
    Timeline_element.get_name(
      from_voidp(Timeline_element.t_typ, timeline_element_ptr),
    )
  ) {
  | None =>
    print_endline(
      "Timeline element, title clip source, does not have a name",
    );
    exit(-1);
  | Some(n) => n
  };

let kDesiredWidth = 1000;
let kDesiredHeight = 1000;
let outputPath = "/home/manas/Downloads/test.webm";
let total_video_duration = 5.0;
let run = () => {
  GstreamerCore.init(None, None);
  ignore @@ GstEditingServicesCore.init();
  let timeline = Timeline.create();
  let audio_track = Audio_track.create();
  let video_track = Video_track.create();
  ignore @@
  Timeline.add_track(
    timeline,
    from_voidp(Track.t_typ, to_voidp(audio_track)),
  );
  ignore @@
  Timeline.add_track(
    timeline,
    from_voidp(Track.t_typ, to_voidp(video_track)),
  );
  let layer = Layer.create();
  Ges_c.set_priority(to_voidp(layer), 0);
  /********* White clip ************/
  let uri_clip =
    Nug.Uri_clip.create(Nug.Uri.create("/home/manas/Downloads/modi-g"));
  let clip = Nug.Uri_clip.to_clip(uri_clip);
  Ges_c.set_clip_property(clip, "start", 0.0);
  Ges_c.set_clip_property(clip, "in-point", 0.0);
  Ges_c.set_clip_property(clip, "duration", total_video_duration);
  Ges_c.set_priority(to_voidp(clip), 0);
  ignore @@ Layer.add_clip(layer, clip);
  /********* White clip ************/
  let uri_clip =
    Nug.Uri_clip.create(Nug.Uri.create("/home/manas/Downloads/white.png"));
  let clip = Nug.Uri_clip.to_clip(uri_clip);
  Ges_c.set_clip_property(clip, "start", 0.0);
  Ges_c.set_clip_property(clip, "in-point", 0.0);
  Ges_c.set_clip_property(clip, "duration", total_video_duration);
  Ges_c.set_priority(to_voidp(clip), 0);
  ignore @@ Layer.add_clip(layer, clip);
  ignore @@
  Nug.signal_connect(
    to_voidp(clip),
    "child-added",
    (container_ptr, timeline_element_ptr, _) =>
      if ("GESEffect" != Ges_c.ges_source_name(timeline_element_ptr)) {
        let effect_as_timeline_element =
          from_voidp(
            Timeline_element.t_typ,
            to_voidp(
              create_effect(
                sprintf(
                  "videoscale ! capsfilter caps=video/x-raw,width=%d,height=%d",
                  kDesiredWidth,
                  kDesiredHeight,
                ),
              ),
            ),
          );

        ignore(
          Timeline_element.set_name(
            effect_as_timeline_element,
            Some("scale-to-fit"),
          ),
        );
        ignore(
          Container.add(
            from_voidp(Container.t_typ, container_ptr),
            effect_as_timeline_element,
          ),
        );
      } else if (get_timeline_element_name(timeline_element_ptr)
                 == "scale-to-fit") {
        let effect_as_timeline_element =
          from_voidp(
            Timeline_element.t_typ,
            to_voidp(
              create_effect(
                sprintf("smptealpha border=1 type=44 position=0.5"),
              ),
            ),
          );

        ignore(
          Timeline_element.set_name(
            effect_as_timeline_element,
            Some("smpte"),
          ),
        );
        ignore(
          Container.add(
            from_voidp(Container.t_typ, container_ptr),
            effect_as_timeline_element,
          ),
        );
      },
    None,
  );
  ignore @@ Timeline.add_layer(timeline, layer);
  let pipeline = GstEditingServices.Pipeline.create();
  ignore @@ Pipeline.set_timeline(pipeline, timeline);
  Ges_c.run_ges_pipeline(
    pipeline,
    Nug.Uri.create(outputPath),
    kDesiredWidth,
    kDesiredHeight,
    int_of_float(total_video_duration),
  );
};
