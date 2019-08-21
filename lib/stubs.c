#include <stdio.h>
#include <ges/ges.h>
#include <caml/custom.h>
#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>

CAMLprim value
to_ges_track(value objptr) {
    return Val_unit;
}
