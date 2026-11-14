attribute vec3 position;
attribute vec2 texCoord;
varying vec2 vTexCoord;
uniform mat4 matrix;

void main()
{
    vTexCoord = texCoord;
    gl_Position = matrix * vec4(position, 1.0);
}
