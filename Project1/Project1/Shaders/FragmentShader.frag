#version 330 core //Verion
uniform sampler2D tex0;

in vec2 texCoord;

out vec4 FragColor; //Returns a Color
// simple shader
void main()
{
//						R	G	B	A      0 -> 1
	//FragColor = vec4(0.7f, 0f, 0f, 1f); // sets the color

	FragColor = texture(tex0, texCoord);
}