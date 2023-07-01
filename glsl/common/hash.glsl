float hash(vec2 p) {
    return fract(1.0e4 * sin(17.0 * p.x + 0.1 * p.y) * (0.1 + abs(sin(13.0 * p.y + p.x))));
}

float hash(vec3 p) {
    return hash(vec2(hash(p.xy), p.z));
}
