cbuffer ConstantBuffer : register(b0)
{
	int4 secs;
}

texture2D Texture : register(t0);

texture2D VectorField : register(t1);

SamplerState Sampler : register(s0);

struct VSInput
{
	float4 pos : POSITION;
	float2 uv : TEXCOORD;
};

struct VSOutput
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD;
};

VSOutput VS(in VSInput vertex)
{
	VSOutput output;

	output.pos = vertex.pos;
	output.uv = vertex.uv;

	return output;
}

float4 PS(in VSOutput input) : SV_Target0
{
	float4 color = float4(0, 0, 0, 1);
	
	float ddx = VectorField.Sample(Sampler, input.uv).r;
	float ddy = VectorField.Sample(Sampler, input.uv).g;

	// float d = 1 / 2048.0;
	int scale = secs.x;

	color.rgb = Texture.Sample(Sampler, input.uv + scale * float2(ddy, ddx)).rgb;

	/*color.rgb = (Texture.Sample(Sampler, input.uv) +
		( ddx * 2048.0 * (Texture.Sample(Sampler, input.uv + float2(0, d)) - Texture.Sample(Sampler, input.uv + float2(0, -d))) ) +
		( ddy * 2048.0 * (Texture.Sample(Sampler, input.uv + float2(d, 0)) - Texture.Sample(Sampler, input.uv + float2(-d, 0))) )).rgb;*/



	/*color.rgb = Texture.Sample(Sampler, input.uv) -
		(ddy / 4.0 * (-Texture.Sample(Sampler, input.uv + float2(2 * d, 0)) + 4 * Texture.Sample(Sampler, input.uv + float2(d, 0)) - 4 * Texture.Sample(Sampler, input.uv + float2(-d, 0)) + Texture.Sample(Sampler, input.uv + float2(-2 * d, 0)))) +
		(ddy / 12.0 * (Texture.Sample(Sampler, input.uv + float2(2 * d, 0)) - 4 * Texture.Sample(Sampler, input.uv + float2(d, 0)) + 6 * Texture.Sample(Sampler, input.uv) - 4 * Texture.Sample(Sampler, input.uv + float2(-d, 0)) + Texture.Sample(Sampler, +float2(-2 * d, 0)))) +
		(ddx / 4.0 * (-Texture.Sample(Sampler, input.uv + float2(0, 2 * d)) + 4 * Texture.Sample(Sampler, input.uv + float2(0, d)) - 4 * Texture.Sample(Sampler, input.uv + float2(0, -d)) + Texture.Sample(Sampler, input.uv + float2(0, -2 * d)))) +
		(ddx / 12.0 * (Texture.Sample(Sampler, input.uv + float2(0, 2 * d)) - 4 * Texture.Sample(Sampler, input.uv + float2(0, d)) + 6 * Texture.Sample(Sampler, input.uv) - 4 * Texture.Sample(Sampler, input.uv + float2(0, -d)) + Texture.Sample(Sampler, +float2(0, -2 * d))));*/

	return color;
}
