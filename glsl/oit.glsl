float OIT_getWeight(float depth, float alpha) {
    float eyeZ = (uClipNear * uClipFar) / ((uClipNear - uClipFar) * depth + uClipFar);
    return alpha * (1.0 / (1.0 + abs(eyeZ) / 100.0));
}
