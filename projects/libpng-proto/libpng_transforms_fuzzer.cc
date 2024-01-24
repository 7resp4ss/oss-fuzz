#include <algorithm>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>
#include <png.h>
#include <stdio.h>
namespace {

struct PngReader {
  png_structp png_ptr = nullptr;
  png_infop info_ptr = nullptr;
};

struct PngArrayStream {
  const uint8_t *data;
  size_t size;
  size_t pos;
};

void PngArrayStreamCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  PngArrayStream *stream = static_cast<PngArrayStream *>(png_get_io_ptr(png_ptr));
  if (stream->pos + length > stream->size) {
    png_error(png_ptr, "Read beyond end of stream");
  } else {
    memcpy(data, stream->data + stream->pos, length);
    stream->pos += length;
  }
}

void PngErrorHandler(png_structp png_ptr, png_const_charp error_message) {
  int a=1;
    // Do nothing or log to a file instead
}

void PngWarningHandler(png_structp png_ptr, png_const_charp warning_message) {
    // Do nothing or log to a file instead
    int a=1;
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  if (size < 8) return 0;
  if (png_sig_cmp(const_cast<png_bytep>(data), 0, 8)) return 0;

  PngReader reader;
  reader.png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, PngErrorHandler, PngWarningHandler);
  if (!reader.png_ptr) return 0;

  reader.info_ptr = png_create_info_struct(reader.png_ptr);
  if (!reader.info_ptr) {
    png_destroy_read_struct(&reader.png_ptr, NULL, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(reader.png_ptr))) {
    png_destroy_read_struct(&reader.png_ptr, &reader.info_ptr, NULL);
    return 0;
  }

  PngArrayStream stream = {data, size, 0};
  png_set_read_fn(reader.png_ptr, &stream, PngArrayStreamCallback);

  // Read the PNG header info
  png_read_info(reader.png_ptr, reader.info_ptr);

  // Example of accessing image information
  png_uint_32 img_width, img_height;
  int bit_depth, color_type, interlace_type;
  png_get_IHDR(reader.png_ptr, reader.info_ptr, &img_width, &img_height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

  // Example of setting transformations
  // Here we transform the image to an 8-bit per channel representation if it is 16-bit.
  if (bit_depth == 16) {
    png_set_strip_16(reader.png_ptr);
  }

  // Update the png info struct.
  png_read_update_info(reader.png_ptr, reader.info_ptr);

  // Allocate memory for reading the PNG data to
  png_bytepp row_pointers = new png_bytep[img_height];
  for (png_uint_32 i = 0; i < img_height; ++i) {
    row_pointers[i] = new png_byte[png_get_rowbytes(reader.png_ptr, reader.info_ptr)];
  }

  // Read the image data
  png_read_image(reader.png_ptr, row_pointers);

  // Clean up row pointers
  for (png_uint_32 i = 0; i < img_height; ++i) {
    delete[] row_pointers[i];
  }
  delete[] row_pointers;

  // Finish reading
  png_read_end(reader.png_ptr, NULL);

  // Clean up
  png_destroy_read_struct(&reader.png_ptr, &reader.info_ptr, NULL);
  return 0;
}

extern "C" const char *__asan_default_options() {
  return "detect_leaks=0";
}
