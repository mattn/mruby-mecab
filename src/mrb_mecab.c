#include <mruby.h>
#include <mruby/class.h>
#include <mruby/string.h>
#include <mruby/array.h>
#include <mruby/hash.h>
#include <mruby/proc.h>
#include <mruby/data.h>
#include <mruby/variable.h>
#include <string.h>
#ifndef _MSC_VER
#include <strings.h>
#endif
#include <ctype.h>
#include <mecab.h>
#include <stdio.h>
#include <stdlib.h>

static void
mrb_mecab_free(mrb_state *mrb, void *p) {
  mecab_destroy((mecab_t*) p);
}
struct mrb_data_type mrb_mecab_data_type = {
  "mecab", mrb_mecab_free 
};

static mrb_value
mrb_mecab_init(mrb_state* mrb, mrb_value self) {
  mrb_value input = mrb_nil_value();
  mecab_t* mecab = NULL;
  mrb_get_args(mrb, "|o", &input);
  if (!mrb_nil_p(input)) {
    if (mrb_type(input) == MRB_TT_STRING)
      mecab = mecab_new2(RSTRING_PTR(input));
    else if (mrb_type(input) == MRB_TT_ARRAY) {
      char** argv;
      int n, argc;
      argc = RARRAY_LEN(input);
      argv = malloc(sizeof(char*) * argc);
      for (n = 0; n < argc; n++)
        argv[n] = mrb_string_value_ptr(mrb, mrb_ary_entry(input, n));
      mecab = mecab_new(argc, argv);
      free(argv);
    } else
      mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid arguments");
  } else
      mecab = mecab_new(0, NULL);
  if (!mecab)
    mrb_raise(mrb, E_ARGUMENT_ERROR, "failed to create mecab");

  DATA_TYPE(self) = &mrb_mecab_data_type;
  DATA_PTR(self) = mecab;
  return self;
}

static mrb_value
mrb_mecab_sparse_tostr(mrb_state* mrb, mrb_value self) {
  int nbest = -1;
  char* input;
  mecab_t* mecab = (mecab_t*) DATA_PTR(self);
  mrb_get_args(mrb, "z|i", &input, &nbest);
  if (nbest < 0) {
    return mrb_str_new_cstr(mrb, mecab_sparse_tostr(mecab, input));
  }
  return mrb_str_new_cstr(mrb, mecab_nbest_sparse_tostr(mecab, nbest, input));
}

static mrb_value
mrb_mecab_sparse_toary(mrb_state* mrb, mrb_value self) {
  int n = 0, nbest = -1;
  char* input;
  mrb_value ary;
  mecab_t* mecab = (mecab_t*) DATA_PTR(self);
  mrb_get_args(mrb, "z|i", &input, &nbest);
  if (!mecab_nbest_init(mecab, input)) {
    mrb_raise(mrb, E_RUNTIME_ERROR, mecab_strerror(mecab));
  }

  ary = mrb_ary_new(mrb);
  while (nbest < 0 || n++ < nbest) {
    const char* str = mecab_nbest_next_tostr(mecab);
    if (!str) break;
    mrb_ary_push(mrb, ary, mrb_str_new_cstr(mrb, str));
  }
  return ary;
}

void
mrb_mruby_mecab_gem_init(mrb_state* mrb) {
  struct RClass* _class_mecab = mrb_define_class(mrb, "Mecab", mrb->object_class);
  MRB_SET_INSTANCE_TT(_class_mecab, MRB_TT_DATA);
  mrb_define_method(mrb, _class_mecab, "initialize", mrb_mecab_init, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_mecab, "sparse_tostr", mrb_mecab_sparse_tostr, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_mecab, "sparse_toary", mrb_mecab_sparse_toary, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
}

void
mrb_mruby_mecab_gem_final(mrb_state* mrb) {
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
