#ifndef _CORE_H
#define _CORE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ges/ges.h>



typedef struct _LLNode
{
    void* data;
    struct _LLNode* next;
} LLNode;

typedef struct _LList
{
    LLNode *head;
} LList;

/**
 * Creates a Linked list node
 * Accepts a void pointer (any struct/data type) 
 * Returns a void pointer (same struct/data type)
 */
void* ll_node_create(void* data);
void ll_node_destroy(LLNode* node);
LList* ll_init();
void ll_destroy(LList *list);
void ll_insert(LList *list, void *data);

typedef struct
{
    LLNode *cur;
} LListIterator;

LListIterator* ll_iterator_create(LList *list);
void ll_iterator_destroy(LListIterator *iterator);
void* ll_iterator_traverse(LListIterator *iterator);













typedef struct
{
  int type;
  char *name;
} transition_type;

typedef struct
{
    guint64 current_position;
} CoreGESPipelineState;


// TODO remove
void
init_core_pipeline_state(CoreGESPipelineState* state);

// TODO remove
typedef struct
{
    const char *path;
    guint64 start;
    guint64 duration;
    guint64 inpoint;
    guint32 priority;
} CoreGESPipelineMediaDeprecated;

typedef struct
{
    const char *path;
    float start;
    float duration;
    float inpoint;
    int priority;
    const char* text;
    /* const char *transition_type; */
    /* float transition_duration; */
} CoreGESPipelineMedia;

typedef struct
{
    const char *text;
    float start;
    float duration;
    float inpoint; // TODO: Must always be zero?
    int priority;
    int color;
    int valignment;
    int halignment;
    float xpos;
    float ypos;
    char *font_desc;
} CoreGESPipelineTextOverlay;

typedef struct
{
    char *transition_nick;
    float start;
    float duration;
} CoreGESPipelineTransition;

// TODO remove
void init_core_pipeline_media(CoreGESPipelineMediaDeprecated* media,
                              const char* path,
                              guint64 start,
                              guint64 duration,
                              guint64 inpoint,
                              guint32 priority);

GESClip *
make_source (gchar * path, guint64 start, guint64 duration, guint64 inpoint,
             guint32 priority);

GESClip *
make_source_from_core_media (CoreGESPipelineMediaDeprecated* media);


/**
 * Append source to a layer without having to maintain
 * start position of every clip
 * Returns TRUE (gboolean) if successfully added clip to layer
 */
gboolean
append_source_to_layer(CoreGESPipelineState *state,
                       GESLayer *layer,
                       CoreGESPipelineMediaDeprecated *media);

GESClip *make_overlay (char *text, guint64 start, guint64 duration,
                       gint priority, guint32 color, gdouble xpos, gdouble ypos, char* font_desc);

GESTimeline*
make_timeline (gchar * nick, double tdur, gchar * patha,
    gfloat adur, gdouble ainpoint, gchar * pathb, gfloat bdur,
    gdouble binpoint, char *text, guint32 color,
               gdouble xpos, gdouble ypos);


#define DEFAULT_DURATION 5
#define DEFAULT_POS 0.5

int core_main (gchar *patha,
               gchar *pathb,

               char  *text,

               gdouble adur,
               gdouble bdur,
               gdouble tdur,

               gdouble ainpoint,
               gdouble binpoint,

               gchar *type);

char* my_args_passing_function (char *filename, gdouble a);
















///// -- Pipeline media list management --- /////

typedef struct
{
    LList* pipeline_elements;
} CoreGESPipeline;

typedef enum { CORE_MEDIA, CORE_TEXT, CORE_TRANSITION } CoreGESPipelineElementType;

typedef struct
{
    CoreGESPipelineElementType type;
    void* element;
} CoreGESPipelineElement;
    

// Internal
CoreGESPipeline* __core_init_pipeline_struct(void);
void __core_destroy_pipeline_struct(CoreGESPipeline *pipeline);
CoreGESPipeline* __core_pipeline_get(int);
CoreGESPipelineMedia* __core_pipeline_media_create(const char *path,
                                                   float start,
                                                   float duration,
                                                   float inpoint,
                                                   int priority,
                                                   const char* text);
CoreGESPipelineTextOverlay* __core_pipeline_text_overlay_create(const char *path,
                                                                float start,
                                                                float duration,
                                                                float inpoint,
                                                                int priority,
                                                                int color,
                                                                int valignment,
                                                                int halignment,
                                                                float xpos,
                                                                float ypos,
                                                                char *font_desc);

CoreGESPipelineTransition* __core_pipeline_transition_create(char *nick,
                                                             float start,
                                                             float duration);

CoreGESPipelineElement* __core_pipeline_element_create(CoreGESPipelineElementType type,
                                                       void* element);

void __core_pipeline_media_destroy(CoreGESPipelineMedia *media);
void __core_pipeline_text_overlay_destroy(CoreGESPipelineTextOverlay *text_overlay);
void __core_pipeline_transition_destroy(CoreGESPipelineTransition *transition);
int core_init_pipeline(void);
void core_pipeline_add_media(int pipeline_id,
                             const char *path,
                             float start,
                             float duration,
                             float inpoint,
                             int priority,
                             const char* text);
void core_pipeline_add_text_overlay(int pipeline_id,
                                    const char *text,
                                    float start,
                                    float duration,
                                    float inpoint,
                                    int priority,
                                    int color,
                                    int valignment,
                                    int halignment,
                                    float xpos,
                                    float ypos,
                                    char *font_desc);
void core_pipeline_add_transition(int pipeline_id,
                                  char* nick,
                                  float start,
                                  float duration);
void core_pipeline_render(int pipeline_id, char *output_path);

void* caml_to_ges_clip_from_void(void* o);
void* caml_element_to_bin(void* e);
GList* caml_elements_of_bin(void* b);
gulong caml_test_gulong();
void caml_set_clip_property(void* obj, char *property, float value);
void caml_set_clip_priority(void* obj, int value);
GESTrackType caml_ges_get_track_type(void* t);
char* caml_gst_element_name(void* e);
char* caml_ges_source_name(void *s);
gchar* caml_get_effect_bin_description(void* effect);
void caml_ges_set_track_restriction_caps(void* track, char* caps_string);
void caml_ges_title_clip_set_child_properties(void* track_element, long int backgroundColor, int x, int y);


    /* void caml_run_ges_pipeline(void *pipeline, char* output_file_path); */
void caml_run_ges_pipeline(void *pipeline, char* output_file_path, int width, int height, int timeout_seconds);
unsigned int* caml_get_media_dimensions(char *media_path);
unsigned int* caml_get_text_dimensions(char *text);
void caml_free_media_dimensions(unsigned int* media_dimensions);
void caml_free_text_dimensions(unsigned int* text_dimensions);
void* caml_to_ges_clip(void* o);
void* caml_to_ges_timeline_element(void* o);
void* caml_to_ges_track_element(void* o);
void* caml_to_ges_container(void* o);
typedef void(callback)(void *, void *, void*);
gulong caml_signal_connect(void *o, char *signal, callback *cb, void* data);
void caml_signal_disconnect(void *o, gulong handler_id);
char* caml_get_effect_description(void *e);
void caml_set_interpolation_cubic_mode(GstControlSource** csource);
GstControlSource** caml_get_control_binding_source(void* control_binding);
char* caml_get_control_binding_name(void* control_binding);


void caml_test(void *o);
void* caml_test2();
void caml_test3(void *o, void *v);




#endif
