#include <gst/gst.h>
#include <ges/ges.h>
#include <string.h>
#include <stdio.h>
#include <caml/mlvalues.h>


GstEncodingProfile *create_x_profile(int width, int height) {
  GstEncodingContainerProfile *prof;
  GstCaps *caps;

  caps = gst_caps_from_string("video/quicktime");
  prof = gst_encoding_container_profile_new(
      "Ogg audio/video", "Standard OGG/THEORA/VORBIS", caps, NULL);
  gst_caps_unref(caps);

  /* caps = gst_caps_from_string("video/x-h264, height=1336, width=752"); */
  gchar video_caps[1000];
  sprintf(video_caps, "video/x-h264,width=%d,height=%d", width, height);
  caps = gst_caps_from_string(video_caps);
  gst_encoding_container_profile_add_profile(
      prof, (GstEncodingProfile *)gst_encoding_video_profile_new(caps, NULL,
                                                                 NULL, 0));
  gst_caps_unref(caps);

  caps = gst_caps_from_string(
      "audio/mpeg, mpegversion=(int)1, layer=(int)[ 1, 3 ]");
  gst_encoding_container_profile_add_profile(
      prof, (GstEncodingProfile *)gst_encoding_audio_profile_new(caps, NULL,
                                                                 NULL, 0));
  gst_caps_unref(caps);

  return (GstEncodingProfile *)prof;
}



CAMLprim value caml_test_media_file(value file) {
  int return_status = 0;

  GstElement *pipeline1;
  GstBus *bus;

  GMainLoop *mainloop1 = NULL;
  mainloop1 = g_main_loop_new(NULL, FALSE);
  gst_init(NULL, NULL);

  char *file_path = String_val(file);
  char parse_launch_string[1000];
  sprintf(parse_launch_string, "playbin uri=file://%s audio-sink=fakesink video-sink=fakesink", file_path);
  pipeline1 =
      gst_parse_launch(parse_launch_string,
                       NULL);

  bus = gst_element_get_bus(pipeline1);

  GstStateChangeReturn ret = gst_element_set_state(pipeline1, GST_STATE_PLAYING);

  if (ret == GST_STATE_CHANGE_FAILURE) {
    /* g_printerr("Unable to set the pipeline to the playing state.\n"); */
    gst_object_unref(pipeline1);
    return_status = -1;
  }

  g_timeout_add_seconds(4, (GSourceFunc)g_main_loop_quit, mainloop1);
  g_main_loop_run(mainloop1);

  GstMessage *msg = NULL;
  do {
    msg = gst_bus_pop(bus);
    GError *err;
    gchar *debug_info;
    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR:
      gst_message_parse_error(msg, &err, &debug_info);
      if ((strcmp(err->message, "Internal data stream error.") == 0 ||
           strcmp(err->message, "Stream contains no data.") == 0) &&
          strcmp(GST_OBJECT_NAME(msg->src), "typefind") == 0) {
        /* g_print("Typefind couldn't figure out what kind of media it was\n"); */
      } else {
        /* g_printerr("Error received from element %s: %s\n", */
        /*            GST_OBJECT_NAME(msg->src), err->message); */
        /* g_printerr("Debugging information: %s\n", */
        /*            debug_info ? debug_info : "none"); */
      }
      g_clear_error(&err);
      g_free(debug_info);
      g_main_loop_quit(mainloop1);
      msg = NULL;
      return_status = -1;
      break;
    case GST_MESSAGE_EOS:
      g_main_loop_quit(mainloop1);
      msg = NULL;
      break;
    default:
      break;
    }
  } while (msg != NULL);

  g_main_loop_quit(mainloop1);
  gst_element_set_state(pipeline1, GST_STATE_NULL);
  gst_object_unref(bus);
  gst_object_unref(pipeline1);

























  /* GError *err = NULL; */
  /* GOptionContext *ctx; */
  /* GESPipeline *pipeline; */
  /* GESTimeline *timeline; */
  /* GESTrack *tracka, *trackv; */
  /* GESLayer *layer1, *layer2; */
  /* GESUriClip *src; */
  /* GMainLoop *mainloop; */
  
  /* ges_init (); */

  /* /\* Create an Audio/Video pipeline with two layers *\/ */
  /* pipeline = ges_pipeline_new (); */
  /* timeline = ges_timeline_new (); */

  /* tracka = GES_TRACK (ges_audio_track_new ()); */
  /* trackv = GES_TRACK (ges_video_track_new ()); */

  /* layer1 = ges_layer_new (); */
  /* layer2 = ges_layer_new (); */
  /* g_object_set (layer2, "priority", 1, NULL); */

  /* if (!ges_timeline_add_layer (timeline, layer1) || */
  /*     !ges_timeline_add_layer (timeline, layer2) || */
  /*     !ges_timeline_add_track (timeline, tracka) || */
  /*     !ges_timeline_add_track (timeline, trackv) || */
  /*     !ges_pipeline_set_timeline (pipeline, timeline)) */
  /*   return -1; */

  /* if (1) { */
  /*   gchar *uri = gst_filename_to_uri ("file:///home/manas/Downloads/video-tool/6.mp4", NULL); */
  /*   /\* Add the main audio/video file *\/ */
  /*   src = ges_uri_clip_new (uri); */
  /*   g_free (uri); */
  /*   g_object_set (src, "start", 0, "in-point", 0 * GST_SECOND, */
  /*       "duration", 2 * GST_SECOND, NULL); */
  /*   ges_layer_add_clip (layer1, GES_CLIP (src)); */
  /* } */


  /* ges_pipeline_set_timeline(pipeline, timeline); */
  /* ges_pipeline_set_render_settings(pipeline, */
  /*                                  "file:///home/manas/Downloads/out.mp4", */
  /*                                  create_x_profile(752, 1336)); */

  /* /\* ges_pipeline_set_mode (pipeline, GES_PIPELINE_MODE_PREVIEW_VIDEO); *\/ */
  /* ges_pipeline_set_mode(pipeline, GES_PIPELINE_MODE_RENDER); */
  
  

  /* /\* Play the pipeline *\/ */
  /* gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PLAYING); */
  /* mainloop = g_main_loop_new (NULL, FALSE); */
  /* g_timeout_add_seconds (2 + 1, (GSourceFunc) g_main_loop_quit, */
  /*     mainloop); */
  /* g_main_loop_run (mainloop); */


  


  

  return Val_int(return_status);
}
