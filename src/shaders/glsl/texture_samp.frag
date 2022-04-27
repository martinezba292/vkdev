#version 450

layout(set=0, binding=0) uniform sampler2D u_Texture;

layout(location=0) in vec3 f_Color;
layout(location=1) in vec2 tex_Coords;

layout(location=0) out vec4 o_Color;

void main(){
  o_Color = vec4(f_Color, 0.0) * texture(u_Texture, tex_Coords);
}