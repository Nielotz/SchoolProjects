export default `#version 300 es
    precision mediump float;

    in vec3 vertexPosition;
    in vec2 textureCoordinate;

    uniform mat4 u_MVP;
    uniform mat4 u_transformation;

    out vec2 v_textureCoordinate;


    void main() {
        gl_Position = u_MVP * u_transformation * vec4(vertexPosition, 1.0);
        v_textureCoordinate = textureCoordinate;
    }`;
