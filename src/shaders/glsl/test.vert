#version 450

layout(location=0) in vec3 i_Position;
layout(location=1) in vec3 i_Color;

layout(location=0) out vec3 f_Color;

void main() {
    //vec2 pos[3] = vec2[3]( vec2(-0.7, 0.7), vec2(0.7, 0.7), vec2(0.0, -0.7) );
    //gl_Position = vec4( pos[gl_VertexIndex], 0.0, 1.0 );
    gl_Position = vec4(i_Position, 1.0);
    f_Color = i_Color;
}
