//      Copyright © 2025  Andrius Mazeikis
//
//      This program is free software: you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation, either version 3 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include <metal_stdlib>
#include <metal_texture>
#include <metal_math>
using namespace metal;

struct VertexInput {
    float2 position [[attribute(0)]];
    float2 texcoord [[attribute(1)]];
};

struct VertexOutput {
    float4 position [[position]];
    float2 texcoord;
};

constexpr sampler smp(filter::linear);
//constexpr sampler smp(filter::nearest);

vertex VertexOutput vertex_func(constant float2x2 &viewTransform [[buffer(1)]], VertexInput v_in [[stage_in]])
{
    VertexOutput v_out;
    v_out.position = float4(viewTransform * v_in.position, 0.0, 1.0);
    v_out.texcoord = v_in.texcoord;
    return v_out;
}

template<typename T>
T interp(T s0, T s1, float x)
{
    return s0 + (s1 - s0) * x;
}

fragment float4 fragment_func(VertexOutput frag_in [[stage_in]], texture2d<float, access::sample> frame [[texture(0)]])
{
//    return frame.sample(smp, frag_in.texcoord);
    auto pixel_position =
        float2(frag_in.texcoord.x * (frame.get_width() - 1),
               frag_in.texcoord.y * (frame.get_height() - 1));
    
    auto xy = floor(pixel_position);
    auto xyi = uint2(xy);
    auto s0 = frame.read(xyi + uint2(0, 1)).rgb;
    auto s1 = frame.read(xyi + uint2(1, 1)).rgb;
    auto s2 = frame.read(xyi + uint2(1, 0)).rgb;
    auto s3 = frame.read(xyi + uint2(0, 0)).rgb;
    
    auto offset = pixel_position - xy;
    
    auto s3s0 = transpose(float2x3(s3, s0));
    auto s2s1 = transpose(float2x3(s2, s1));

    auto across = interp(s3s0, s2s1, offset.x);

    auto s3s2i = float2(1, 0) * across;
    auto s0s1i = float2(0, 1) * across;
    
    auto oneminusoffsety = 1.0 - offset.y;
    auto s3s2ig = s3s2i * exp(-offset.y * offset.y * 2.5);
    auto s0s1ig = s0s1i * exp(-oneminusoffsety * oneminusoffsety * 2.5);

    return float4(s3s2ig + s0s1ig, 1.0);
}
