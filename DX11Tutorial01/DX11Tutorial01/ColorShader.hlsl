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

Texture2D LayerTexture : register(t0); // фон

Texture2D ColorTexture : register(t1); // слой 1
Texture2D VectorField : register(t2);

Texture2D ColorTexture2 : register(t3); // слой 2
Texture2D VectorField2 : register(t4);

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

float2 MatrixCutSwordInterpolation(float t, float2 x1, float2 x2, float shift)
{
	return (1 - t) * x1 + t * (x1 - float2(shift, 0));
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
	/*{
		float d = 1 / 2048.0;
		float t = scale.x;
		float pi = 3.14159265359;

		float T = 2 * pi * t / 1;

		float a11 = cos(T);
		float a12 = sin(T);
		float a21 = -sin(T);
		float a22 = cos(T);

		float2 coord = input.uv;
		float2 center = float2(1024 * d, 1024 * d);  // 778 647

		float2 newCoord;
		newCoord.x = a11 * (coord.x - center.x) + a12 * (coord.y - center.y) + center.x;
		newCoord.y = a21 * (coord.x - center.x) + a22 * (coord.y - center.y) + center.y;

		float4 color = float4(0, 0, 0, 0);
		Texture2D textures[] = { ColorTexture, ColorTexture2 };
		int texturesNum = 2;

		float3 layerColor = LayerTexture.Sample(Sampler, coord).rgb;   // фон

		for (int i = texturesNum - 1; i >= 0; --i)
		{
			float3 matColor = textures[i].Sample(Sampler, newCoord).rgb;

			if (matColor.r != 0 || matColor.g != 0 || matColor.b != 0)  // :-(
			{
				color.xyz = matColor;
				break;
			}

			color.xyz = layerColor;
		}

		return color;
	}*/

	float4 color = float4(0, 0, 0, 0);

	float2 coord = input.uv;
	
	float ddx = VectorField.Sample(Sampler, coord).r;
	float ddy = VectorField.Sample(Sampler, coord).g;
	float2 vec1 = float2(ddx, ddy);

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
		newCoord1 = HalfCircle(t, x1, x2, float2(-ddx, -ddy));
	}

	// Поле для второй текстуры
	ddx = VectorField2.Sample(Sampler, coord).r;
	ddy = VectorField2.Sample(Sampler, coord).g;
	float2 vec2 = float2(ddx, ddy);

	float2 newCoord2 = coord + scale.x * float2(ddx, ddy);

	Texture2D textures[] = { ColorTexture, ColorTexture2 };
	float2 newCoords[] = { newCoord1, newCoord2 };
	Texture2D fields[] = { VectorField, VectorField2 };
	float2 vecs[] = { VectorField.Sample(Sampler, coord), VectorField2.Sample(Sampler, coord) };
	// float2 newCoords[] = { coord, coord };
	int texturesNum = 2;

	float3 layerColor = LayerTexture.Sample(Sampler, coord).rgb;   // фон

	for (int i = texturesNum - 1; i >= 0; --i)
	{
		// float3 srcColor = textures[i].Sample(Sampler, newCoords[i]).rgb;
		// float3 dstColor = textures[i].Sample(Sampler, newCoords[i] - float2(ddx, ddy)).rgb;

		// Пока так определяю тип интерполяции (не работает)
		float2 c = newCoords[i];

		float2 xx = input.uv;
		float d = 1 / 2048.0;

		ddx = fields[i].Sample(Sampler, coord).r;
		ddy = fields[i].Sample(Sampler, coord).g;

		float transformddx = fields[i].Sample(Sampler, coord).b;
		float transformddy = fields[i].Sample(Sampler, coord).a;

		float interpolateType = fields[i].Sample(Sampler, coord).b;

		// if (abs(transformddx) > 0.0)
		{
			float2 xxx = input.uv;

			if (xxx.y > 320 * d && xxx.y <= 530 * d)
			{
				c = (1 - t) * input.uv + t * (input.uv + float2(transformddx, transformddy));

				if (xxx.x > 625 * d && xxx.x <= 655 * d)
				{
					float2 xx2 = input.uv + float2(ddx, ddy) + float2(-30 * d, 0);   // src
					float2 xx1 = xx2 - float2(-30 * d, 0);   // dst
					c = t * xx1 + (1 - t) * xx2;
					c = input.uv + float2(transformddx, transformddy) + (1 - t) * float2(
						-fields[i].Sample(Sampler, input.uv + float2(transformddx, transformddy)).r,
						-fields[i].Sample(Sampler, input.uv + float2(transformddx, transformddy)).g
						);

					// c = input.uv + float2(transformddx, transformddy);

					c = input.uv + float2(ddx, ddy) + (1 - t) * float2(transformddx, transformddy);

					// return float4(1, 0, 0, 1);
				}
			}

			// return float4(0, 1, 0, 1);
		}

		float3 matColor = textures[i].Sample(Sampler, c).rgb;
		// float3 matColor = srcColor * t + dstColor * (1 - t);
		
		if (matColor.r != 0 || matColor.g != 0 || matColor.b != 0)  // :-(
		{
			color.xyz = matColor;
			break;
		}


		color.xyz = layerColor;
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
