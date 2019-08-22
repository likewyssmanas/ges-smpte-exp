module GstreamerCore = Gstreamer.Core;
module GstEditingServicesCore = GstEditingServices.Core_raw;

open GstEditingServices;
open Ctypes;
open Printf;

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
  let pipeline = GstEditingServices.Pipeline.create();
  let layer = Layer.create();
  Ges_c.set_priority(to_voidp(layer), 0);
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
  ignore @@ Timeline.add_layer(timeline, layer);
  let uri_clip =
    Nug.Uri_clip.create(Nug.Uri.create("/home/manas/Downloads/modi-g"));
  let clip = Nug.Uri_clip.to_clip(uri_clip);
  Ges_c.set_clip_property(clip, "start", 0.0);
  Ges_c.set_clip_property(clip, "in-point", 0.0);
  Ges_c.set_clip_property(clip, "duration", total_video_duration);
  Ges_c.set_priority(to_voidp(clip), 0);

  ignore @@ Pipeline.set_timeline(pipeline, timeline);
  Ges_c.run_ges_pipeline(
    pipeline,
    Nug.Uri.create(outputPath),
    kDesiredWidth,
    kDesiredHeight,
    int_of_float(total_video_duration),
  );
};
