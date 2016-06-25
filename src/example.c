#include <string.h>
#include <wand/magick_wand.h>
#include <mruby.h>
#include <mruby/variable.h>
#include <mruby/class.h>
#include <mruby/data.h>

typedef struct {
  MagickWand *wand;
} mrb_thumbnail;

static mrb_value mrb_thumbnail_initialize(mrb_state *mrb, mrb_value self);
static void mrb_thumbnail_free(mrb_state *mrb, void *p);
static char *mrb_thumbnail_get_error_message(MagickWand *wand, char *message, size_t message_len);
static mrb_value mrb_thumbnail_load_file(mrb_state *mrb, mrb_value self);
static mrb_value mrb_thumbnail_get_height(mrb_state *mrb, mrb_value self);
static mrb_value mrb_thumbnail_get_width(mrb_state *mrb, mrb_value self);
static mrb_value mrb_thumbnail_resize(mrb_state *mrb, mrb_value self);
static mrb_value mrb_thumbnail_write_file(mrb_state *mrb, mrb_value self);

static const mrb_data_type mrb_thumbnail_type = {
  "mrb_thumbnail", mrb_thumbnail_free,
};

static mrb_value
mrb_thumbnail_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_thumbnail *thumbnail;

  thumbnail = (mrb_thumbnail *)malloc(sizeof(mrb_thumbnail));
  if (thumbnail == NULL) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "cannot allocate memory");
  }

  thumbnail->wand = NewMagickWand();
  if (thumbnail->wand == NULL) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "cannot allocate memory");
  }

  DATA_PTR(self) = thumbnail;
  DATA_TYPE(self) = &mrb_thumbnail_type;

  return self;
}

static void
mrb_thumbnail_free(mrb_state *mrb, void *p)
{
  mrb_thumbnail *thumbnail = (mrb_thumbnail *)p;

  if (thumbnail->wand != NULL) {
    DestroyMagickWand(thumbnail->wand);
  }

  free(thumbnail);
}

static char*
mrb_thumbnail_get_error_message(MagickWand *wand, char *message, size_t message_len)
{
  char *description;
  ExceptionType excep;

  description = MagickGetException(wand, &excep);

  strncpy(message, description, message_len);

  MagickRelinquishMemory(description);

  return message;
}

static mrb_value
mrb_thumbnail_load_file(mrb_state *mrb, mrb_value self)
{
  mrb_thumbnail *thumbnail;
  char *filename;
  MagickBooleanType ret;
  char err_message[1024];

  mrb_get_args(mrb, "z", &filename);

  thumbnail = mrb_get_datatype(mrb, self, &mrb_thumbnail_type);
  if (thumbnail == NULL) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid argument");
  }

  ret = MagickReadImage(thumbnail->wand, filename);
  if (ret != MagickTrue) {
    err_message[sizeof(err_message) - 1] = '\0';
    mrb_raise(mrb, E_RUNTIME_ERROR, mrb_thumbnail_get_error_message(thumbnail->wand, err_message, sizeof(err_message) - 1));
  }

  return self;
}

static mrb_value
mrb_thumbnail_get_height(mrb_state *mrb, mrb_value self)
{
  mrb_thumbnail *thumbnail;

  thumbnail = mrb_get_datatype(mrb, self, &mrb_thumbnail_type);
  if (thumbnail == NULL) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid argument");
  }

  return mrb_fixnum_value(MagickGetImageHeight(thumbnail->wand));
}

static mrb_value
mrb_thumbnail_get_width(mrb_state *mrb, mrb_value self)
{
  mrb_thumbnail *thumbnail;

  thumbnail = mrb_get_datatype(mrb, self, &mrb_thumbnail_type);
  if (thumbnail == NULL) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid argument");
  }
 
  return mrb_fixnum_value(MagickGetImageWidth(thumbnail->wand));
}

static mrb_value
mrb_thumbnail_resize(mrb_state *mrb, mrb_value self)
{
  mrb_thumbnail *thumbnail;
  mrb_int width;
  mrb_int height;
  MagickBooleanType ret;
  char err_message[1024];

  mrb_get_args(mrb, "ii", &width, &height);

  thumbnail = mrb_get_datatype(mrb, self, &mrb_thumbnail_type);
  if (thumbnail == NULL) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid argument");
  }

  ret = MagickResizeImage(thumbnail->wand, width, height, LanczosFilter, 1);
  if (ret != MagickTrue) {
    err_message[sizeof(err_message) - 1] = '\0';
    mrb_raise(mrb, E_RUNTIME_ERROR, mrb_thumbnail_get_error_message(thumbnail->wand, err_message, sizeof(err_message) - 1));
  }

  return self;
}

static mrb_value
mrb_thumbnail_write_file(mrb_state *mrb, mrb_value self)
{
  mrb_thumbnail *thumbnail;
  char *filename;
  MagickBooleanType ret;
  char err_message[1024];

  mrb_get_args(mrb, "z", &filename);

  thumbnail = mrb_get_datatype(mrb, self, &mrb_thumbnail_type);
  if (thumbnail == NULL) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid argument");
  }

  ret = MagickSetImageCompressionQuality(thumbnail->wand, 95);
  if (ret != MagickTrue) {
    err_message[sizeof(err_message) - 1] = '\0';
    mrb_raise(mrb, E_RUNTIME_ERROR, mrb_thumbnail_get_error_message(thumbnail->wand, err_message, sizeof(err_message) - 1));
  }

  ret = MagickWriteImage(thumbnail->wand, filename);
  if (ret != MagickTrue) {
    err_message[sizeof(err_message) - 1] = '\0';
    mrb_raise(mrb, E_RUNTIME_ERROR, mrb_thumbnail_get_error_message(thumbnail->wand, err_message, sizeof(err_message) - 1));
  }

  return self;
}

void
mrb_mruby_thumbnail_gem_init(mrb_state* mrb) {
  struct RClass *class_thumbnail;

  MagickWandGenesis();

  class_thumbnail = mrb_define_class(mrb, "Thumbnail", mrb->object_class);
  MRB_SET_INSTANCE_TT(class_thumbnail, MRB_TT_DATA);

  mrb_define_method(mrb, class_thumbnail, "initialize", mrb_thumbnail_initialize, MRB_ARGS_NONE());
  mrb_define_method(mrb, class_thumbnail, "load_file", mrb_thumbnail_load_file, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, class_thumbnail, "height", mrb_thumbnail_get_height, MRB_ARGS_NONE());
  mrb_define_method(mrb, class_thumbnail, "width", mrb_thumbnail_get_width, MRB_ARGS_NONE());
  mrb_define_method(mrb, class_thumbnail, "resize", mrb_thumbnail_resize, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, class_thumbnail, "write_file", mrb_thumbnail_write_file, MRB_ARGS_REQ(1));
}

void
mrb_mruby_thumbnail_gem_final(mrb_state* mrb) {
}

