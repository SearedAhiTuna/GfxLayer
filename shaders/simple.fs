
#version 330 core

// Input data
in vec2 uv;
in vec3 norm;

// Output data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D TEXTURE_SAMPLER;

void main()
{
    // Output color = color of the texture at the specified UV
    
	//color = texture(TEXTURE_SAMPLER, uv);
	color = vec4(1, 1, 1, 1);

	//if (texture(TEXTURE_SAMPLER, uv).a < .25f)
		//discard;

	float light = abs(norm.z);
	color *= vec4(vec3(1,1,1) * light, 1);
}