Creating mrb_values from C values
=================================

Use the macros from the boxing_*.h files.

```C
#define SET_NIL_VALUE(r)         BOXNIX_SET_VALUE(r, MRB_TT_FALSE, value.i, 0)
#define SET_FALSE_VALUE(r)       BOXNIX_SET_VALUE(r, MRB_TT_FALSE, value.i, 1)
#define SET_TRUE_VALUE(r)        BOXNIX_SET_VALUE(r, MRB_TT_TRUE, value.i, 1)
#define SET_BOOL_VALUE(r,b)      BOXNIX_SET_VALUE(r, b ? MRB_TT_TRUE : MRB_TT_FALSE, value.i, 1)
#define SET_INT_VALUE(r,n)       BOXNIX_SET_VALUE(r, MRB_TT_FIXNUM, value.i, (n))
#define SET_FLOAT_VALUE(mrb,r,v) BOXNIX_SET_VALUE(r, MRB_TT_FLOAT, value.f, (v))
#define SET_SYM_VALUE(r,v)       BOXNIX_SET_VALUE(r, MRB_TT_SYMBOL, value.sym, (v))
#define SET_OBJ_VALUE(r,v)       BOXNIX_SET_VALUE(r, (((struct RObject*)(v))->tt), value.p, (v))
#define SET_PROC_VALUE(r,v)      BOXNIX_SET_VALUE(r, MRB_TT_PROC, value.p, v)
#define SET_CPTR_VALUE(mrb,r,v)  BOXNIX_SET_VALUE(r, MRB_TT_CPTR, value.p, v)
#define SET_UNDEF_VALUE(r)       BOXNIX_SET_VALUE(r, MRB_TT_UNDEF, value.i, 0)
```

MRuby Data Types
================

It's sometimes helpful to look at the structure of these mrb data types to see how they relate.
For example, you may see an RClass variable cast to an object via `mruby_obj_value` like so:

```C
mrb_value v8_value = mrb_funcall(mrb, mrb_obj_value(mrb_cef.v8_value_class), "new", 0);
```

This allows you to treat the `RClass` variable `mrb_cef.v8_value_class` as an `mrb_value`
(the correct param type for mrb_funcall). Seeing the similarities between the structures
can make clear why casts like these are appropriate.

```C
/**
 * mrb_values either hold a symbol, float, or int directly, or point
 * to some kind of RBasic/RObject/RClass
 * See: boxing_no.h
 */
typedef struct mrb_value {
  union {
    mrb_float f;
    void *p;
    mrb_int i;
    mrb_sym sym;
  } value;
  enum mrb_vtype tt;
} mrb_value;

/**
 * See: object.h
 */
#define MRB_OBJECT_HEADER \
  enum mrb_vtype tt:8;\
  uint32_t color:3;\
  uint32_t flags:21;\
  struct RClass *c;\
  struct RBasic *gcnext

struct RBasic {
  MRB_OBJECT_HEADER;
};

struct RObject {
  MRB_OBJECT_HEADER;
  struct iv_tbl *iv;
};

struct RClass {
  MRB_OBJECT_HEADER;
  struct iv_tbl *iv;
  struct kh_mt *mt;
  struct RClass *super;
};

struct RFiber {
  MRB_OBJECT_HEADER;
  struct mrb_context *cxt;
};
```

Booleans
========

Sometimes you get a value from Ruby and want to see if it evaluates as true (most values) or false (nil or false).

For this, use `mrb_test`

Here's some needless implementation details...

```
// value.h
#define mrb_bool(o)   (mrb_type(o) != MRB_TT_FALSE)
#define mrb_test(o)   mrb_bool(o)

// boxing_no.h
#define mrb_fixnum(o)   (o).value.i

// class.h (notice that nil and false share type of MRB_TT_FALSE, and only differ in their fixnum value)
static inline struct RClass*
mrb_class(mrb_state *mrb, mrb_value v)
{
  switch (mrb_type(v)) {
  case MRB_TT_FALSE:
    if (mrb_fixnum(v))
      return mrb->false_class;
    return mrb->nil_class;
  case MRB_TT_TRUE:
    return mrb->true_class;
  case MRB_TT_SYMBOL:
    return mrb->symbol_class;
  case MRB_TT_FIXNUM:
    return mrb->fixnum_class;
  case MRB_TT_FLOAT:
    return mrb->float_class;
  case MRB_TT_CPTR:
    return mrb->object_class;
  case MRB_TT_ENV:
    return NULL;
  default:
    return mrb_obj_ptr(v)->c;
  }
}
```

mrb_get_args
============

```C
int mrb_get_args(mrb_state *mrb, const char *format, ...);
```

Used to read the arguments to a Ruby function defined in C.

char|mruby type|retrieve types|note
:---:|----------|--------------|---
`o`|`Object`|`mrb_value`|Could be used to retreive any type of argument
`C`|`Class`/`Module`|`mrb_value`|
`S`|`String`|`mrb_value`|
`A`|`Array`|`mrb_value`|
`H`|`Hash`|`mrb_value`|
`s`|`String`|`char*`, `mrb_int`|
`z`|`String`|`char*`|
`a`|`Array`|`mrb_value*`, `mrb_int`|
`f`|`Float`|`mrb_float`|
`i`|`Integer`|`mrb_int`|
`b`|boolean|`mrb_bool`|
`n`|`Symbol`|`mrb_sym`|
`&`|block|`mrb_value`|
`*`|rest arguments|`mrb_value*`, `mrb_int`|Receive the rest of arguments as an array.
<code>&#124;</code>|optional||After this spec following specs would be optional.
`?`|optional given|`mrb_bool`|True if preceding argument is given. Used to check optional argument is given.

The passing variadic arguments must be a pointer of retrieving type.

Ex:

```C
static mrb_value
mrb_cef_v8_create_string(mrb_state *mrb, mrb_value self) {
  mrb_value rbString;
  mrb_get_args(mrb, "S", &rbString);
  char* cString = mrb_str_to_cstr(mrb, rbString);
  ...
```

Data_Wrap_Struct
================

```C
#define Data_Wrap_Struct(mrb,klass,type,ptr)\
  mrb_data_object_alloc(mrb,klass,ptr,type)
```

```C
MRB_API struct RData *mrb_data_object_alloc(mrb_state *mrb, struct RClass* klass, void *datap, const mrb_data_type *type);
```

Data wrapping is used to wrap a C struct in a Ruby object.

Generally, when you wrap a struct, you'll perform the following steps:

- Define a freeing function for your data structure
- Define an `mrb_data_type`
  + This pretty much just holds a reference to the freeing function, and a label for debugging
- Call `Data_Wrap_Struct` with a pointer to your stuct, and the `mrb_data_type` you've defined
  + `Data_Wrap_Struct` is a maco that delegates to `mrb_data_object_alloc`, which you may use directly instead
  + You will also specify the Ruby class to wrap the object in.
    - I tend to just use `mrb->object_class` since I'll asign this value as a
      hidden instance variable on some other object. This may not be necessary.
- Take the newly wrapped object and assign it as a "hidden" instance variable on some other object

After this has been done, you can read the object from the hidden instance variable
from your C code, unwrap it with `DATA_PTR(the_mrb_value)`, and call functions on it.

Ex:

```C
/* Defining the freeing function */
static void free_cef_ref_ptr(mrb_state* mrb, void* ptr) {
   free(ptr);
};

/* Defining the mrb_data_type */
static const mrb_data_type cef_v8value_data_type = {
   "cef_v8value_t", free_cef_ref_ptr
};

/* This function creates the wrapped object. It is bound to a Ruby class' method */
static mrb_value
mrb_cef_v8_create_undefined(mrb_state *mrb, mrb_value self) {
   /* Creating the object I wish to wrap */
   CefRefPtr<CefV8Value>* ptr = new CefRefPtr<CefV8Value>();

   /* Getting a pointer to the object */
   *ptr = CefV8Value::CreateUndefined();

   /* Wrapping the native object as a Ruby Object */
   RData* data = mrb_data_object_alloc(mrb, mrb->object_class, ptr, &cef_v8value_data_type);

   /* Creating the container object */
   mrb_value v8_value = mrb_funcall(mrb, mrb_obj_value(mrb_cef.v8_value_class), "new", 0);

   /* Setting the hidden instance variable. Ruby code wont see it because it doesn't start with '@' */
   mrb_iv_set(mrb, v8_value, mrb_intern_cstr(mrb, "value"), mrb_obj_value(data));

   return v8_value;
}

/* This function unwraps the object for use from C/C++ code */
CefRefPtr<CefV8Value>
mrb_cef_v8_value_unwrap(mrb_state* mrb, mrb_value cef_v8value){
   mrb_value wrapped_value = mrb_iv_get(mrb, cef_v8value, mrb_intern_cstr(mrb, "value"));
   return *(CefRefPtr<CefV8Value>*) DATA_PTR(wrapped_value);
}

```
