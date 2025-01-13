#pragma once

#include "image.h"
#include <cstdint>
#include <string>

const WORD BMPTYPE1 = 0x4D42;
const WORD BITCOUNT = 24;
const DWORD INFOSIZE = 40;

Image ReadFile(const char *file);
void SaveFile(const Image &im, const char *file);