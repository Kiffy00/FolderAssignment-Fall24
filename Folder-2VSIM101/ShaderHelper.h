#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <vector>

enum shaderEnum {
    VertexSource,
    FragmentSource
};

class ShaderHelper
{
public:
    static GLuint compileShader(const GLchar* source, GLenum type) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);

        // Check for shader compilation errors
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    static GLuint createProgram(const GLchar* vertexSource, const GLchar* fragmentSource) {
        GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
        GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

        if (!vertexShader || !fragmentShader) {
            std::cerr << "ERROR::SHADER::PROGRAM::SHADER_CREATION_FAILED\n";
            return 0;  // Failed to compile
        }

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        // Check for linking errors
        GLint success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(program, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            glDeleteProgram(program);
            program = 0;  // Return 0 to indicate failure
        }

        // Clean up shaders after linking
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return program;
    }

    template <typename T, typename I>
    static void setupObjects(GLuint& VAO, GLuint& VBO, GLuint& EBO, const std::vector<T>& vertices, const std::vector<I>& indices) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        if (!indices.empty()) {
            glGenBuffers(1, &EBO);
        }

        glBindVertexArray(VAO);

        // Bind and set the vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), GL_STATIC_DRAW);

        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(T), (void*)0);

        // Color attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(T), (void*)(3 * sizeof(float)));

        // Normal attribute
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(T), (void*)(6 * sizeof(float)));

        // Bind and set the element buffer
        if (!indices.empty()) {
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        }

        // Unbind the VAO
        glBindVertexArray(0);

        // Unbind the VBO and EBO to avoid accidental modifications
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }


    // reads from current folder
    static std::string readShader(bool fragment) {
        std::ifstream shaderFile;
        if (fragment)
            shaderFile.open("Triangle.fs");
        else
            shaderFile.open("Triangle.vs");

        std::string str((std::istreambuf_iterator<char>(shaderFile)),
            std::istreambuf_iterator<char>());
        return str;
    }
};

