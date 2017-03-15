#version 330 core
//set floats to high precision
precision highp float; 

in  vec3 ex_Color;
out vec4 gl_FragColor;

void main(void) {
    gl_FragColor = vec4(ex_Color,1.0);
}