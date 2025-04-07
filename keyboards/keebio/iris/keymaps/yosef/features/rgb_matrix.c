#include "rgb_matrix.h"
#include "split_util.h"

void set_rgb_sync_slave_handler(
    uint8_t in_buflen,
    const void *in_data,
    uint8_t out_buflen,
    void *out_data
) {
  SET_RGB_SYNC_data *set_rgb_sync_data = (SET_RGB_SYNC_data *)in_data;
  rgb_matrix_set_color(
      set_rgb_sync_data->index,
      set_rgb_sync_data->r,
      set_rgb_sync_data->g,
      set_rgb_sync_data->b
  );
}

void set_rgb_all_sync_slave_handler(
    uint8_t in_buflen,
    const void *in_data,
    uint8_t out_buflen,
    void *out_data
) {
  SET_RGB_ALL_SYNC_data *set_rgb_all_sync_data =
      (SET_RGB_ALL_SYNC_data *)in_data;
  rgb_matrix_set_color_all(
      set_rgb_all_sync_data->r,
      set_rgb_all_sync_data->g,
      set_rgb_all_sync_data->b
  );
}

RGB limit_brightness_rgb(RGB rgb) {
  HSV hsv = rgb_to_hsv(rgb);
  hsv = limit_brightness_hsv(hsv);
  rgb = hsv_to_rgb(hsv);
  return rgb;
}

HSV limit_brightness_hsv(HSV hsv) {
  //dprintf("RGB: MAX_BRIGHTNESS: %u\n", RGB_MATRIX_MAXIMUM_BRIGHTNESS);
  if (hsv.v > RGB_MATRIX_MAXIMUM_BRIGHTNESS) {
    hsv.v = RGB_MATRIX_MAXIMUM_BRIGHTNESS;
  }
  return hsv;
}

HSV rgb_to_hsv(RGB rgb) {
  HSV hsv;
  unsigned char rgbMin, rgbMax;

  rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b)
                         : (rgb.g < rgb.b ? rgb.g : rgb.b);
  rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b)
                         : (rgb.g > rgb.b ? rgb.g : rgb.b);

  hsv.v = rgbMax;
  if (hsv.v == 0) {
    hsv.h = 0;
    hsv.s = 0;
    return hsv;
  }

  hsv.s = 255 * (long)(rgbMax - rgbMin) / hsv.v;
  if (hsv.s == 0) {
    hsv.h = 0;
    return hsv;
  }

  if (rgbMax == rgb.r)
    hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
  else if (rgbMax == rgb.g)
    hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
  else
    hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

  return hsv;
}

void rgb_matrix_set_color_iris_rev8(
    uint8_t index,
    uint8_t r,
    uint8_t g,
    uint8_t b
) {
  if (index < 0)
    return;
  // TODO : make split count configurable
  if (index <= 34) {
    if (!isLeftHand)
      rgb_matrix_set_color_through_rpc(index, r, g, b);
    else
      rgb_matrix_set_color_native(index, r, g, b);
  } else if (index > 34 && index <= 68) {
    if (isLeftHand)
      rgb_matrix_set_color_through_rpc(index, r, g, b);
    else
      rgb_matrix_set_color_native(index, r, g, b);
  }
}

// transaction_rpc_send(); // cannot be used on slave side, causes other side to
// not respond
void rgb_matrix_set_color_through_rpc(
    uint8_t index,
    uint8_t r,
    uint8_t g,
    uint8_t b
) {
  if (!is_keyboard_master())
    return;
  SET_RGB_SYNC_data set_rgb_sync_data_local = {index, r, g, b};
  transaction_rpc_send(
      SET_RGB_SYNC,
      sizeof(set_rgb_sync_data_local),
      &set_rgb_sync_data_local
  );
}

void rgb_matrix_set_color_native(
    uint8_t index,
    uint8_t r,
    uint8_t g,
    uint8_t b
) {
  RGB color = {.r = r, .g = g, .b = b};
  color = limit_brightness_rgb(color);
  rgb_matrix_set_color(index, color.r, color.g, color.b);
}

void rgb_matrix_set_color_all_iris_rev8(uint8_t r, uint8_t g, uint8_t b) {
  RGB color = {.r = r, .g = g, .b = b};
  color = limit_brightness_rgb(color);
  rgb_matrix_set_color_all(color.r, color.g, color.b);
  if (!is_keyboard_master())
    return;
  SET_RGB_ALL_SYNC_data set_rgb_all_sync_data_local = {r, g, b};
  transaction_rpc_send(
      SET_RGB_ALL_SYNC,
      sizeof(set_rgb_all_sync_data_local),
      &set_rgb_all_sync_data_local
  );
}
