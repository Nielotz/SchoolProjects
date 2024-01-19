export default `#version 300 es
    precision mediump float;

    uniform vec4 u_color;
    uniform sampler2D u_texture;

    in vec2 v_textureCoordinate;

    out vec4 fragColor;

    void main() {
        fragColor = texture(u_texture, v_textureCoordinate) + vec4(v_textureCoordinate.xy, v_textureCoordinate.x * v_textureCoordinate.y, 0);
  }`;
