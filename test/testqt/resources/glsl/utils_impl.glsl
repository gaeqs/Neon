bool bounded_impl (vec2 v, float min, float max) {
    return v.x <= max && v.y <= max && v.x >= min && v.y >= min;
}