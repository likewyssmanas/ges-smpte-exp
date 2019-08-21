#include "core.h"
#include <cairo.h>
#include <glib.h>
#include <pango/pangocairo.h>

const int DESIRED_WIDTH = 800; //1000; //752;
const int DESIRED_HEIGHT = 800; // 1336;

typedef struct {
  float start;
  float duration;
  char *text;
  guint height;
  guint width;
  char *path;
  int left;
  int right;
  int top;
  int bottom;
} SharedData;

void get_scaled_dimensions(int result_dimensions[], int desired_dimensions[],
                           int input_dimensions[]) {

  // By height
  // input_dimensions[0] (width) => desired_dimensions[1] / input_dimensions[1]
  // * input_dimensions[0] input_dimensions[1] (height) => desired_dimensions[1]
  // (height)
  float fit_by_height_factor =
      (float)input_dimensions[1] / desired_dimensions[1];
  /* g_print("fit_by_height_factor: %f\n", fit_by_height_factor); */
  int fit_by_height[2] = {desired_dimensions[0] * fit_by_height_factor,
                          input_dimensions[1]};

  // Similarly by width
  float fit_by_width_factor =
      (float)input_dimensions[0] / desired_dimensions[0];
  /* g_print("fit_by_width_factor: %f\n", fit_by_width_factor); */
  int fit_by_width[2] = {input_dimensions[0],
                         desired_dimensions[1] * fit_by_width_factor};

  // If, when trying to fit by height, scaled width is smaller than desired
  // width, discard it
  if (fit_by_height[0] > input_dimensions[0]) {
    /* g_print("cropped width is greater:  %d (cropped) %d (input)\n",
     * fit_by_height[0], input_dimensions[0] ); */
    result_dimensions[0] = fit_by_width[0];
    result_dimensions[1] = fit_by_width[1];
    return;
  }

  // If, when trying to fit by width, scaled height is smaller than desired
  // height, discard it
  if (fit_by_width[1] > input_dimensions[1]) {
    /* g_print("cropped height is greater: %d (cropped) %d (input)\n",
     * fit_by_width[1], input_dimensions[1]); */
    result_dimensions[0] = fit_by_height[0];
    result_dimensions[1] = fit_by_height[1];
    return;
  }

  // If we are still here, this means, scaling by either dimension works. We
  // must now use those that lead to smallest crop area.
  int fit_by_height_area =
      abs(input_dimensions[0] - fit_by_height[0]) * fit_by_height[1];
  int fit_by_width_area =
      fit_by_width[0] * abs(input_dimensions[1] - fit_by_width[1]);

  if (fit_by_height_area < fit_by_width_area) {
    /* g_print("by height\n"); */
    result_dimensions[0] = fit_by_height[0];
    result_dimensions[1] = fit_by_height[1];
  } else {
    /* g_print("by width\n"); */
    result_dimensions[0] = fit_by_width[0];
    result_dimensions[1] = fit_by_width[1];
  }
}

void get_crop_offsets(int crop_offsets[], int input_dimensions[],
                      int cropped_dimensions[]) {

  // crop_offsets: top right bottom left
  crop_offsets[0] = (input_dimensions[1] - cropped_dimensions[1]) / 2;
  if (crop_offsets[0] < 0) {
    g_print("crop_offsets[0] is negative!!!\n");
  }

  crop_offsets[1] = (input_dimensions[0] - cropped_dimensions[0]) / 2;
  if (crop_offsets[1] < 0) {
    g_print("crop_offsets[1] is negative!!!\n");
  }

  crop_offsets[2] = (input_dimensions[1] - cropped_dimensions[1]) / 2;
  if (crop_offsets[2] < 0) {
    g_print("crop_offsets[2] is negative!!!\n");
  }

  crop_offsets[3] = (input_dimensions[0] - cropped_dimensions[0]) / 2;
  if (crop_offsets[3] < 0) {
    g_print("crop_offsets[3] is negative: %d - %d = %d!!!\n",
            input_dimensions[0], cropped_dimensions[0],
            (input_dimensions[0] - cropped_dimensions[0]));
  }
}

void control_binding_added(GESTrackElement *track_element,
                           GstControlBinding *control_binding,
                           gpointer _user_data) {

  g_print("control binding...\n");
  SharedData *user_data = (SharedData *)_user_data;

  gchar *binding_name;
  g_object_get(control_binding, "name", &binding_name, NULL);
  g_print("Name of the binding: %s\n", binding_name);

  GstControlSource *csource;
  g_object_get(control_binding, "control_source", &csource, NULL);

  GstClockTime start = user_data->start * GST_SECOND;
  g_print("-------- start: %f\n", user_data->start);

  if (strcmp("x-absolute", binding_name) == 0) {
    if (gst_timed_value_control_source_set(
            (GstTimedValueControlSource *)csource, 0, -18) != TRUE) {
      g_print("Initial failed\n");
    };

    if (gst_timed_value_control_source_set(
            (GstTimedValueControlSource *)csource, 0 + 1.5 * GST_SECOND, 0.5) !=
        TRUE) {
      g_print("Final failed\n");
    }

  } else if (strcmp("left", binding_name) == 0 /* && FALSE */) {

    /* int desired_dimesions[] = { DESIRED_WIDTH, DESIRED_HEIGHT  }, */
    /*     input_dimensions[] = { shared_media_data->width,
     * shared_media_data->height }, */
    /*     result_dimensions[] = { 0, 0 }, */
    /*     crop_offsets[] = { 0, 0, 0, 0 }; */

    /* get_scaled_dimensions(result_dimensions, desired_dimesions,
     * input_dimensions); */
    /* get_crop_offsets(crop_offsets, input_dimensions, result_dimensions); */

    GError *error;
    gchar *uri = gst_filename_to_uri(user_data->path, NULL);
    GstDiscoverer *discoverer = gst_discoverer_new(10 * GST_SECOND, &error);
    GstDiscovererInfo *info =
        gst_discoverer_discover_uri(discoverer, uri, &error);
    GList *video_info_glist = gst_discoverer_info_get_video_streams(info);
    const /* GstDiscovererVideoInfo */ GList *video_info =
        g_list_first(video_info_glist);
    /* g_print(video_info == NULL ? "GstDiscovererVideoInfo is NULL": "we
     * good"); */
    /* TODO Handle null pointers ^^^ */
    guint media_height = gst_discoverer_video_info_get_height(video_info->data);
    guint media_width = gst_discoverer_video_info_get_width(video_info->data);

    int desired_dimesions[] = {DESIRED_WIDTH, DESIRED_HEIGHT},
        input_dimensions[] = {media_width, media_height},
        result_dimensions[] = {0, 0}, crop_offsets[] = {0, 0, 0, 0};

    get_scaled_dimensions(result_dimensions, desired_dimesions,
                          input_dimensions);
    get_crop_offsets(crop_offsets, input_dimensions, result_dimensions);

    if (gst_timed_value_control_source_set(
            (GstTimedValueControlSource *)csource, 0, crop_offsets[3] - 32) !=
        TRUE) {
      g_print("Initial failed\n");
    };

    if (gst_timed_value_control_source_set(
            (GstTimedValueControlSource *)csource, 0 + 1.5 * GST_SECOND,
            crop_offsets[3] + 32) != TRUE) {
      g_print("Final failed\n");
    }

  } else if (strcmp("right", binding_name) == 0) {

    GError *error;
    gchar *uri = gst_filename_to_uri(user_data->path, NULL);
    GstDiscoverer *discoverer = gst_discoverer_new(10 * GST_SECOND, &error);
    GstDiscovererInfo *info =
        gst_discoverer_discover_uri(discoverer, uri, &error);
    GList *video_info_glist = gst_discoverer_info_get_video_streams(info);
    const /* GstDiscovererVideoInfo */ GList *video_info =
        g_list_first(video_info_glist);
    /* g_print(video_info == NULL ? "GstDiscovererVideoInfo is NULL": "we
     * good"); */
    /* TODO Handle null pointers ^^^ */
    guint media_height = gst_discoverer_video_info_get_height(video_info->data);
    guint media_width = gst_discoverer_video_info_get_width(video_info->data);

    int desired_dimesions[] = {DESIRED_WIDTH, DESIRED_HEIGHT},
        input_dimensions[] = {media_width, media_height},
        result_dimensions[] = {0, 0}, crop_offsets[] = {0, 0, 0, 0};

    get_scaled_dimensions(result_dimensions, desired_dimesions,
                          input_dimensions);
    get_crop_offsets(crop_offsets, input_dimensions, result_dimensions);

    if (gst_timed_value_control_source_set(
            (GstTimedValueControlSource *)csource, 0, crop_offsets[1] + 32) !=
        TRUE) {
      g_print("Initial failed\n");
    };

    if (gst_timed_value_control_source_set(
            (GstTimedValueControlSource *)csource, 0 + 1.5 * GST_SECOND,
            crop_offsets[1] - 32) != TRUE) {
      g_print("Final failed\n");
    }
  }
}

void child_added(GESContainer *container, GESTrackElement *track_element,
                 gpointer user_data) {

  if (strcmp(G_OBJECT_TYPE_NAME(track_element), "GESAudioUriSource") == 0) {
    return;
  }

  SharedData *shared_media_data = (SharedData *)user_data;
  g_print("track element added %s for %f\n", G_OBJECT_TYPE_NAME(track_element),
          shared_media_data->duration);

  if (strcmp(G_OBJECT_TYPE_NAME(track_element), "GESEffect") != 0) {

    gchar bin_description[1000];
    sprintf(bin_description,
            "textoverlay shaded-background=0 shading-value=50 draw_outline=0 "
            "x-absolute=-19 y-absolute=0.1 text=\"%s\"  line-alignment=left "
            "draw-shadow=false valignment=absolute halignment=absolute "
            "font-desc=\"fira sans 36\" scale-mode=1",
            "<span foreground=\\\"blue\\\" size=\\\"x-large\\\">Blue "
            "text</span> is <i>cool</i>" /*shared_media_data->text*/);
    /* sprintf(bin_description, "agingtv"); */
    g_print("%s --------- \n", bin_description);
    GESTimelineElement *timeline_element =
        GES_TIMELINE_ELEMENT(ges_effect_new(bin_description));
    g_object_set(timeline_element, "start",
                 (guint64)shared_media_data->start * GST_SECOND, "duration",
                 (guint64)shared_media_data->duration * GST_SECOND, "priority",
                 (guint32)0, "in-point", (guint64)0, NULL);
    ges_container_add(container, timeline_element);

    return;
  }

  gchar *bin_desc;
  g_object_get(track_element, "bin-description", &bin_desc, NULL);

  if (bin_desc[0] == 't' && bin_desc[1] == 'e' && bin_desc[2] == 'x' &&
      bin_desc[3] == 't') {
    GstControlSource *csource = gst_interpolation_control_source_new();
    g_object_set(csource, "mode",
                 GST_INTERPOLATION_MODE_CUBIC, // GST_INTERPOLATION_MODE_LINEAR,
                 NULL);

    g_signal_connect(track_element, "control-binding-added",
                     (GCallback)control_binding_added, user_data);

    ges_track_element_set_control_source(track_element, csource, "x-absolute",
                                         "direct-absolute");

    GError *error;

    int desired_dimesions[] = {DESIRED_WIDTH, DESIRED_HEIGHT},
        input_dimensions[] = {shared_media_data->width,
                              shared_media_data->height},
        result_dimensions[] = {0, 0}, crop_offsets[] = {0, 0, 0, 0};

    get_scaled_dimensions(result_dimensions, desired_dimesions,
                          input_dimensions);
    get_crop_offsets(crop_offsets, input_dimensions, result_dimensions);

    /* g_print("Input dimensions: %d x %d\n", input_dimensions[0],
     * input_dimensions[1]); */
    /* g_print("Desired dimensions: %d x %d\n", desired_dimesions[0],
     * desired_dimesions[1]); */
    /* g_print("Media must to be cropped to: %d x %d\n", result_dimensions[0],
     * result_dimensions[1]); */
    /* g_print("And then cropped with\ntop: %d bottom: %d height: %d right: %d
     * left: %d width: %d\n", crop_offsets[0], crop_offsets[2], crop_offsets[2]
     * - crop_offsets[0], crop_offsets[1], crop_offsets[3], crop_offsets[1] -
     * crop_offsets[3]); */

    char bin_description[1000];
    /* sprintf(bin_description, "videobox top=%d right=%d bottom=%d left=%d !
     * videoscale ! video/x-raw,height=%d ", crop_offsets[0], crop_offsets[1],
     * crop_offsets[2], crop_offsets[3], DESIRED_HEIGHT); */
    /* sprintf(bin_description, "videobox top=%d right=%d bottom=%d left=%d !
     * videoscale", crop_offsets[0], crop_offsets[1], crop_offsets[2],
     * crop_offsets[3]); */
    /* sprintf(bin_description, "videobox top=%d right=%d bottom=%d left=%d !
     * videoconvert ! videoscale ! video/x-raw,height=668 ! textoverlay
     * shaded-background=0 shading-value=50 draw_outline=0 x-absolute=-19
     * y-absolute=0.1 text=\"%s\"  line-alignment=left draw-shadow=false
     * valignment=absolute halignment=absolute font-desc=\"left serif 36\"",
     * crop_offsets[0], crop_offsets[1], crop_offsets[2], crop_offsets[3],
     * shared_media_data->text); */
    /* sprintf(bin_description, "videobox top=%d right=%d bottom=%d left=%d !
     * videoscale ! capsfilter caps=video/x-raw,height=1336", crop_offsets[0],
     * crop_offsets[1], crop_offsets[2], crop_offsets[3]); */
    sprintf(bin_description, "videobox top=%d right=%d bottom=%d left=%d",
            crop_offsets[0], crop_offsets[1], crop_offsets[2], crop_offsets[3]);
    /* g_print("%s\n", bin_description); */

    GESTimelineElement *timeline_element =
        GES_TIMELINE_ELEMENT(ges_effect_new(bin_description));
    g_object_set(timeline_element, "start",
                 (guint64)shared_media_data->start * GST_SECOND, "duration",
                 (guint64)shared_media_data->duration * GST_SECOND, "priority",
                 (guint32)0, "in-point", (guint64)0, NULL);

    ges_container_add(container, timeline_element);

  } else if (bin_desc[5] == 'b' && bin_desc[6] == 'o' && bin_desc[7] == 'x') {

    GList *children = GES_CONTAINER_CHILDREN(container);
    gboolean has_image = FALSE;
    while (children != NULL) {
      g_print("[GLIST] %s\n", G_OBJECT_TYPE_NAME(children->data));
      has_image = has_image || (strcmp(G_OBJECT_TYPE_NAME(children->data),
                                       "GESImageSource") == 0);
      children = children->next;
    }

    /* if (strcmp(shared_media_data, "GESImageSource") == 0) { */
    if (has_image) {

      GstControlSource *csource_left_offset =
                           gst_interpolation_control_source_new(),
                       *csource_right_offset =
                           gst_interpolation_control_source_new();

      g_object_set(csource_left_offset, "mode", GST_INTERPOLATION_MODE_CUBIC,
                   NULL);
      g_object_set(csource_right_offset, "mode", GST_INTERPOLATION_MODE_CUBIC,
                   NULL);

      g_signal_connect(track_element, "control-binding-added",
                       (GCallback)control_binding_added, user_data);

      ges_track_element_set_control_source(track_element, csource_left_offset,
                                           "left", "direct-absolute");

      ges_track_element_set_control_source(track_element, csource_right_offset,
                                           "right", "direct-absolute");
    }

    char bin_description[1000];
    sprintf(bin_description,
            "videoscale ! capsfilter caps=video/x-raw,height=1336");
    GESTimelineElement *timeline_element =
        GES_TIMELINE_ELEMENT(ges_effect_new(bin_description));
    g_object_set(timeline_element, "start",
                 (guint64)shared_media_data->start * GST_SECOND, "duration",
                 (guint64)shared_media_data->duration * GST_SECOND, "priority",
                 (guint32)0, "in-point", (guint64)0, NULL);

    ges_container_add(container, timeline_element);

  } else {
  }
}

void layer_clip_added(GESLayer *layer, GESClip *clip, gpointer user_data) {

  /* g_print(">>>>>>> clip added\n"); */
  /* ges_timeline_commit_sync(user_data); */
}

GstEncodingProfile *create_ogg_theora_profile(void) {
  GstEncodingContainerProfile *prof;
  GstCaps *caps;

  caps = gst_caps_from_string("application/ogg");
  prof = gst_encoding_container_profile_new(
      "Ogg audio/video", "Standard OGG/THEORA/VORBIS", caps, NULL);
  gst_caps_unref(caps);

  caps = gst_caps_from_string("video/x-theora");
  gst_encoding_container_profile_add_profile(
      prof, (GstEncodingProfile *)gst_encoding_video_profile_new(caps, NULL,
                                                                 NULL, 0));
  gst_caps_unref(caps);

  caps = gst_caps_from_string("audio/x-vorbis");
  gst_encoding_container_profile_add_profile(
      prof, (GstEncodingProfile *)gst_encoding_audio_profile_new(caps, NULL,
                                                                 NULL, 0));
  gst_caps_unref(caps);

  return (GstEncodingProfile *)prof;
}

GstEncodingProfile *create_webm_profile(int width, int height) {
  GstEncodingContainerProfile *prof;
  GstCaps *caps;

  caps = gst_caps_from_string("video/webm");
  prof = gst_encoding_container_profile_new("Webm", "Standard Webm/VP8/VORBIS",
                                            caps, NULL);
  gst_caps_unref(caps);

  /* caps = gst_caps_from_string("video/x-h264, height=1336, width=752"); */
  gchar video_caps[1000];
  sprintf(video_caps, "video/x-vp8,width=%d,height=%d", width, height);
  caps = gst_caps_from_string(video_caps);
  gst_encoding_container_profile_add_profile(
      prof, (GstEncodingProfile *)gst_encoding_video_profile_new(caps, NULL,
                                                                 NULL, 0));
  gst_caps_unref(caps);

  caps = gst_caps_from_string("audio/x-vorbis");
  gst_encoding_container_profile_add_profile(
      prof, (GstEncodingProfile *)gst_encoding_audio_profile_new(caps, NULL,
                                                                 NULL, 0));
  gst_caps_unref(caps);

  return (GstEncodingProfile *)prof;
}

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

GESClip *make_source(gchar *path, guint64 start, guint64 duration,
                     guint64 inpoint, guint32 priority) {
  gchar *uri = gst_filename_to_uri(path, NULL);

  GESClip *ret = GES_CLIP(ges_uri_clip_new(uri));
  g_object_set(ret, "start", (guint64)start, "duration", (guint64)duration,
               "priority", (guint32)priority, "in-point", (guint64)inpoint,
               NULL);

  g_free(uri);

  return ret;
}

GESClip *make_source_from_core_media(CoreGESPipelineMediaDeprecated *media) {
  gchar *uri = gst_filename_to_uri(media->path, NULL);
  GESClip *ret = GES_CLIP(ges_uri_clip_new(uri));
  g_object_set(ret, "start", (guint64)media->start, "duration",
               (guint64)media->duration, "priority", (guint32)media->priority,
               "in-point", (guint64)media->inpoint, NULL);
  g_free(uri);
  return ret;
}

gboolean print_transition_data(GESClip *tr) {
  GESTrackElement *trackelement;
  GstElement *nleobj;
  guint64 start, duration;
  guint32 priority;
  char *name;
  GList *trackelements;

  if (!tr)
    return FALSE;

  if (!(trackelements = GES_CONTAINER_CHILDREN(tr)))
    return FALSE;
  if (!(trackelement = GES_TRACK_ELEMENT(trackelements->data)))
    return FALSE;
  if (!(nleobj = ges_track_element_get_nleobject(trackelement)))
    return FALSE;

  g_object_get(nleobj, "start", &start, "duration", &duration, "priority",
               &priority, "name", &name, NULL);
  /* g_print ("\nnleobject for %s \n start: %f \n duration:  %f \n priority:
   * %d\n", */
  /*          name, */
  /*          ((float) start) / GST_SECOND, */
  /*          ((float) duration) / GST_SECOND, priority); */

  return FALSE;
}

gboolean append_source_to_layer(CoreGESPipelineState *state, GESLayer *layer,
                                CoreGESPipelineMediaDeprecated *media) {
  GESClip *clip = make_source_from_core_media(media);
  gboolean success = ges_layer_add_clip(layer, clip);
  g_timeout_add_seconds(1, (GSourceFunc)print_transition_data, clip);
  if (success) {
    state->current_position = state->current_position + media->duration;
  }
  return success;
}

GESClip *make_overlay(char *text, guint64 start, guint64 duration,
                      gint priority, guint32 color, gdouble xpos, gdouble ypos,
                      gchar *font_desc) {
  GESClip *ret = GES_CLIP(ges_text_overlay_clip_new());

  g_object_set(ret, "text", (gchar *)text, "start", (guint64)start, "duration",
               (guint64)duration, "priority", (guint32)priority, "in-point",
               (guint64)0, "color", (guint32)color, "valignment",
               (gint)GES_TEXT_VALIGN_POSITION, "halignment",
               (gint)GES_TEXT_HALIGN_POSITION, "xpos", (gdouble)xpos, "ypos",
               (gdouble)ypos, "font-desc", font_desc, NULL);

  return ret;
}

GESTimeline *make_timeline(gchar *nick, double tdur, gchar *patha, gfloat adur,
                           gdouble ainp, gchar *pathb, gfloat bdur,
                           gdouble binp, char *text, guint32 color,
                           gdouble xpos, gdouble ypos) {
  GESTimeline *timeline;
  GESTrack *trackv, *tracka;
  GESLayer *layer1;
  GESClip *srca, *srcb;
  GESClip *overlay;
  guint64 aduration, bduration, tduration, tstart, ainpoint, binpoint;
  GESTransitionClip *tr = NULL;
  CoreGESPipelineState core_pipeline_state;
  CoreGESPipelineMediaDeprecated media_a, media_b;

  timeline = ges_timeline_new();

  trackv = GES_TRACK(ges_video_track_new());
  ges_timeline_add_track(timeline, trackv);

  tracka = GES_TRACK(ges_audio_track_new());
  ges_timeline_add_track(timeline, tracka);

  layer1 = GES_LAYER(ges_layer_new());
  g_object_set(layer1, "priority", (gint32)0, NULL);

  // TODO return -1 instead of exit
  if (!ges_timeline_add_layer(timeline, layer1)) {
    g_print("%s", "unable to add timeline layer");
  }

  aduration = (guint64)(adur * GST_SECOND);
  bduration = (guint64)(bdur * GST_SECOND);
  tduration = (guint64)(tdur * GST_SECOND);
  ainpoint = (guint64)(ainp * GST_SECOND);
  binpoint = (guint64)(binp * GST_SECOND);
  tstart = aduration - tduration;

  init_core_pipeline_state(&core_pipeline_state);
  init_core_pipeline_media(&media_a, patha,
                           (guint64)core_pipeline_state.current_position,
                           (guint64)aduration,
                           (guint64)ainpoint, // inpoint
                           1);
  append_source_to_layer(&core_pipeline_state, layer1, &media_a);
  init_core_pipeline_media(&media_b, pathb,
                           (guint64)core_pipeline_state.current_position -
                               tduration,
                           (guint64)bduration,
                           (guint64)binpoint, // inpoint
                           2);
  append_source_to_layer(&core_pipeline_state, layer1, &media_b);

  overlay =
      make_overlay(text, 0, aduration, 0, color, xpos, ypos, "sans bold 12");
  ges_layer_add_clip(layer1, overlay);

  if (tduration != 0) {
    g_print("creating transition at %" GST_TIME_FORMAT
            " of %f duration (%" GST_TIME_FORMAT ")\n",
            GST_TIME_ARGS(tstart), tdur, GST_TIME_ARGS(tduration));
    if (!(tr = ges_transition_clip_new_for_nick(nick)))
      g_error("invalid transition type %s\n", nick);

    g_object_set(tr, "start", (guint64)tstart, "duration", (guint64)tduration,
                 "in-point", (guint64)0, NULL);
    ges_layer_add_clip(layer1, GES_CLIP(tr));
    g_timeout_add_seconds(1, (GSourceFunc)print_transition_data, tr);
  }

  /* srca = make_source (patha, 0, aduration, ainpoint, 1); */
  /* srcb = make_source (pathb, tstart, bduration, binpoint, 2); */
  /* ges_layer_add_clip (layer1, srca); */
  /* ges_layer_add_clip (layer1, srcb); */
  /* g_timeout_add_seconds (1, (GSourceFunc) print_transition_data, srca); */
  /* g_timeout_add_seconds (1, (GSourceFunc) print_transition_data, srcb); */

  /* overlay = make_overlay (text, 0, aduration, 0, color, xpos, ypos); */
  /* ges_layer_add_clip (layer1, overlay); */

  /* if (tduration != 0) { */
  /*     g_print ("creating transition at %" GST_TIME_FORMAT " of %f duration
   * (%" */
  /*              GST_TIME_FORMAT ")\n", GST_TIME_ARGS (tstart), tdur, */
  /*              GST_TIME_ARGS (tduration)); */

  /*     if (!(tr = ges_transition_clip_new_for_nick (nick))) */
  /*         g_error ("invalid transition type %s\n", nick); */

  /*     g_object_set (tr, */
  /*                   "start", (guint64) tstart, */
  /*                   "duration", (guint64) tduration, */
  /*                   "in-point", (guint64) 0, */
  /*                   NULL); */
  /*     ges_layer_add_clip (layer1, GES_CLIP (tr)); */
  /*     g_timeout_add_seconds (1, (GSourceFunc) print_transition_data, tr); */
  /* } */

  return timeline;
}

int core_main(gchar *patha, gchar *pathb, char *text, gdouble adur,
              gdouble bdur, gdouble tdur, gdouble ainpoint, gdouble binpoint,
              gchar *type) {

  typedef struct {
    int a;
  } test_data_type;

  test_data_type t1, t2, t3;
  t1.a = 100;
  t2.a = 200;
  t3.a = 300;

  LList *list = ll_init();

  ll_insert(list, &t1);
  ll_insert(list, &t2);
  ll_insert(list, &t3);

  LLNode *n;
  test_data_type *ptr;

  n = list->head;
  ptr = n->data;

  /* g_print("Pointer %d\n", (int) n->next); */
  /* g_print("Data %d\n", ptr->a); */

  /* n = n->next; */
  /* ptr = n->data; */
  /* g_print("Pointer %d\n", (int) n->next); */
  /* g_print("Data %d\n", ptr->a); */

  /* n = n->next; */
  /* ptr = n->data; */
  /* g_print("Pointer %d\n", (int) n->next); */
  /* g_print("Data %d\n", ptr->a); */

  LListIterator *iterator = ll_iterator_create(list);
  test_data_type *test_data;
  g_print("Starting traversal...\n");
  while ((test_data = (test_data_type *)ll_iterator_traverse(iterator)) !=
         NULL) {
    g_print(" => %d\n", test_data->a);
  }
  g_print("Done traversing...\n");

  ll_iterator_destroy(iterator);
  ll_destroy(list);

  g_print("%s", "Core started...\n");

  GError *err = NULL;
  gdouble duration = DEFAULT_DURATION;

  guint64 color;
  gdouble xpos = DEFAULT_POS, ypos = DEFAULT_POS;

  gst_init(NULL, NULL);

  ges_init();

  GESTimeline *timeline =
      make_timeline(type, tdur, patha, adur, ainpoint, pathb, bdur, binpoint,
                    text, color, xpos, ypos);

  GESPipeline *pipeline = ges_pipeline_new();
  ges_pipeline_set_timeline(pipeline, timeline);
  ges_pipeline_set_render_settings(pipeline,
                                   "file:///home/manas/Downloads/out.mp4",
                                   create_x_profile(752, 1336));

  /* ges_pipeline_set_mode (pipeline, GES_PIPELINE_MODE_PREVIEW_VIDEO); */
  ges_pipeline_set_mode(pipeline, GES_PIPELINE_MODE_RENDER);

  gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
  GstStateChangeReturn ret =
      gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);

  GMainLoop *mainloop;
  mainloop = g_main_loop_new(NULL, FALSE);
  g_timeout_add_seconds((adur + bdur) + 1, (GSourceFunc)g_main_loop_quit,
                        mainloop);

  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr("Unable to set the pipeline to the playing state.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  g_main_loop_run(mainloop);

  g_print("Done\n");

  return 0;
}

char *my_args_passing_function(char *filename, gdouble a) {
  char *f = malloc(100);
  f[0] = 'a';
  f[1] = 'b';
  f[2] = '\0';
  return f;
}

void init_core_pipeline_state(CoreGESPipelineState *state) {
  state->current_position = 0;
}

void init_core_pipeline_media(CoreGESPipelineMediaDeprecated *media,
                              const char *path, guint64 start, guint64 duration,
                              guint64 inpoint, guint32 priority) {

  media->path = path;
  media->start = start;
  media->duration = duration;
  media->inpoint = inpoint;
  media->priority = priority;
}

CoreGESPipeline *pipeline = NULL;
int core_init_pipeline(void) {

  // TODO. Currently only one global. Must handle different session
  pipeline = __core_init_pipeline_struct();

  // TODO Session. For now only one pipeline can be rendered.
  return 667; // integer id of the pipeline
}

CoreGESPipeline *__core_init_pipeline_struct(void) {
  CoreGESPipeline *pipeline = malloc(sizeof(CoreGESPipeline));
  pipeline->pipeline_elements = ll_init();
  return pipeline;
}

void __core_destroy_pipeline_struct(CoreGESPipeline *pipeline) {
  ll_destroy(pipeline->pipeline_elements);
  free(pipeline);
}

CoreGESPipelineMedia *__core_pipeline_media_create(const char *path,
                                                   float start, float duration,
                                                   float inpoint, int priority,
                                                   const char *text) {
  CoreGESPipelineMedia *media = malloc(sizeof(CoreGESPipelineMedia));
  media->path = path;
  media->start = start;
  media->duration = duration;
  media->inpoint = inpoint;
  media->priority = priority;
  media->text = text;
  return media;
}

CoreGESPipelineTextOverlay *
__core_pipeline_text_overlay_create(const char *text, float start,
                                    float duration, float inpoint, int priority,
                                    int color, int valignment, int halignment,
                                    float xpos, float ypos, char *font_desc) {
  CoreGESPipelineTextOverlay *text_overlay =
      malloc(sizeof(CoreGESPipelineTextOverlay));
  text_overlay->text = text;
  text_overlay->start = start;
  text_overlay->duration = duration;
  text_overlay->inpoint = inpoint;
  text_overlay->priority = priority;
  text_overlay->font_desc = font_desc;
  return text_overlay;
}

CoreGESPipelineTransition *
__core_pipeline_transition_create(char *nick, float start, float duration) {
  CoreGESPipelineTransition *transition =
      malloc(sizeof(CoreGESPipelineTransition));
  transition->transition_nick = nick;
  transition->start = start;
  transition->duration = duration;
  return transition;
}

void __core_pipeline_media_destroy(CoreGESPipelineMedia *media) { free(media); }

void __core_pipeline_text_overlay_destroy(
    CoreGESPipelineTextOverlay *text_overlay) {
  free(text_overlay);
}

void __core_pipeline_transition_destroy(CoreGESPipelineTransition *transition) {
  free(transition);
}

// TODO Hard coded to return global pipeline (CoreGESPipeline pipeline). Fix it.
CoreGESPipeline *__core_pipeline_get(int core_ges_pipeline_id) {
  return pipeline;
}

CoreGESPipelineElement *
__core_pipeline_element_create(CoreGESPipelineElementType type, void *element) {
  CoreGESPipelineElement *pipeline_element =
      malloc(sizeof(CoreGESPipelineElement));
  pipeline_element->type = type;
  pipeline_element->element = element;
  return pipeline_element;
}

void core_pipeline_add_media(int pipeline_id, const char *path, float start,
                             float duration, float inpoint, int priority,
                             const char *text) {
  CoreGESPipeline *pipeline = __core_pipeline_get(pipeline_id);
  CoreGESPipelineMedia *media = __core_pipeline_media_create(
      path, start, duration, inpoint, priority, text);
  CoreGESPipelineElement *element =
      __core_pipeline_element_create(CORE_MEDIA, media);
  ll_insert(pipeline->pipeline_elements, element);
}

void core_pipeline_add_transition(int pipeline_id, char *transition,
                                  float start, float duration) {
  CoreGESPipeline *pipeline = __core_pipeline_get(pipeline_id);
  CoreGESPipelineTransition *transition_element =
      __core_pipeline_transition_create(transition, start, duration);
  CoreGESPipelineElement *element =
      __core_pipeline_element_create(CORE_TRANSITION, transition_element);
  ll_insert(pipeline->pipeline_elements, element);
}

void core_pipeline_add_text_overlay(int pipeline_id, const char *text,
                                    float start, float duration, float inpoint,
                                    int priority, int color, int valignment,
                                    int halignment, float xpos, float ypos,
                                    char *font_desc) {
  CoreGESPipeline *pipeline = __core_pipeline_get(pipeline_id);
  CoreGESPipelineTextOverlay *text_overlay =
      __core_pipeline_text_overlay_create(text, start, duration, inpoint,
                                          priority, color, valignment,
                                          halignment, xpos, ypos, font_desc);
  CoreGESPipelineElement *element =
      __core_pipeline_element_create(CORE_TEXT, text_overlay);
  ll_insert(pipeline->pipeline_elements, element);
}

void core_pipeline_render(int pipeline_id, char *output_path) {
  CoreGESPipeline *pipeline = __core_pipeline_get(pipeline_id);
  LListIterator *iterator = ll_iterator_create(pipeline->pipeline_elements);
  CoreGESPipelineElement *element;

  // Initialising GST
  /* gst_init (NULL, NULL); */
  /* ges_init (); */
  GESPipeline *ges_pipeline = ges_pipeline_new();
  GESTimeline *timeline = ges_timeline_new();
  GESTrack *trackv, *tracka;
  GESLayer *layer;
  GESClip *clip;
  guint64 duration, inpoint;
  gchar *uri;

  trackv = GES_TRACK(ges_video_track_new());
  ges_timeline_add_track(timeline, trackv);

  tracka = GES_TRACK(ges_audio_track_new());
  ges_timeline_add_track(timeline, tracka);

  layer = GES_LAYER(ges_layer_new());
  g_object_set(layer, "priority", (gint32)0, NULL);
  g_signal_connect(layer, "clip-added", (GCallback)layer_clip_added, timeline);

  if (!ges_timeline_add_layer(timeline, layer)) {
    g_print("%s", "unable to add timeline layer");
    // TODO: Handle failures
  }

  g_print("Starting traversal...\n");
  CoreGESPipelineMedia *media = NULL;
  CoreGESPipelineTextOverlay *text_element = NULL;
  CoreGESPipelineTransition *transition = NULL;
  guint64 total_video_duration = 0;
  while ((element = (CoreGESPipelineElement *)ll_iterator_traverse(iterator)) !=
         NULL) {
    switch (element->type) {
    case CORE_MEDIA:
      media = (CoreGESPipelineMedia *)element->element;
      /* g_print("Appending media: %s\n", media->path); */
      /* g_print(">>>>>>>> %f", media->inpoint); */
      uri = gst_filename_to_uri(media->path, NULL);
      clip = GES_CLIP(ges_uri_clip_new(uri));
      g_object_set(clip, "start", (guint64)media->start * GST_SECOND,
                   "duration", (guint64)media->duration * GST_SECOND,
                   "priority", (guint32)6, "in-point",
                   (guint64)media->inpoint * GST_SECOND, NULL);
      SharedData *shared_media_data = (SharedData *)malloc(sizeof(SharedData));
      shared_media_data->duration = media->duration;
      shared_media_data->start = media->start;
      g_print("************************* %f", media->start);
      shared_media_data->text = (gchar *)media->text;
      shared_media_data->path = (gchar *)media->path;

      GError *error;
      GstDiscoverer *discoverer = gst_discoverer_new(10 * GST_SECOND, &error);
      GstDiscovererInfo *info =
          gst_discoverer_discover_uri(discoverer, uri, &error);
      GList *video_info_glist = gst_discoverer_info_get_video_streams(info);
      const /* GstDiscovererVideoInfo */ GList *video_info =
          g_list_first(video_info_glist);
      /* g_print(video_info == NULL ? "GstDiscovererVideoInfo is NULL": "we
       * good"); */
      /* TODO Handle null pointers ^^^ */
      guint height = gst_discoverer_video_info_get_height(video_info->data);
      guint width = gst_discoverer_video_info_get_width(video_info->data);
      shared_media_data->height = height;
      shared_media_data->width = width;

      g_signal_connect(clip, "child-added", (GCallback)child_added,
                       shared_media_data);
      g_free(uri);
      ges_layer_add_clip(layer, clip);
      total_video_duration += media->duration;
      /* g_timeout_add_seconds (1, (GSourceFunc) print_transition_data, clip);
       */
      break;
    case CORE_TRANSITION:
      transition = (CoreGESPipelineTransition *)element->element;
      g_print(
          "Appending transition to the pipeline: %s start: %f duration: %f\n",
          transition->transition_nick, transition->start, transition->duration);
      clip = GES_CLIP(ges_transition_clip_new_for_nick(
          transition->transition_nick)); // TODO check return
      g_object_set(clip, "start", (guint64)transition->start * GST_SECOND,
                   "duration", (guint64)transition->duration * GST_SECOND,
                   "in-point", (guint64)0, // TODO should this be configurable?
                   NULL);
      ges_layer_add_clip(layer, clip);
      break;
    default:
      /* g_warning("Weird. Shouldn't be here"); */
      break;
    }
    /* g_print(" => %d\n", test_data->a); */
  }

  g_print("Done traversing...\n");

  ges_pipeline_set_timeline(ges_pipeline, timeline);
  ges_pipeline_set_render_settings(ges_pipeline,
                                   gst_filename_to_uri(output_path, NULL),
                                   create_x_profile(752, 1336));

  /* ges_pipeline_set_mode (ges_pipeline, GES_PIPELINE_MODE_PREVIEW_VIDEO); */
  ges_pipeline_set_mode(ges_pipeline, GES_PIPELINE_MODE_RENDER);
  GstStateChangeReturn ret =
      gst_element_set_state(GST_ELEMENT(ges_pipeline), GST_STATE_PLAYING);

  GMainLoop *mainloop;

  GstBus *bus = gst_element_get_bus((GstElement *)ges_pipeline);

  gboolean should_exit = FALSE;
  while (!should_exit) {
    GstMessage *msg = gst_bus_timed_pop_filtered(
        bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    /* Parse message */
    if (msg != NULL) {
      GError *err;
      gchar *debug_info;

      switch (GST_MESSAGE_TYPE(msg)) {
      case GST_MESSAGE_ERROR:
        gst_message_parse_error(msg, &err, &debug_info);
        g_printerr("Error received from element %s: %s\n",
                   GST_OBJECT_NAME(msg->src), err->message);
        g_printerr("Debugging information: %s\n",
                   debug_info ? debug_info : "none");
        g_clear_error(&err);
        g_free(debug_info);
        break;
      case GST_MESSAGE_EOS:
        g_print("Should exit....\n");
        should_exit = TRUE;
        g_print("Done");
        break;
      default:
        /* We should not reach here because we only asked for ERRORs and EOS */
        g_printerr("Unexpected message received.\n");
        break;
      }
      gst_message_unref(msg);
    }
  }

  g_print("Rendered\n");
}

void *ll_node_create(void *data) {
  LLNode *node = malloc(sizeof(LLNode));
  node->data = data;
  node->next = NULL;
  return node;
}

void ll_node_destroy(LLNode *node) {
  node->next = NULL;
  free(node);
}

LList *ll_init() {
  LList *list = malloc(sizeof(LList));
  list->head = NULL;
  return list;
}

void ll_destroy(LList *list) {
  LLNode *cur = list->head;
  while (cur != NULL) {
    ll_node_destroy(cur);
    cur = cur->next;
  }
  free(list);
}

void ll_insert(LList *list, void *data) {
  LLNode *node = ll_node_create(data);
  node->next = list->head;
  list->head = node;
}

LListIterator *ll_iterator_create(LList *list) {
  LListIterator *iterator = malloc(sizeof(LListIterator));
  iterator->cur = list->head;
  return iterator;
}

void ll_iterator_destroy(LListIterator *iterator) { free(iterator); }

void *ll_iterator_traverse(LListIterator *iterator) {
  if (iterator == NULL)
    return NULL;
  LLNode *node = iterator->cur;
  if (node != NULL) {
    iterator->cur = node->next;
    return node->data;
  } else {
    iterator->cur = NULL;
    return NULL;
  }
}

void *caml_to_ges_clip(void *o) { return GES_CLIP(o); }

void *caml_to_ges_clip_from_void(void *o) { return GES_CLIP(o); }

void *caml_to_ges_timeline_element(void *o) { return GES_TIMELINE_ELEMENT(o); }

void *caml_to_ges_track_element(void *o) { return GES_TRACK_ELEMENT(o); }

void *caml_to_ges_container(void *o) { return GES_CONTAINER(o); }

void *caml_element_to_bin(void *e) { return GST_BIN(e); }

GList *caml_elements_of_bin(void *b) { return GST_BIN_CHILDREN(b); }

gulong caml_test_gulong() {
  gulong a = 0xFFFFFFFFFFFFFFFF;
  return a;
}

void caml_set_clip_property(void *obj, char *property, float value) {
  g_object_set(obj, property, (guint64)value * GST_SECOND, NULL);
}

void caml_mute_clip(void *obj) {
  g_print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  g_print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  g_print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  g_print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  ges_uri_clip_set_mute(obj, TRUE);
  g_print(ges_uri_clip_is_muted(obj) ? "YES\n": "NO\n");
  g_print("%s\n", ges_uri_clip_get_uri(obj));
  g_print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  g_print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  g_print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  g_print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  
  /* g_object_set(obj, "mute", (gboolean) TRUE, NULL); */
}

void caml_mute_source(void *obj) {
  g_print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  ges_timeline_element_set_child_properties (obj,
                                             "mute",TRUE,
                                             NULL);
  g_print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
}

void caml_set_clip_priority(void *obj, int value) {
  g_object_set(obj, "priority", (guint32)value, NULL);
}

void bus_message_cb(GstBus *bus, GstMessage *msg, GMainLoop *mainloop) {

  /* g_print("Message received from element %s\n", */
  /*            GST_OBJECT_NAME(msg->src)); */

  /* Parse message */
  GError *err;
  gchar *debug_info;
  GstState old_state, new_state;

  switch (GST_MESSAGE_TYPE(msg)) {
  case GST_MESSAGE_ERROR:
    gst_message_parse_error(msg, &err, &debug_info);
    g_printerr("Error received from element %s: %s\n",
               GST_OBJECT_NAME(msg->src), err->message);
    g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
    g_clear_error(&err);
    g_free(debug_info);
    break;
  case GST_MESSAGE_EOS:
    g_main_loop_quit(mainloop);
    break;
  case GST_MESSAGE_STATE_CHANGED:

    gst_message_parse_state_changed(msg, &old_state, &new_state, NULL);
    g_print("Element %s changed state from %s to %s.\n",
            GST_OBJECT_NAME(msg->src), gst_element_state_get_name(old_state),
            gst_element_state_get_name(new_state));
  default:
    break;
  }

  /* gst_message_unref(msg); */
}

void caml_run_ges_pipeline(void *pipeline, char *output_file_path, int width,
                           int height, int timeout_seconds) {
  GMainLoop *mainloop = NULL;
  mainloop = g_main_loop_new(NULL, FALSE);
  ges_pipeline_set_render_settings(pipeline, output_file_path,
                                   create_webm_profile(width, height));
  ges_pipeline_set_mode(pipeline, GES_PIPELINE_MODE_RENDER);

  GstBus *bus = gst_element_get_bus(pipeline);

  gst_bus_add_signal_watch(bus);
  g_signal_connect(bus, "message", G_CALLBACK(bus_message_cb), mainloop);

  gst_element_set_state(pipeline, GST_STATE_PLAYING);
  g_main_loop_run(mainloop);
  g_timeout_add_seconds (timeout_seconds, (GSourceFunc) g_main_loop_quit, mainloop);
  gst_element_set_state(pipeline, GST_STATE_NULL);
}

unsigned int *caml_get_media_dimensions(char *uri) {
  unsigned int *result = (unsigned int *)malloc(sizeof(unsigned int) * 2);
  GError *error;
  GstDiscoverer *discoverer = gst_discoverer_new(10 * GST_SECOND, &error);
  GstDiscovererInfo *info =
      gst_discoverer_discover_uri(discoverer, uri, &error);
  GList *video_info_glist = gst_discoverer_info_get_video_streams(info);
  const /* GstDiscovererVideoInfo */ GList *video_info =
      g_list_first(video_info_glist);
  /* g_print(video_info == NULL ? "GstDiscovererVideoInfo for is NULL\n": "we
   * good\n"); */
  /* TODO Handle null pointers ^^^ */
  guint media_height = gst_discoverer_video_info_get_height(video_info->data);
  guint media_width = gst_discoverer_video_info_get_width(video_info->data);
  result[0] = media_width;
  result[1] = media_height;
  return result;
}

unsigned int *caml_get_text_dimensions(char *text) {
  cairo_surface_t *surface =
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 752, 1336);

  cairo_t *cr = cairo_create(surface);

  cairo_rectangle(cr, 0, 0, 752, 1336);
  cairo_set_source_rgb(cr, 255.0, 0.0, 0.0);
  cairo_fill(cr);

  PangoLayout *layout;
  PangoFontDescription *font_description;

  font_description = pango_font_description_new();
  pango_font_description_set_family(font_description, "Fira Code");
  pango_font_description_set_weight(font_description, PANGO_WEIGHT_BOLD);
  pango_font_description_set_absolute_size(font_description, 32 * PANGO_SCALE);

  layout = pango_cairo_create_layout(cr);
  pango_layout_set_width(layout, 752 * PANGO_SCALE);
  pango_layout_set_wrap(layout, PANGO_WRAP_WORD);
  pango_layout_set_font_description(layout, font_description);
  pango_layout_set_text(layout,
                        "We have defeated ISIS in Syria, my only reason for "
                        "being there during the Trump presidency.",
                        -1);
  /* printf("Width: %d Height: %d\n", pango_layout_get_width(layout),
   * pango_layout_get_height(layout)); */
  PangoRectangle ink, log;
  pango_layout_get_pixel_extents(layout, &ink, &log);
  /* printf("Width: %d Height: %d\n", ink.width, ink.height); */
  /* printf("Width: %d Height: %d\n", log.width, log.height); */

  cairo_set_source_rgb(cr, 255.0, 255.0, 255.0);
  cairo_move_to(cr, 10.0, 50.0);
  pango_cairo_show_layout(cr, layout);

  cairo_destroy(cr);
  cairo_surface_write_to_png(surface, "hello.png");
  cairo_surface_destroy(surface);
  g_object_unref(layout);
  pango_font_description_free(font_description);

  unsigned int *dimensions = malloc(sizeof(unsigned int) * 2);
  dimensions[0] = ink.width;
  dimensions[1] = ink.height;
  return dimensions;
}

void caml_free_media_dimensions(unsigned int *media_dimensions) {
  free(media_dimensions);
}

void caml_free_text_dimensions(unsigned int *text_dimensions) {
  free(text_dimensions);
}

gulong caml_signal_connect(void *o, char *signal, callback *cb, void *data) {
  return g_signal_connect(o, signal, (GCallback)cb, data);
}

void caml_signal_disconnect(void *o, gulong handler_id) {
  g_signal_handler_disconnect(o, handler_id);
}

char *caml_get_effect_description(void *e) {
  char *description;
  gchar *bin_desc;
  g_object_get(e, "bin-description", &description, NULL);
  return description;
}

void cb(void *a, void *b, void *data) {
  GstElement *e = ges_track_element_get_element(b);
  char *text;

  /* g_print("name: %s\n", GST_ELEMENT_NAME(e)); */

  GList *l = GST_BIN_CHILDREN(GST_BIN(e));
  GList *cur = l;
  while (cur != NULL) {
    GstElement *item = GST_ELEMENT(cur->data);
    gchar *name = GST_ELEMENT_NAME(item);
    if (strcmp("textoverlay3", name) == 0) {
      gint x;
      guint width;
      gdouble xpos;
      g_object_get(item, "text", &text, "text-x", &x, "text-width", &width,
                   "xpos", &xpos, NULL);
    }
    cur = cur->next;
  }
}

void *caml_test2() {

  char *bin =
      "textoverlay shaded-background=0 shading-value=50 draw_outline=0 "
      "y-absolute=0.2 deltax=0 xpad=32 deltay=0 text=\"This is a testing "
      "paragraphtesting paragraphtesting paragraph\" line-alignment=left "
      "draw-shadow=false valignment=absolute halignment=left "
      "font-desc=\"fira "
      "sans 32\" auto-resize=false scale-mode=1";
  GESClip *c = GES_CLIP(ges_effect_clip_new(bin, NULL));
  g_object_set(c, "duration", 5 * GST_SECOND, "start", 1 * GST_SECOND, NULL);
  return c;
}

void caml_test3(void *o, void *vv) {
  gdouble value;
  GstControlPoint *v = (GstControlPoint *)vv;
  /* gst_control_source_get_value (o, */
  /*                               0 * GST_SECOND, */
  /*                               &value); */
  if (v == NULL) {
    g_print("NULL\n");
  } else {
    g_print(":::: timestamp: %f \n value: %f\n",
            (float)(v->timestamp / GST_SECOND), v->value);
  }
}

void caml_test(void *o) {

  const gchar *nano_str;
  guint major, minor, micro, nano;

  gst_version(&major, &minor, &micro, &nano);

  printf("This program is linked against GStreamer %d.%d.%d\n", major, minor,
         micro);

  GList *tracks = ges_timeline_get_tracks(o);
  GList *cur = tracks;
  while (cur != NULL) {
    GESTrack *track = cur->data;
    GESTrackType track_type;
    g_object_get(track, "track-type", &track_type, NULL);
    /* g_print("type: %d\n", track_type); */
    switch (track_type) {
    case GES_TRACK_TYPE_VIDEO:
      /* g_print("Track type video\n"); */
      g_signal_connect(track, "track-element-added", (GCallback)cb, NULL);
      break;
    case GES_TRACK_TYPE_AUDIO:
      /* g_print("Track type audio\n"); */
      break;
    case GES_TRACK_TYPE_TEXT:
      /* g_print("Track type TEXT\n"); */
      break;
    case GES_TRACK_TYPE_CUSTOM:
      /* g_print("Track type custom\n"); */
      break;
    case GES_TRACK_TYPE_UNKNOWN:
      /* g_print("Track type unknown\n"); */
      break;
    }
    cur = cur->next;
  }
}

GESTrackType caml_ges_get_track_type(void *t) {
  GESTrackType track_type;
  g_object_get(t, "track-type", &track_type, NULL);
  return track_type;
}

char *caml_gst_element_name(void *e) { return GST_ELEMENT_NAME(e); }

char *caml_ges_source_name(void *s) { return G_OBJECT_TYPE_NAME(s); }

void caml_ges_set_track_restriction_caps(void *track, char *caps_string) {
  GstCaps *caps;
  caps = gst_caps_from_string(caps_string);
  ges_track_update_restriction_caps(track, caps);
}

void caml_ges_title_clip_set_child_properties(void *track_element,
                                              long int backgroundColor, int x,
                                              int y) {

  /* GstElement *e = ges_track_element_get_element (track_element); */

  /* /\* g_print("name: %s\n", GST_ELEMENT_NAME(e)); *\/ */

  /* GList *l = GST_BIN_CHILDREN(GST_BIN(e)); */
  /* GList *cur = l; */
  /* while (cur != NULL) { */
  /*     GstElement *item = GST_ELEMENT(cur->data); */
  /*     gchar* name = GST_ELEMENT_NAME(item); */
  /*     /\* g_print(">>>> %s\n", name); *\/ */
  /*     if (strcmp("titlesrc-bin", name) == 0) { */

  /*             GList *la = GST_BIN_CHILDREN(GST_BIN(item)); */
  /*             GList *cura = la; */
  /*             while (cura != NULL) { */
  /*                 GstElement *item = GST_ELEMENT(cura->data); */
  /*                 gchar* name = GST_ELEMENT_NAME(item); */
  /*                 /\* g_print(":::: %s\n", name); *\/ */
  /*                 if (strcmp(name, "titlesrc-bg") == 0) { */
  /*                     /\* GstElement *filter = gst_element_factory_make
   * ("capsfilter", "filter"); *\/ */
  /*                     /\* GstCaps *filtercaps = gst_caps_new_simple
   * ("video/x-raw", *\/ */
  /*                     /\* "width", G_TYPE_INT, DESIRED_WIDTH, *\/ */
  /*                     /\* "height", G_TYPE_INT, DESIRED_HEIGHT, *\/ */
  /*                     /\*                                            NULL);
   * *\/ */
  /*                     /\* g_object_set (G_OBJECT (filter), "caps",
   * filtercaps, NULL); *\/ */

  /*                     /\* if (!gst_element_link (item, filter)) { *\/ */
  /*                     /\*     g_printerr ("Elements could not be
   * linked.\n");
   * *\/ */
  /*                     /\* } *\/ */

  /*                 } */
  /*                 cura = cura->next; */
  /*             } */

  /*     } */
  /*     cur = cur->next; */
  /* } */

  ges_track_element_set_child_properties(
      track_element, "text",
      "", /* Title text usually. Since we don't really add a title, but
             instead use the surface to render text effects, we render an
             empty string
           */
      "background", backgroundColor, "shaded-background", FALSE, "font-desc",
      "fira sans 24", "valignment",
      0x00000003, /* Absolute position clamped to canvas (0x00000003) position
                   */
      /* "deltax", -10, Has not worked with absolute, center or position for
         some reason.*/
      /* "x-absolute", 0.0, /\* Must be in float *\/ */
      "halignment", 0x5, "posx", x, "posy", y, NULL);
}

void caml_set_interpolation_cubic_mode(GstControlSource **csource) {
  g_object_set((GstControlSource *)csource,
               /* "mode", GST_INTERPOLATION_MODE_CUBIC, */
               "mode", GST_INTERPOLATION_MODE_CUBIC_MONOTONIC,
               /* "mode", GST_INTERPOLATION_MODE_LINEAR, */
               NULL);
}

GstControlSource **caml_get_control_binding_source(void *control_binding) {
  GstControlSource *csource;
  g_object_get(control_binding, "control_source", &csource, NULL);
  return (GstControlSource **)csource;
}

gchar *caml_get_effect_bin_description(void *effect) {
  gchar *desc;
  g_object_get(effect, "bin-description", &desc, NULL);
  return desc;
}

void caml_interpolate(void *control_binding, float start, float duration,
                      float initial, float final) {
  GstControlSource *csource;
  g_object_get(control_binding, "control_source", &csource, NULL);

  if (gst_timed_value_control_source_set((GstTimedValueControlSource *)csource,
                                         0, 0) != TRUE) {
    g_print("Initial failed\n");
  };

  if (gst_timed_value_control_source_set((GstTimedValueControlSource *)csource,
                                         1.5 * GST_SECOND, 100) != TRUE) {
    g_print("Final failed\n");
  }
}

char *caml_get_control_binding_name(void *control_binding) {
  char *name;
  g_object_get(control_binding, "name", &name, NULL);
  return name;
}
