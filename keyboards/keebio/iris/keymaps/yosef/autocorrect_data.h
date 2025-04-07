// Copyright 2024 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

/*******************************************************************************
  88888888888 888      d8b                .d888 d8b 888               d8b
      888     888      Y8P               d88P"  Y8P 888               Y8P
      888     888                        888        888
      888     88888b.  888 .d8888b       888888 888 888  .d88b.       888 .d8888b
      888     888 "88b 888 88K           888    888 888 d8P  Y8b      888 88K
      888     888  888 888 "Y8888b.      888    888 888 88888888      888 "Y8888b.
      888     888  888 888      X88      888    888 888 Y8b.          888      X88
      888     888  888 888  88888P'      888    888 888  "Y8888       888  88888P'
                                                        888                 888
                                                        888                 888
                                                        888                 888
     .d88b.   .d88b.  88888b.   .d88b.  888d888 8888b.  888888 .d88b.   .d88888
    d88P"88b d8P  Y8b 888 "88b d8P  Y8b 888P"      "88b 888   d8P  Y8b d88" 888
    888  888 88888888 888  888 88888888 888    .d888888 888   88888888 888  888
    Y88b 888 Y8b.     888  888 Y8b.     888    888  888 Y88b. Y8b.     Y88b 888
     "Y88888  "Y8888  888  888  "Y8888  888    "Y888888  "Y888 "Y8888   "Y88888
         888
    Y8b d88P
     "Y88P"
*******************************************************************************/

#pragma once

// Autocorrection dictionary (3 entries):
//   retrun -> return
//   yosefb -> yosef.bayoudeizouka
//   ayosef -> a-yosef.bayoudeizouk

#define AUTOCORRECT_MIN_LENGTH 6 // "retrun"
#define AUTOCORRECT_MAX_LENGTH 6 // "retrun"
#define DICTIONARY_SIZE 70

static const uint8_t autocorrect_data[DICTIONARY_SIZE] PROGMEM = {
    0x45, 0x0A, 0x00, 0x09, 0x20, 0x00, 0x11, 0x3B, 0x00, 0x00, 0x09, 0x08, 0x16, 0x12, 0x1C, 0x00,
    0x80, 0x2E, 0x62, 0x61, 0x79, 0x6F, 0x75, 0x64, 0x65, 0x69, 0x7A, 0x6F, 0x75, 0x6B, 0x61, 0x00,
    0x08, 0x16, 0x12, 0x1C, 0x04, 0x00, 0x84, 0x2D, 0x79, 0x6F, 0x73, 0x65, 0x66, 0x2E, 0x62, 0x61,
    0x79, 0x6F, 0x75, 0x64, 0x65, 0x69, 0x7A, 0x6F, 0x75, 0x6B, 0x00, 0x18, 0x15, 0x17, 0x08, 0x15,
    0x00, 0x82, 0x75, 0x72, 0x6E, 0x00
};
