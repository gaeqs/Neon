#include "utils_impl.glsl"

bool bounded (vec2 v, float min, float max) {
    return bounded_impl(v, min, max);
}