#version 430 core

layout (points) in;
layout (points, max_vertices = 100) out;

in VS_OUT{
	int num;
	mat4 MVP;
} gs_in[];

void main()
{


	vec3 p0 = vec3(-2, -2, 0);
	vec3 p1 = vec3(-1, 2, 0);
	vec3 p2 = gl_in[0].gl_Position.xyz;
	vec3 p3 = vec3(1, 2, 0);
	vec3 p4 = vec3(2, -2, 0);

	float delta = 1 / float(gs_in[0].num);

	for(int i = 0; i<= gs_in[0].num; i++){
		float u = i * delta;
		float u1 = (1.0 - u);
		float u2 = u * u;
		//bernstein polynomials evaluated at u
		//or called the BEZ
		float b4 = u2 * u2;
		float b3 = 4.0 * u2 * u * u1;
		float b2 = 6.0 * u2 * u1 * u1;
		float b1 = 4.0 * u * u1 * u1 * u1;
		float b0 = u1 * u1 * u1 * u1;

		//Cubic Bezier interpolation
		vec3 p = p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3 + p4 * b4;
		gl_Position = gs_in[0].MVP * vec4(p, 1.0);
		EmitVertex();
	}
	EndPrimitive();
}