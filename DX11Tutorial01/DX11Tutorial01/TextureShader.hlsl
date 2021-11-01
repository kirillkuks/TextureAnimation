texture2D Texture : register(t0);

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
	float dy = -1 / 1920.0;

	color.rgb = Texture.Sample(Sampler, input.uv + float2(0, dy)).rgb;
	return color;
}
