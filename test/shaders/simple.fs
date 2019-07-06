
#version 330 core

// Input data
//in vec4 norm;
in vec2 uv;

// Output data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D TEXTURE_SAMPLER;

void main()
{
    // Output color = color of the texture at the specified UV
    color = texture(TEXTURE_SAMPLER, uv);
	//color = (gl_FragCoord.xyz + vec3(1,1,1)) * .5f;

	//float red = abs(norm.z);
	//float blue = 1 - abs(norm.z);

	//color = texture(TEXTURE_SAMPLER, UV).rgb * red;

	if (color.a < .25f)
		discard;
}