
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float x_mod = 0;
float y_mod = 0;
float x_rot = 0;
float y_rot = 0;
float scale_mod = 0;

void Key_Callback(GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods)
{
    // transforming
    if (key == GLFW_KEY_D &&
        (action == GLFW_PRESS || GLFW_REPEAT)) {
        x_mod += 0.02f;
    }
    if (key == GLFW_KEY_A &&
        (action == GLFW_PRESS || GLFW_REPEAT)) {
        x_mod -= 0.02f;
    }
    if (key == GLFW_KEY_W &&
        (action == GLFW_PRESS || GLFW_REPEAT)) {
        y_mod += 0.02f;
    }
    if (key == GLFW_KEY_S &&
        (action == GLFW_PRESS || GLFW_REPEAT)) {
        y_mod -= 0.02f;
    }

    //rotating
    if (key == GLFW_KEY_RIGHT &&
        (action == GLFW_PRESS || GLFW_REPEAT)) {
        x_rot += 2.f;
    }
    if (key == GLFW_KEY_LEFT &&
        (action == GLFW_PRESS || GLFW_REPEAT)) {
        x_rot -= 2.f;
    }
    if (key == GLFW_KEY_UP &&
        (action == GLFW_PRESS || GLFW_REPEAT)) {
        y_rot += 2.f;
    }
    if (key == GLFW_KEY_DOWN &&
        (action == GLFW_PRESS || GLFW_REPEAT)) {
        y_rot -= 2.f;
    }

    // scaling
    if (key == GLFW_KEY_E &&
        (action == GLFW_PRESS || GLFW_REPEAT)) {
        scale_mod += 0.02f;
    }
    if (key == GLFW_KEY_Q &&
        (action == GLFW_PRESS || GLFW_REPEAT)) {
        scale_mod -= 0.02f;
    }
}

#include <string>
#include <iostream>

std::string path = "3D/myCube.obj";
std::vector < tinyobj::shape_t> shapes;
std::vector < tinyobj::material_t> material;
std::string warning, error;
tinyobj::attrib_t attributes;


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    float height = 600.0f;
    float width = 600.0f;


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(600, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    int img_width, img_height, colorChannels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* tex_bytes =
        stbi_load("3D/ayaya.png",
            &img_width,
            &img_height,
            &colorChannels,
            0);


    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGBA,
        img_width,
        img_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        tex_bytes);

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);

    glEnable(GL_DEPTH_TEST);

    glViewport(0,
        0,
        width,
        height);

    glfwSetKeyCallback(window, Key_Callback);


    std::fstream vertSrc("Shaders/VertexShader.vert");
    std::stringstream vertBuff;

    vertBuff << vertSrc.rdbuf();

    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    std::fstream fragSrc("Shaders/FragmentShader.frag");
    std::stringstream fragBuff;

    fragBuff << fragSrc.rdbuf();

    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &v, NULL);

    glCompileShader(vertexShader);


    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragShader, 1, &f, NULL);

    glCompileShader(fragShader);

    GLuint shaderProg = glCreateProgram();

    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragShader);

    glLinkProgram(shaderProg);

    GLuint VAO, VBO, EBO, VBO_UV;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO_UV);
    glGenBuffers(1, &EBO);

    bool success = tinyobj::LoadObj(&attributes,
                                    &shapes,
                                    &material,
                                    &warning,
                                    &error,
                                    path.c_str());

    std::vector<GLuint> mesh_indices;

    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(shapes[0].mesh.indices[i].vertex_index);
    }


    GLfloat vertices[]{
        -0.5f, -0.5f, 0,

        0, 0.5f, 0,

        0.5, -0.5f, 0
    };

    //EBO infex array
    GLuint indices[] = {
        0,1,2 // traingle
    };

    GLfloat UV[]{
        0.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        0.f, 0.f
    };

    glm::mat3 identity_matrix3 = glm::mat3(1.0f);
    glm::mat4 identity_matrix4 = glm::mat4(1.0f);

    float x = 0, y = 0, z = 0;
    float scalex = 1, scaley = 1, scalez = 1;
    float theta = 0;
    float axisVectorx = 1, axisVectory = 1, axisVectorz = 0;

    //tex from fragment shader
    GLuint tex0Address = glGetUniformLocation(shaderProg, "tex0");
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(tex0Address, 0);


    //position of the camera in the world
    glm::vec3 cameraPos = glm::vec3(x_mod, 0, 10.f);

    //position matrix using the eye
    glm::mat4 cameraPositionMatrix =
        glm::translate(glm::mat4(1.0f),
            cameraPos * -1.0f);

    //worlds up direction
    glm::vec3 WorldUp = glm::vec3(0, 1.0f, 0);
    glm::vec3 Center = glm::vec3(0, 3.f, 0);

    //get the forwards
    glm::vec3 F = glm::vec3(Center - cameraPos);
    F = glm::normalize(F);

    //right
    glm::vec3 R = glm::normalize(
        glm::cross(F, WorldUp)
    );

    // up
    glm::vec3 U = glm::normalize(
        glm::cross(R, F)
    );

    //construct the camera orientation matrix
    glm::mat4 cameraOrientation = glm::mat4(1.f);
    //Matrix[Column][Row]
    cameraOrientation[0][0] = R.x;
    cameraOrientation[1][0] = R.y;
    cameraOrientation[2][0] = R.z;

    cameraOrientation[0][1] = U.x;
    cameraOrientation[1][1] = U.y;
    cameraOrientation[2][1] = U.z;

    cameraOrientation[0][2] = -F.x;
    cameraOrientation[1][2] = -F.y;
    cameraOrientation[2][2] = -F.z;

    //Camera View Matrix
    glm::mat4 viewMatrix = cameraOrientation * cameraPositionMatrix;



    glm::mat4 translation =
        glm::translate(identity_matrix4,
            glm::vec3(  x,
                        y,
                        z)
        );


    glm::mat4 scale =
        glm::scale(identity_matrix4,
            glm::vec3(  x,
                        y,
                        z)
        );


    glm::mat4 rotation =
        glm::rotate(identity_matrix4,
            glm::radians(theta),
            glm::vec3(x,
                y,
                z)
        );
    
    
    glm::mat4 projection = glm::perspective(
        glm::radians(60.0f), // FOV
        height / width, // as[ect ration
        0.1f, // near
        100.f // far
    );
    

    
    glUseProgram(shaderProg);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GL_FLOAT) * attributes.vertices.size(),
        &attributes.vertices[0],
        GL_STATIC_DRAW);

    glVertexAttribPointer(0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLuint) * mesh_indices.size(),
        mesh_indices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GLfloat)* (sizeof(UV) / sizeof(UV[0])),
        &UV[0],
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(float),
        (void*)0
    );



    glEnableVertexAttribArray(2);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    


    // Loop until the user closes the window 
    while (!glfwWindowShouldClose(window))
    {
        // Render here 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");
        
        // transform
        glm::mat4 transformation_matrix = glm::translate(
            identity_matrix4,
            glm::vec3(x + x_mod,y + y_mod,-5.0f)
            );


        // scale
        transformation_matrix = glm::scale(
            transformation_matrix,
            glm::vec3(scalex + scale_mod, scaley + scale_mod, scalez + scale_mod)
        );
        
        // rotate L/R
        transformation_matrix = glm::rotate(
            transformation_matrix,
            glm::radians(theta + x_rot),
            glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))
        );

        // rotate Up/Down
        transformation_matrix = glm::rotate(
            transformation_matrix,
            glm::radians(theta - y_rot),
            glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f))
        );

        glUniformMatrix4fv(transformLoc,
            1,
            GL_FALSE,
            glm::value_ptr(transformation_matrix));


        unsigned int projLoc = glGetUniformLocation(shaderProg, "projection");
        glUniformMatrix4fv(projLoc,
            1,
            GL_FALSE,
            glm::value_ptr(projection));

        unsigned int viewLoc = glGetUniformLocation(shaderProg, "view");
        glUniformMatrix4fv(viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(viewMatrix));


        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES,
            mesh_indices.size(),
            GL_UNSIGNED_INT,
            0);

        // Swap front and back buffers 
        glfwSwapBuffers(window);

        // Poll for and process events 
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    /* Module 0
    // Loop until the user closes the window 
    while (!glfwWindowShouldClose(window))
    {
        // Render here 
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();

        
        // Swap front and back buffers 
        glfwSwapBuffers(window);

        // Poll for and process events 
        glfwPollEvents();
    }
    */
    glfwTerminate();
    return 0;
}
