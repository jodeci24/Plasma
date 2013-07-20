#version 150
in vec2 aVertexPosition;
in vec2 aTextureCoord;

out vec2 vTextureCoord;

void main() {
    gl_Position = vec4(aVertexPosition, 0.0, 1.0);
    vTextureCoord = aTextureCoord;
}
