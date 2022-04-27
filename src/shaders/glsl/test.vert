#version 450

layout(location=0) in vec3 i_Position;
layout(location=1) in vec3 i_Color;
layout(location=2) in vec2 uv;

layout(set=0, binding=0) uniform UboView {
    mat4 view;
    mat4 projection;
}uboView;

layout(set=1, binding=0) uniform UboInstance {
    mat4 model;
    //mat4 padding[3];
}uboInstance;

layout(location=0) out vec3 f_Color;
//layout(location=1) out vec2 tex_Coords;

void main() {
    mat4 modelView = uboView.view * uboInstance.model;
    gl_Position = uboView.projection * modelView * vec4(i_Position, 1.0);
    f_Color = i_Color;
    //tex_Coords = uv;
}
