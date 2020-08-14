#pragma once

#include <cstdint>
#include <limits>

using U32 = uint32_t;
using U64 = uint64_t;

using F32 = float;
using F64 = double;

static_assert(sizeof(F32) == 4);
static_assert(std::numeric_limits<F32>::is_iec559);
static_assert(sizeof(F64) == 8);
static_assert(std::numeric_limits<F64>::is_iec559);

using BudId = U32;