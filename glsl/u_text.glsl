const int MAX_TEXT_CHARS = 128;

struct Character {
    vec4 posScale;
    vec4 uv;
};

layout(std140) uniform Text {
    Character uTextChars[MAX_TEXT_CHARS];
};
