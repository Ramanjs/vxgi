#include "shader.h"

#include <iostream>

void Shader::printLog(GLuint object) {
  GLint log_length = 0;
  if (glIsShader(object))
    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
  else if (glIsProgram(object))
    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
  else {
    fprintf(stderr, "printlog: Not a shader or a program\n");
    return;
  }

  char *log = (char *)malloc(log_length);

  if (glIsShader(object))
    glGetShaderInfoLog(object, log_length, NULL, log);
  else if (glIsProgram(object))
    glGetProgramInfoLog(object, log_length, NULL, log);

  fprintf(stderr, "%s", log);
  free(log);
}

char *Shader::loadFile(const char *filename) {

  FILE *input = fopen(filename, "rb");
  if (input == NULL)
    return NULL;

  if (fseek(input, 0, SEEK_END) == -1)
    return NULL;
  long size = ftell(input);
  if (size == -1)
    return NULL;
  if (fseek(input, 0, SEEK_SET) == -1)
    return NULL;

  /*if using c-compiler: dont cast malloc's return value*/
  char *content = (char *)malloc((size_t)size + 1);
  if (content == NULL)
    return NULL;

  fread(content, 1, (size_t)size, input);
  if (ferror(input)) {
    free(content);
    return NULL;
  }

  fclose(input);
  content[size] = '\0';
  return content;
}

GLuint Shader::initShader(const char *filename, GLenum type) {
  const GLchar *source = loadFile(filename);
  if (source == NULL) {
    fprintf(stderr, "Error opening %s: ", filename);
    perror("");
    exit(1);
  }
  GLuint res = glCreateShader(type);
  glShaderSource(res, 1, &source, NULL);
  free((void *)source);

  glCompileShader(res);
  GLint compile_ok = GL_FALSE;
  char infoLog[512];
  glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
  if (compile_ok == GL_FALSE) {
    fprintf(stderr, "%s:", filename);
    printLog(res);
    exit(1);
  }

  return res;
}

void Shader::attachAndLinkProgram() {
  if (vs)
    glAttachShader(program, vs);
  if (fs)
    glAttachShader(program, fs);
  if (gs)
    glAttachShader(program, gs);

  glLinkProgram(program);
  GLint linked;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  if (!linked) {
    std::cout << "Failed to link: " << std::endl;
    printLog(program);
  }
}

Shader::Shader(const char *vsPath, const char *fsPath, const char *gsPath) {
  if (vsPath)
    vs = initShader(vsPath, GL_VERTEX_SHADER);
  if (fsPath)
    fs = initShader(fsPath, GL_FRAGMENT_SHADER);
  if (gsPath)
    gs = initShader(fsPath, GL_GEOMETRY_SHADER);

  program = glCreateProgram();

  attachAndLinkProgram();
}

void Shader::use() { glUseProgram(program); }

void Shader::setUniform(uniformType type, void *param, char *name) {
  GLint loc = glGetUniformLocation(program, name);

  if (type == uniformType::i1) {
    glUniform1i(glGetUniformLocation(program, name), *((int *)param));
  } else if (type == uniformType::f1) {
    glUniform1f(glGetUniformLocation(program, name), *((float *)param));
  } else if (type == uniformType::fv3) {
    glUniform3fv(glGetUniformLocation(program, name), 1, (float *)param);
  } else if (type == uniformType::fv4) {
    glUniform4fv(glGetUniformLocation(program, name), 1, (float *)param);
  } else if (type == uniformType::mat4x4) {
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE,
                       (float *)param);
  } else if (type == uniformType::mat3x3) {
    glUniformMatrix3fv(glGetUniformLocation(program, name), 1, GL_FALSE,
                       (float *)param);
  }
}
