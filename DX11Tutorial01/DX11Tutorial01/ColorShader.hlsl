cbuffer ModelBuffer : register(b0)
{
    float4x4 modelMatrix;
	float4x4 normalMatrix;
}

struct Light
{
	float4 pos;
	float4 color;
};

cbuffer SceneBuffer : register(b1)
{
    float4x4 VP;
	// int4 lightParams;
	// Light lights[4];
}

cbuffer ConstantBuffer : register(b2)
{
	float4 scale; // x - scale factor // y- interpolate type (int)
}

Texture2D ColorTexture : register(t0);
Texture2D LayerTexture : register(t1);
// Texture2D NormalTexture : register(t1);

Texture2D VectorField : register(t2);

SamplerState Sampler : register(s0);

struct VSInput
{
	float4 pos : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct VSOutput
{
	float4 pos : SV_Position;
	float4 worldPos : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

VSOutput VS(in VSInput vertex)
{
	VSOutput output;
	float4 worldPos = mul(vertex.pos, modelMatrix);
	output.pos = mul(worldPos, VP);
	output.worldPos = worldPos;
	output.uv = vertex.uv;
	output.normal = mul(vertex.normal, normalMatrix);
	output.tangent = mul(vertex.tangent, normalMatrix);
	
	return output;
}

float2 Line(float t, float2 x1, float2 x2)
{
	return t * x1 + (1 - t) * x2;
}

float2 HalfCircle(float t, float2 x1, float2 x2, float2 vec)
{
	float2 newCoord1 = float2(0, 0);
	
	float pi = 3.14159265359;
	t = pi * t; // или pi * t чтобы в другую сторону
	float delta_x = (x1.r - x2.r);
	float delta_y = (x1.g - x2.g);

	float r = sqrt(delta_x * delta_x + delta_y * delta_y) / 2;
	float2 center = (x1 + x2) / 2;

	float2 bas = float2(1, 0);

	float angle = 0;
	angle = (bas.r * (vec.r) + bas.g * (vec.g)) / (1 * sqrt(vec.r * vec.r + vec.g * vec.g));
	float start = acos(angle);

	newCoord1.r = 5 * r * cos(t + start) + center.r;
	newCoord1.g = r * sin(t + start) + center.g;

	return newCoord1;
}

float4 PS(in VSOutput input) : SV_Target0
{
	float4 color = float4(0, 0, 0, 0);

	float2 coord = input.uv;
	
	float ddx = VectorField.Sample(Sampler, coord).r;
	float ddy = VectorField.Sample(Sampler, coord).g;

	float2 x2 = input.uv;
	float2 x1 = x2 + float2(ddx, ddy);

	float2 xc = (x1 + x2) / 2;

	float t = scale.x;

	float2 newCoord = coord + scale.x * float2(ddx, ddy);

	float2 newCoord1 = t * x1 + (1 - t) * x2;

	float pi = 3.14159265359;

	int sw = (int)scale.y;
	if (sw == 0)
	{
		newCoord1 = Line(t, x1, x2);
	}
	else // полукруг
	{
		/*t = -pi * t; // или pi * t чтобы в другую сторону
		float delta_x = (x1.r - x2.r);
		float delta_y = (x1.g - x2.g);

		float r = sqrt(delta_x * delta_x + delta_y * delta_y) / 2;
		float2 center = (x1 + x2) / 2;

		float2 bas = float2(1, 0);
		float2 vec = float2(-ddx, -ddy);

		float angle = (bas.r * vec.r + bas.g * vec.g) / (len(bas) * len(vec));
		angle = (bas.r * (-ddx) + bas.g * (-ddy)) / (1 * sqrt(ddx * ddx + ddy * ddy));
		float start = acos(angle);

		newCoord1.r = r * cos(t + start) + center.r;
		newCoord1.g = r * sin(t + start) + center.g;*/

		newCoord1 = HalfCircle(t, x1, x2, float2(-ddx, -ddy));
	}

	float3 matColor = ColorTexture.Sample(Sampler, newCoord1).rgb;   // анимированный слой
	// float3 matColor = ColorTexture.Sample(Sampler, coord).rgb;
	float3 layerColor = LayerTexture.Sample(Sampler, coord).rgb;   // фон

	if (matColor.r == 0 && matColor.g == 0 && matColor.b == 0)
	{
		color.xyz = layerColor;
	}
	else
	{
		color.xyz = matColor;
	}

	return color;

	/*float3 nm = (NormalTexture.Sample(Sampler, input.uv) - float3(0.5, 0.5, 0.5)) * 2.0;
	
	float3 binormal = cross(input.normal, input.tangent);
	
	float3 normal = float3(0, 0, 0);
	if (mode == 0)
	{
		normal = nm.x * input.tangent + nm.y * binormal + input.normal;
	}
	else
	{
		normal = input.normal;
	}
	
	for (int i = 0; i < lightCount; i++)
	{
		float3 l = lights[i].pos.xyz - input.worldPos.xyz;
		float dist = length(l);		
		l = l / dist;
		
		float ndotl = max(dot(l, normal),0);
		float atten = 1.0 / (0.2 + dist * dist);
	
		color.xyz += matColor * lights[i].color.xyz * ndotl * atten;
	}
	
	//color.xyz = 0.5 * (normal + float3(1,1,1));

	return color;*/
}
