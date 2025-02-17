#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

#include "puppies/fifo_coder.hpp"

using namespace common::puppies::fifo;

size_t handle_encoded_fifo(std::array<uint16_t, MODBUS_FIFO_LEN> &fifo);
