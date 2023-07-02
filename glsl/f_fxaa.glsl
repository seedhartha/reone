#define FXAA_PS 5
#define FXAA_P0 1.0
#define FXAA_P1 1.5
#define FXAA_P2 2.0
#define FXAA_P3 4.0
#define FXAA_P4 12.0

const float SUBPIX = 0.75;
const float EDGE_THRESHOLD = 0.166;
const float EDGE_THRESHOLD_MIN = 0.0833;

uniform sampler2D sMainTex;

noperspective in vec2 fragUV1;

out vec4 fragColor;

void main() {
    vec2 posM;
    posM.x = fragUV1.x;
    posM.y = fragUV1.y;
    vec4 rgbaM = textureLod(sMainTex, posM, 0.0);
    float lumaM = rgbToLuma(rgbaM.rgb);
    float lumaS = rgbToLuma(textureLodOffset(sMainTex, posM, 0.0, ivec2(0, 1)).rgb);
    float lumaE = rgbToLuma(textureLodOffset(sMainTex, posM, 0.0, ivec2(1, 0)).rgb);
    float lumaN = rgbToLuma(textureLodOffset(sMainTex, posM, 0.0, ivec2(0, -1)).rgb);
    float lumaW = rgbToLuma(textureLodOffset(sMainTex, posM, 0.0, ivec2(-1, 0)).rgb);

    float maxSM = max(lumaS, lumaM);
    float minSM = min(lumaS, lumaM);
    float maxESM = max(lumaE, maxSM);
    float minESM = min(lumaE, minSM);
    float maxWN = max(lumaN, lumaW);
    float minWN = min(lumaN, lumaW);
    float rangeMax = max(maxWN, maxESM);
    float rangeMin = min(minWN, minESM);
    float rangeMaxScaled = rangeMax * EDGE_THRESHOLD;
    float range = rangeMax - rangeMin;
    float rangeMaxClamped = max(EDGE_THRESHOLD_MIN, rangeMaxScaled);
    bool earlyExit = range < rangeMaxClamped;
    if (earlyExit) {
        fragColor = rgbaM;
        return;
    }

    float lumaNW = rgbToLuma(textureLodOffset(sMainTex, posM, 0.0, ivec2(-1, -1)).rgb);
    float lumaSE = rgbToLuma(textureLodOffset(sMainTex, posM, 0.0, ivec2(1, 1)).rgb);
    float lumaNE = rgbToLuma(textureLodOffset(sMainTex, posM, 0.0, ivec2(1, -1)).rgb);
    float lumaSW = rgbToLuma(textureLodOffset(sMainTex, posM, 0.0, ivec2(-1, 1)).rgb);

    float lumaNS = lumaN + lumaS;
    float lumaWE = lumaW + lumaE;
    float subpixRcpRange = 1.0 / range;
    float subpixNSWE = lumaNS + lumaWE;
    float edgeHorz1 = (-2.0 * lumaM) + lumaNS;
    float edgeVert1 = (-2.0 * lumaM) + lumaWE;

    float lumaNESE = lumaNE + lumaSE;
    float lumaNWNE = lumaNW + lumaNE;
    float edgeHorz2 = (-2.0 * lumaE) + lumaNESE;
    float edgeVert2 = (-2.0 * lumaN) + lumaNWNE;

    float lumaNWSW = lumaNW + lumaSW;
    float lumaSWSE = lumaSW + lumaSE;
    float edgeHorz4 = (abs(edgeHorz1) * 2.0) + abs(edgeHorz2);
    float edgeVert4 = (abs(edgeVert1) * 2.0) + abs(edgeVert2);
    float edgeHorz3 = (-2.0 * lumaW) + lumaNWSW;
    float edgeVert3 = (-2.0 * lumaS) + lumaSWSE;
    float edgeHorz = abs(edgeHorz3) + edgeHorz4;
    float edgeVert = abs(edgeVert3) + edgeVert4;

    float subpixNWSWNESE = lumaNWSW + lumaNESE;
    float lengthSign = uScreenResolutionRcp.x;
    bool horzSpan = edgeHorz >= edgeVert;
    float subpixA = subpixNSWE * 2.0 + subpixNWSWNESE;

    if (!horzSpan)
        lumaN = lumaW;
    if (!horzSpan)
        lumaS = lumaE;
    if (horzSpan)
        lengthSign = uScreenResolutionRcp.y;
    float subpixB = (subpixA * (1.0 / 12.0)) - lumaM;

    float gradientN = lumaN - lumaM;
    float gradientS = lumaS - lumaM;
    float lumaNN = lumaN + lumaM;
    float lumaSS = lumaS + lumaM;
    bool pairN = abs(gradientN) >= abs(gradientS);
    float gradient = max(abs(gradientN), abs(gradientS));
    if (pairN)
        lengthSign = -lengthSign;
    float subpixC = clamp(abs(subpixB) * subpixRcpRange, 0.0, 1.0);

    vec2 posB;
    posB.x = posM.x;
    posB.y = posM.y;
    vec2 offNP;
    offNP.x = (!horzSpan) ? 0.0 : uScreenResolutionRcp.x;
    offNP.y = (horzSpan) ? 0.0 : uScreenResolutionRcp.y;
    if (!horzSpan)
        posB.x += lengthSign * 0.5;
    if (horzSpan)
        posB.y += lengthSign * 0.5;

    vec2 posN;
    posN.x = posB.x - offNP.x * FXAA_P0;
    posN.y = posB.y - offNP.y * FXAA_P0;
    vec2 posP;
    posP.x = posB.x + offNP.x * FXAA_P0;
    posP.y = posB.y + offNP.y * FXAA_P0;
    float subpixD = ((-2.0) * subpixC) + 3.0;
    float lumaEndN = rgbToLuma(textureLod(sMainTex, posN, 0.0).rgb);
    float subpixE = subpixC * subpixC;
    float lumaEndP = rgbToLuma(textureLod(sMainTex, posP, 0.0).rgb);

    if (!pairN)
        lumaNN = lumaSS;
    float gradientScaled = gradient * 1.0 / 4.0;
    float lumaMM = lumaM - lumaNN * 0.5;
    float subpixF = subpixD * subpixE;
    bool lumaMLTZero = lumaMM < 0.0;

    lumaEndN -= lumaNN * 0.5;
    lumaEndP -= lumaNN * 0.5;
    bool doneN = abs(lumaEndN) >= gradientScaled;
    bool doneP = abs(lumaEndP) >= gradientScaled;
    if (!doneN)
        posN.x -= offNP.x * FXAA_P1;
    if (!doneN)
        posN.y -= offNP.y * FXAA_P1;
    bool doneNP = (!doneN) || (!doneP);
    if (!doneP)
        posP.x += offNP.x * FXAA_P1;
    if (!doneP)
        posP.y += offNP.y * FXAA_P1;

    if (doneNP) {
        if (!doneN)
            lumaEndN = rgbToLuma(textureLod(sMainTex, posN.xy, 0.0).rgb);
        if (!doneP)
            lumaEndP = rgbToLuma(textureLod(sMainTex, posP.xy, 0.0).rgb);
        if (!doneN)
            lumaEndN = lumaEndN - lumaNN * 0.5;
        if (!doneP)
            lumaEndP = lumaEndP - lumaNN * 0.5;
        doneN = abs(lumaEndN) >= gradientScaled;
        doneP = abs(lumaEndP) >= gradientScaled;
        if (!doneN)
            posN.x -= offNP.x * FXAA_P2;
        if (!doneN)
            posN.y -= offNP.y * FXAA_P2;
        doneNP = (!doneN) || (!doneP);
        if (!doneP)
            posP.x += offNP.x * FXAA_P2;
        if (!doneP)
            posP.y += offNP.y * FXAA_P2;

#if (FXAA_PS > 3)
        if (doneNP) {
            if (!doneN)
                lumaEndN = rgbToLuma(textureLod(sMainTex, posN.xy, 0.0).rgb);
            if (!doneP)
                lumaEndP = rgbToLuma(textureLod(sMainTex, posP.xy, 0.0).rgb);
            if (!doneN)
                lumaEndN = lumaEndN - lumaNN * 0.5;
            if (!doneP)
                lumaEndP = lumaEndP - lumaNN * 0.5;
            doneN = abs(lumaEndN) >= gradientScaled;
            doneP = abs(lumaEndP) >= gradientScaled;
            if (!doneN)
                posN.x -= offNP.x * FXAA_P3;
            if (!doneN)
                posN.y -= offNP.y * FXAA_P3;
            doneNP = (!doneN) || (!doneP);
            if (!doneP)
                posP.x += offNP.x * FXAA_P3;
            if (!doneP)
                posP.y += offNP.y * FXAA_P3;

#if (FXAA_PS > 4)
            if (doneNP) {
                if (!doneN)
                    lumaEndN = rgbToLuma(textureLod(sMainTex, posN.xy, 0.0).rgb);
                if (!doneP)
                    lumaEndP = rgbToLuma(textureLod(sMainTex, posP.xy, 0.0).rgb);
                if (!doneN)
                    lumaEndN = lumaEndN - lumaNN * 0.5;
                if (!doneP)
                    lumaEndP = lumaEndP - lumaNN * 0.5;
                doneN = abs(lumaEndN) >= gradientScaled;
                doneP = abs(lumaEndP) >= gradientScaled;
                if (!doneN)
                    posN.x -= offNP.x * FXAA_P4;
                if (!doneN)
                    posN.y -= offNP.y * FXAA_P4;
                doneNP = (!doneN) || (!doneP);
                if (!doneP)
                    posP.x += offNP.x * FXAA_P4;
                if (!doneP)
                    posP.y += offNP.y * FXAA_P4;

#if (FXAA_PS > 5)
                if (doneNP) {
                    if (!doneN)
                        lumaEndN = rgbToLuma(textureLod(sMainTex, posN.xy, 0.0).rgb);
                    if (!doneP)
                        lumaEndP = rgbToLuma(textureLod(sMainTex, posP.xy, 0.0).rgb);
                    if (!doneN)
                        lumaEndN = lumaEndN - lumaNN * 0.5;
                    if (!doneP)
                        lumaEndP = lumaEndP - lumaNN * 0.5;
                    doneN = abs(lumaEndN) >= gradientScaled;
                    doneP = abs(lumaEndP) >= gradientScaled;
                    if (!doneN)
                        posN.x -= offNP.x * FXAA_P5;
                    if (!doneN)
                        posN.y -= offNP.y * FXAA_P5;
                    doneNP = (!doneN) || (!doneP);
                    if (!doneP)
                        posP.x += offNP.x * FXAA_P5;
                    if (!doneP)
                        posP.y += offNP.y * FXAA_P5;

#if (FXAA_PS > 6)
                    if (doneNP) {
                        if (!doneN)
                            lumaEndN = rgbToLuma(textureLod(sMainTex, posN.xy, 0.0).rgb);
                        if (!doneP)
                            lumaEndP = rgbToLuma(textureLod(sMainTex, posP.xy, 0.0).rgb);
                        if (!doneN)
                            lumaEndN = lumaEndN - lumaNN * 0.5;
                        if (!doneP)
                            lumaEndP = lumaEndP - lumaNN * 0.5;
                        doneN = abs(lumaEndN) >= gradientScaled;
                        doneP = abs(lumaEndP) >= gradientScaled;
                        if (!doneN)
                            posN.x -= offNP.x * FXAA_P6;
                        if (!doneN)
                            posN.y -= offNP.y * FXAA_P6;
                        doneNP = (!doneN) || (!doneP);
                        if (!doneP)
                            posP.x += offNP.x * FXAA_P6;
                        if (!doneP)
                            posP.y += offNP.y * FXAA_P6;

#if (FXAA_PS > 7)
                        if (doneNP) {
                            if (!doneN)
                                lumaEndN = rgbToLuma(textureLod(sMainTex, posN.xy, 0.0).rgb);
                            if (!doneP)
                                lumaEndP = rgbToLuma(textureLod(sMainTex, posP.xy, 0.0).rgb);
                            if (!doneN)
                                lumaEndN = lumaEndN - lumaNN * 0.5;
                            if (!doneP)
                                lumaEndP = lumaEndP - lumaNN * 0.5;
                            doneN = abs(lumaEndN) >= gradientScaled;
                            doneP = abs(lumaEndP) >= gradientScaled;
                            if (!doneN)
                                posN.x -= offNP.x * FXAA_P7;
                            if (!doneN)
                                posN.y -= offNP.y * FXAA_P7;
                            doneNP = (!doneN) || (!doneP);
                            if (!doneP)
                                posP.x += offNP.x * FXAA_P7;
                            if (!doneP)
                                posP.y += offNP.y * FXAA_P7;

#if (FXAA_PS > 8)
                            if (doneNP) {
                                if (!doneN)
                                    lumaEndN = rgbToLuma(textureLod(sMainTex, posN.xy, 0.0).rgb);
                                if (!doneP)
                                    lumaEndP = rgbToLuma(textureLod(sMainTex, posP.xy, 0.0).rgb);
                                if (!doneN)
                                    lumaEndN = lumaEndN - lumaNN * 0.5;
                                if (!doneP)
                                    lumaEndP = lumaEndP - lumaNN * 0.5;
                                doneN = abs(lumaEndN) >= gradientScaled;
                                doneP = abs(lumaEndP) >= gradientScaled;
                                if (!doneN)
                                    posN.x -= offNP.x * FXAA_P8;
                                if (!doneN)
                                    posN.y -= offNP.y * FXAA_P8;
                                doneNP = (!doneN) || (!doneP);
                                if (!doneP)
                                    posP.x += offNP.x * FXAA_P8;
                                if (!doneP)
                                    posP.y += offNP.y * FXAA_P8;

#if (FXAA_PS > 9)
                                if (doneNP) {
                                    if (!doneN)
                                        lumaEndN = rgbToLuma(textureLod(sMainTex, posN.xy, 0.0).rgb);
                                    if (!doneP)
                                        lumaEndP = rgbToLuma(textureLod(sMainTex, posP.xy, 0.0).rgb);
                                    if (!doneN)
                                        lumaEndN = lumaEndN - lumaNN * 0.5;
                                    if (!doneP)
                                        lumaEndP = lumaEndP - lumaNN * 0.5;
                                    doneN = abs(lumaEndN) >= gradientScaled;
                                    doneP = abs(lumaEndP) >= gradientScaled;
                                    if (!doneN)
                                        posN.x -= offNP.x * FXAA_P9;
                                    if (!doneN)
                                        posN.y -= offNP.y * FXAA_P9;
                                    doneNP = (!doneN) || (!doneP);
                                    if (!doneP)
                                        posP.x += offNP.x * FXAA_P9;
                                    if (!doneP)
                                        posP.y += offNP.y * FXAA_P9;

#if (FXAA_PS > 10)
                                    if (doneNP) {
                                        if (!doneN)
                                            lumaEndN = rgbToLuma(textureLod(sMainTex, posN.xy, 0.0).rgb);
                                        if (!doneP)
                                            lumaEndP = rgbToLuma(textureLod(sMainTex, posP.xy, 0.0).rgb);
                                        if (!doneN)
                                            lumaEndN = lumaEndN - lumaNN * 0.5;
                                        if (!doneP)
                                            lumaEndP = lumaEndP - lumaNN * 0.5;
                                        doneN = abs(lumaEndN) >= gradientScaled;
                                        doneP = abs(lumaEndP) >= gradientScaled;
                                        if (!doneN)
                                            posN.x -= offNP.x * FXAA_P10;
                                        if (!doneN)
                                            posN.y -= offNP.y * FXAA_P10;
                                        doneNP = (!doneN) || (!doneP);
                                        if (!doneP)
                                            posP.x += offNP.x * FXAA_P10;
                                        if (!doneP)
                                            posP.y += offNP.y * FXAA_P10;

#if (FXAA_PS > 11)
                                        if (doneNP) {
                                            if (!doneN)
                                                lumaEndN = rgbToLuma(textureLod(sMainTex, posN.xy, 0.0).rgb);
                                            if (!doneP)
                                                lumaEndP = rgbToLuma(textureLod(sMainTex, posP.xy, 0.0).rgb);
                                            if (!doneN)
                                                lumaEndN = lumaEndN - lumaNN * 0.5;
                                            if (!doneP)
                                                lumaEndP = lumaEndP - lumaNN * 0.5;
                                            doneN = abs(lumaEndN) >= gradientScaled;
                                            doneP = abs(lumaEndP) >= gradientScaled;
                                            if (!doneN)
                                                posN.x -= offNP.x * FXAA_P11;
                                            if (!doneN)
                                                posN.y -= offNP.y * FXAA_P11;
                                            doneNP = (!doneN) || (!doneP);
                                            if (!doneP)
                                                posP.x += offNP.x * FXAA_P11;
                                            if (!doneP)
                                                posP.y += offNP.y * FXAA_P11;
                                        }
#endif
                                    }
#endif
                                }
#endif
                            }
#endif
                        }
#endif
                    }
#endif
                }
#endif
            }
#endif
        }
#endif
    }

    float dstN = posM.x - posN.x;
    float dstP = posP.x - posM.x;
    if (!horzSpan)
        dstN = posM.y - posN.y;
    if (!horzSpan)
        dstP = posP.y - posM.y;

    bool goodSpanN = (lumaEndN < 0.0) != lumaMLTZero;
    float spanLength = (dstP + dstN);
    bool goodSpanP = (lumaEndP < 0.0) != lumaMLTZero;
    float spanLengthRcp = 1.0 / spanLength;

    bool directionN = dstN < dstP;
    float dst = min(dstN, dstP);
    bool goodSpan = directionN ? goodSpanN : goodSpanP;
    float subpixG = subpixF * subpixF;
    float pixelOffset = (dst * (-spanLengthRcp)) + 0.5;
    float subpixH = subpixG * SUBPIX;

    float pixelOffsetGood = goodSpan ? pixelOffset : 0.0;
    float pixelOffsetSubpix = max(pixelOffsetGood, subpixH);
    if (!horzSpan)
        posM.x += pixelOffsetSubpix * lengthSign;
    if (horzSpan)
        posM.y += pixelOffsetSubpix * lengthSign;
    fragColor = vec4(textureLod(sMainTex, posM, 0.0).rgb, rgbaM.a);
}
