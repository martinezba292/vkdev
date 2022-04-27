#version 450

layout(location=0) in vec3 f_Color;

layout(location=0) out vec4 o_Color;

void main(){
  //o_Color = vec4( 0.0, 0.4, 1.0, 1.0 );
  o_Color = vec4(f_Color, 0.0);
}
