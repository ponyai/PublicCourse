// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <cstdint>

#include <GL/gl.h>

#include "common/utils/gl/context.h"

namespace gl {

class Context::Functions {
 public:
  explicit Functions(Context* context);
  virtual ~Functions() = default;

  // Basics.
  void (*GetIntegerv)(GLenum target, GLuint* ids) = nullptr;
  GLenum (*GetError)() = nullptr;

  // Alpha test
  void (*AlphaFunc)(GLenum func, GLclampf ref) = nullptr;

  // Blend state
  void (*BlendFunc)(GLenum sfactor, GLenum dfactor) = nullptr;

  // Framebuffer.
  void (*GenFramebuffers)(GLsizei n, GLuint* ids) = nullptr;
  void (*BindFramebuffer)(GLenum target, GLuint framebuffer) = nullptr;
  void (*FramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture,
                               GLint level) = nullptr;
  GLenum (*CheckFramebufferStatus)(GLenum target) = nullptr;
  void (*ReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type,
                     GLvoid* pixels) = nullptr;
  void (*DeleteFramebuffers)(GLsizei n, const GLuint* framebuffers) = nullptr;
  void (*GetFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname,
                                              GLint* params);

  // VertexBuffer
  void (*GenBuffers)(GLsizei n, GLuint* buffers) = nullptr;
  void (*BindBuffer)(GLenum target, GLuint buffer) = nullptr;
  void (*DeleteBuffers)(GLsizei n, const GLuint* buffers) = nullptr;
  void (*BufferData)(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage) = nullptr;

  // Texture.
  void (*GenTextures)(GLsizei n, GLuint* textures) = nullptr;
  void (*ActiveTexture)(GLenum texture) = nullptr;
  void (*BindTexture)(GLenum target, GLuint texture) = nullptr;
  void (*TexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width,
                     GLsizei height, GLint border, GLenum format, GLenum type,
                     const GLvoid* pixels) = nullptr;
  void (*TexEnvf)(GLenum target, GLenum pname, GLfloat param) = nullptr;
  void (*TexEnvi)(GLenum target, GLenum pname, GLint param) = nullptr;
  void (*TexParameteri)(GLenum target, GLenum pname, GLint param) = nullptr;
  void (*DeleteTextures)(GLsizei n, const GLuint* textures) = nullptr;
  void (*GenerateMipmap)(GLenum target) = nullptr;
  void (*GetTexImage)(GLenum target, GLint level, GLenum format, GLenum type,
                      GLvoid* pixels) = nullptr;

  // Rendering.
  void (*Clear)(GLbitfield mask) = nullptr;
  void (*ClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) = nullptr;
  void (*DrawArrays)(GLenum mode, GLint first, GLsizei count) = nullptr;
  void (*Finish)() = nullptr;
  void (*Viewport)(GLint x, GLint y, GLsizei width, GLsizei height) = nullptr;

  void (*ColorPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) = nullptr;
  void (*TexCoordPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) = nullptr;
  void (*VertexPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) = nullptr;

  // Client state.
  void (*EnableClientState)(GLenum cap) = nullptr;
  void (*DisableClientState)(GLenum cap) = nullptr;

  // fixed pipeline
  void (*ShadeModel)(GLenum mode) = nullptr;
  void (*Lightfv)(GLenum light, GLenum pname, const GLfloat* params) = nullptr;
  void (*PushMatrix)(void) = nullptr;
  void (*PopMatrix)(void) = nullptr;
  void (*MatrixMode)(GLenum mode) = nullptr;
  void (*LoadIdentity)(void) = nullptr;
  void (*Translatef)(GLfloat x, GLfloat y, GLfloat z) = nullptr;
  void (*Rotatef)(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) = nullptr;

  // Pipelines.
  GLuint (*CreateShader)(GLenum shaderType) = nullptr;
  void (*DeleteShader)(GLuint shader) = nullptr;
  void (*CompileShader)(GLuint shader) = nullptr;
  void (*ShaderSource)(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
  void (*GetShaderiv)(GLuint shader, GLenum pname, GLint* params);
  void (*GetShaderInfoLog)(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog);
  GLuint (*CreateProgram)(void) = nullptr;
  void (*DeleteProgram)(GLuint program) = nullptr;
  void (*AttachShader)(GLuint program, GLuint shader) = nullptr;
  void (*DetachShader)(GLuint program, GLuint shader) = nullptr;
  void (*LinkProgram)(GLuint program) = nullptr;
  void (*UseProgram)(GLuint program) = nullptr;
  void (*EnableVertexAttribArray)(GLuint index) = nullptr;
  void (*DisableVertexAttribArray)(GLuint index) = nullptr;
  GLint (*GetAttribLocation)(GLuint program, const GLchar* name) = nullptr;
  GLint (*GetUniformLocation)(GLuint program, const GLchar* name) = nullptr;
  void (*BindAttribLocation)(GLuint program, GLuint index, const GLchar* name) = nullptr;
  void (*VertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized,
                              GLsizei stride, const GLvoid* pointer) = nullptr;
  void (*Uniform1i)(GLint location, GLint v0) = nullptr;
  void (*UniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose,
                           const GLfloat* value) = nullptr;

  // Deprecated.
  void (*PushAttrib)(GLbitfield mask) = nullptr;
  void (*PopAttrib)() = nullptr;
  void (*Enable)(GLenum cap) = nullptr;
  void (*Disable)(GLenum cap) = nullptr;
  void (*Color3ub)(GLubyte red, GLubyte green, GLubyte blue) = nullptr;
  void (*Color3f)(GLfloat r, GLfloat g, GLfloat b) = nullptr;
  void (*Color4f)(GLfloat r, GLfloat g, GLfloat b, GLfloat a) = nullptr;
  void (*Color3d)(GLdouble r, GLdouble g, GLdouble b) = nullptr;
  void (*Color4d)(GLdouble r, GLdouble g, GLdouble b, GLdouble a) = nullptr;
  void (*TexCoord2f)(GLfloat x, GLfloat y) = nullptr;
  void (*TexCoord2d)(GLdouble x, GLdouble y) = nullptr;
  void (*Vertex2f)(GLfloat x, GLfloat y) = nullptr;
  void (*Vertex3f)(GLfloat x, GLfloat y, GLfloat z) = nullptr;
  void (*Vertex2d)(GLdouble x, GLdouble y) = nullptr;
  void (*Vertex3d)(GLdouble x, GLdouble y, GLdouble z) = nullptr;
  void (*Begin)(GLenum mode) = nullptr;
  void (*End)() = nullptr;
  void (*PointSize)(GLfloat pt_size) = nullptr;
  void (*LineWidth)(GLfloat line_width) = nullptr;
  void (*LineStipple)(GLint factor, GLushort pattern) = nullptr;
};

// ======================================== Implementation ========================================

inline Context::Functions::Functions(Context* context) {
#define INIT_FUNC(name)                                                         \
  name = reinterpret_cast<decltype(name)>(context->GetProcAddress("gl" #name)); \
  DCHECK(name != nullptr) << "cannot find OpenGL Func: " #name;

  INIT_FUNC(GetIntegerv);
  INIT_FUNC(GetError);

  INIT_FUNC(AlphaFunc);
  INIT_FUNC(BlendFunc);

  INIT_FUNC(GenFramebuffers);
  INIT_FUNC(BindFramebuffer);
  INIT_FUNC(FramebufferTexture2D);
  INIT_FUNC(CheckFramebufferStatus);
  INIT_FUNC(ReadPixels);
  INIT_FUNC(DeleteFramebuffers);
  INIT_FUNC(GetFramebufferAttachmentParameteriv);

  INIT_FUNC(GenBuffers);
  INIT_FUNC(BindBuffer);
  INIT_FUNC(DeleteBuffers);
  INIT_FUNC(BufferData);

  INIT_FUNC(GenTextures);
  INIT_FUNC(ActiveTexture);
  INIT_FUNC(BindTexture);
  INIT_FUNC(TexImage2D);
  INIT_FUNC(TexEnvf);
  INIT_FUNC(TexEnvi);
  INIT_FUNC(TexParameteri);
  INIT_FUNC(DeleteTextures);
  INIT_FUNC(GenerateMipmap);
  INIT_FUNC(GetTexImage);

  INIT_FUNC(Clear);
  INIT_FUNC(ClearColor);
  INIT_FUNC(DrawArrays);
  INIT_FUNC(Finish);
  INIT_FUNC(Viewport);

  INIT_FUNC(ColorPointer);
  INIT_FUNC(TexCoordPointer);
  INIT_FUNC(VertexPointer);

  INIT_FUNC(CreateShader);
  INIT_FUNC(DeleteShader);
  INIT_FUNC(CompileShader);
  INIT_FUNC(ShaderSource);
  INIT_FUNC(GetShaderiv);
  INIT_FUNC(GetShaderInfoLog);
  INIT_FUNC(CreateProgram);
  INIT_FUNC(DeleteProgram);
  INIT_FUNC(AttachShader);
  INIT_FUNC(DetachShader);
  INIT_FUNC(LinkProgram);
  INIT_FUNC(UseProgram);
  INIT_FUNC(EnableVertexAttribArray);
  INIT_FUNC(DisableVertexAttribArray);
  INIT_FUNC(GetAttribLocation);
  INIT_FUNC(GetUniformLocation);
  INIT_FUNC(BindAttribLocation);
  INIT_FUNC(VertexAttribPointer);
  INIT_FUNC(Uniform1i);
  INIT_FUNC(UniformMatrix4fv);

  INIT_FUNC(EnableClientState);
  INIT_FUNC(DisableClientState);

  INIT_FUNC(ShadeModel);
  INIT_FUNC(Lightfv);
  INIT_FUNC(Translatef);
  INIT_FUNC(Rotatef);
  INIT_FUNC(PushMatrix);
  INIT_FUNC(PopMatrix);
  INIT_FUNC(MatrixMode);
  INIT_FUNC(LoadIdentity);

  INIT_FUNC(PushAttrib);
  INIT_FUNC(PopAttrib);
  INIT_FUNC(Enable);
  INIT_FUNC(Disable);
  INIT_FUNC(Color3d);
  INIT_FUNC(Color3ub);
  INIT_FUNC(Color3f);
  INIT_FUNC(Color4d);
  INIT_FUNC(Color4f);
  INIT_FUNC(TexCoord2f);
  INIT_FUNC(TexCoord2d);
  INIT_FUNC(Vertex2d);
  INIT_FUNC(Vertex2f);
  INIT_FUNC(Vertex3d);
  INIT_FUNC(Vertex3f);
  INIT_FUNC(Begin);
  INIT_FUNC(End);
  INIT_FUNC(PointSize);
  INIT_FUNC(LineWidth);
  INIT_FUNC(LineStipple);
#undef INIT_FUNC
}

}  // namespace gl
