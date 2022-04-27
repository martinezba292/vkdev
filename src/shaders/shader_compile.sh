#!/bin/bash

GLSLANG="./../../deps/glslang/build/install/bin/glslangValidator -V"

vertex_shaders=("test")
i=0
for vertex in ./glsl/*.vert; do
$GLSLANG $vertex -o ./spirv/${vertex_shaders[$i]}_vert.spv
let "i++"
done

fragment_shaders=(
                  "test"
                  "stexture"
                 )
i=0
for fragment in ./glsl/*.frag; do
$GLSLANG $fragment -o ./spirv/${fragment_shaders[$i]}_frag.spv
let "i++"
done

