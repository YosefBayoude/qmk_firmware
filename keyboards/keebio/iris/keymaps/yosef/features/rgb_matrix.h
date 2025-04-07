#pragma once

#include "quantum.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "transactions.h"

typedef struct _SET_RGB_SYNC_data {
  int index;
  uint8_t r;
  uint8_t g;
  uint8_t b;
} SET_RGB_SYNC_data;

typedef struct _SET_RGB_ALL_SYNC_data {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} SET_RGB_ALL_SYNC_data;

void rgb_matrix_set_color_through_rpc(
    uint8_t index,
    uint8_t r,
    uint8_t g,
    uint8_t b
);

void rgb_matrix_set_color_native(
    uint8_t index,
    uint8_t r,
    uint8_t g,
    uint8_t b
);

RGB limit_brightness_rgb(RGB rgb);
HSV limit_brightness_hsv(HSV hsv);
HSV rgb_to_hsv(RGB in);

void set_rgb_sync_slave_handler(
    uint8_t in_buflen,
    const void *in_data,
    uint8_t out_buflen,
    void *out_data
);
void set_rgb_all_sync_slave_handler(
    uint8_t in_buflen,
    const void *in_data,
    uint8_t out_buflen,
    void *out_data
);
void rgb_matrix_set_color_iris_rev8(
    uint8_t index,
    uint8_t r,
    uint8_t g,
    uint8_t b
);
void rgb_matrix_set_color_all_iris_rev8(uint8_t r, uint8_t g, uint8_t b);
#ifdef __cplusplus
}
#endif
