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

//constexpr sampler smp(filter::linear);
constexpr sampler smp(filter::nearest);

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
    float3x4 samples = float3x4(exp(frame.gather(smp, frag_in.texcoord, int2(0), component::x)),
                                exp(frame.gather(smp, frag_in.texcoord, int2(0), component::y)),
                                exp(frame.gather(smp, frag_in.texcoord, int2(0), component::z)));
    
    auto offset = fract(float3x2(frame.get_width(), 0, 0, frame.get_height(), 0.5, 0.5) * float3(frag_in.texcoord, 1.0));

    auto s3s0 = float4x2(0, 1, 0, 0, 0, 0, 1, 0) * samples;
    auto s2s1 = float4x2(0, 0, 0, 1, 1, 0, 0, 0) * samples;
    
    auto across = interp(s3s0, s2s1, offset.x);

    auto s3s2i = float2(1, 0) * across;
    auto s0s1i = float2(0, 1) * across;
    
    return float4(log(interp(s3s2i, s0s1i, offset.y)), 1.0);
}
