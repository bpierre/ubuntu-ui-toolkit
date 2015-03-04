// Copyright © 2015 Canonical Ltd.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; version 3.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Author: Loïc Molinari <loic.molinari@canonical.com>

// Static flow control (branching on a uniform value) is fast on most GPUs (including ultra-low
// power ones) because it allows to use the same shader execution path for an entire draw call. We
// rely on that technique here (also known as "uber-shader" solution) to avoid the complexity of
// dealing with a multiple shaders solution.
// FIXME(loicm) Validate GPU behavior with regards to static flow control.

uniform sampler2D shapeTexture;
uniform sampler2D sourceTexture;
uniform lowp vec2 factors;
uniform lowp float sourceOpacity;
uniform lowp float distanceAA;
uniform lowp float dfdtFlip;
uniform bool textured;
uniform mediump int style;

varying mediump vec2 shapeCoord;
varying mediump vec4 sourceCoord;
varying lowp vec4 backgroundColor;

const mediump int PLAIN   = 0x08;  // 1 << 3
const mediump int SUNKEN  = 0x10;  // 1 << 4

void main(void)
{
    lowp vec4 shapeData = texture2D(shapeTexture, shapeCoord);
    lowp vec4 color = backgroundColor;

    // FIXME(loicm) Would be better to use a bitfield but bitwise ops have only been integrated in
    //     GLSL 1.3 (OpenGL 3) and GLSL ES 3 (OpenGL ES 3).
    if (textured) {
        // Blend the source over the current color.
        // FIXME(loicm) sign() is far from optimal. Call texture2D() at beginning of scope.
        lowp vec2 axisMask = -sign((sourceCoord.zw * sourceCoord.zw) - vec2(1.0));
        lowp float mask = clamp(axisMask.x + axisMask.y, 0.0, 1.0);
        lowp vec4 source = texture2D(sourceTexture, sourceCoord.st) * vec4(sourceOpacity * mask);
        color = vec4(1.0 - source.a) * color + source;
    }

    if (style == PLAIN) {
        // Get screen-space derivative of texture coordinate t representing the normalized distance
        // between 2 pixels then mask the current color with an anti-aliased and resolution
        // independent shape mask built from distance fields.
        lowp float dfdt = dFdy(shapeCoord.t);
        lowp float distanceMin = abs(dfdt) * -distanceAA + 0.5;
        lowp float distanceMax = abs(dfdt) * distanceAA + 0.5;
        color *= smoothstep(distanceMin, distanceMax, shapeData.b);

    } else if (style == SUNKEN) {
        // Get screen-space derivative of texture coordinate t representing the normalized distance
        // between 2 pixels. The vertex layout of the shape is made so that the derivative is
        // negative from top to middle and positive from middle to bottom.
        lowp float dfdt = dFdy(shapeCoord.t) * dfdtFlip;
        lowp float shapeSide = dfdt <= 0.0 ? 0.0 : 1.0;
        // Blend the shape inner shadow over the current color. The shadow color is black, its
        // translucency is stored in the texture.
        lowp float shadow = shapeData[int(shapeSide)];
        color = vec4(1.0 - shadow) * color + vec4(0.0, 0.0, 0.0, shadow);
        // Get the anti-aliased and resolution independent shape mask using distance fields.
        lowp float distanceMin = abs(dfdt) * -distanceAA + 0.5;
        lowp float distanceMax = abs(dfdt) * distanceAA + 0.5;
        lowp vec2 mask = smoothstep(distanceMin, distanceMax, shapeData.ba);
        // Get the bevel color. The bevel is made of the top mask masked with the bottom mask. A
        // gradient from the bottom (1) to the middle (0) of the shape is used to factor out values
        // resulting from the mask anti-aliasing. The bevel color is white with 60% opacity.
        lowp float bevel = (mask.x * -mask.y) + mask.x;  // -ab + a = a(1 - b)
        lowp float gradient = clamp((shapeSide * -shapeCoord.t) + shapeSide, 0.0, 1.0);
        bevel *= gradient * 0.6;
        // Mask the current color then blend the bevel over the resulting color. We simply use
        // additive blending since the bevel has already been masked.
        color = (color * vec4(mask[int(shapeSide)])) + vec4(bevel);
    }

    gl_FragColor = color * vec4(factors.x, factors.x, factors.x, factors.y);
}
