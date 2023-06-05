/***********************************************************************
 * @file	FrameBuffer.hpp
 * @author	jjyou
 * @date	2023-5-23
 * @brief	This file implements FrameBuffer class.
***********************************************************************/
#ifndef jjyou_gl_FrameBuffer_hpp
#define jjyou_gl_FrameBuffer_hpp

#include <glad/glad.h>
#include <memory>
#include <vector>

namespace jjyou {
	namespace gl {

		class FrameBuffer {
		public:
			using Ptr = std::shared_ptr<FrameBuffer>;
			enum class AttachType {
				NotAttached = 0,
				Texture2D = 1,
				RenderBuffer = 2
			};
			FrameBuffer(GLsizei width, GLsizei height) :
				_width(width), _height(height),
				depthAttachment(0), depthAttachFormat(0), depthAttachType(AttachType::NotAttached),
				stencilAttachment(0), stencilAttachFormat(0), stencilAttachType(AttachType::NotAttached)
			{
				glGenFramebuffers(1, &this->fbo);
				glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &this->maxAttachments);
				this->colorAttachments.resize(this->maxAttachments, 0);
				this->colorAttachFormat.resize(this->maxAttachments, 0);
				this->colorAttachType.resize(this->maxAttachments, AttachType::NotAttached);
			}
			void bind(void) {
				glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
			}
			bool resize(GLsizei width, GLsizei height) {
				if (width <= 0 || height <= 0)
					return false;
				if (this->_width == width && this->_height == height)
					return true;
				this->_width = width;
				this->_height = height;
				//color
				for (int index = 0; index < this->maxAttachments; index++)
					if (this->colorAttachType[index] == AttachType::Texture2D) {
						glBindTexture(GL_TEXTURE_2D, this->colorAttachments[index]);
						glTexStorage2D(GL_TEXTURE_2D, 1, this->colorAttachFormat[index], this->_width, this->_height);
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, this->colorAttachments[index], 0);
					}
					else if (this->colorAttachType[index] == AttachType::RenderBuffer) {
						glBindRenderbuffer(GL_RENDERBUFFER, this->colorAttachments[index]);
						glRenderbufferStorage(GL_RENDERBUFFER, this->colorAttachFormat[index], this->_width, this->_height);
						glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_RENDERBUFFER, this->colorAttachments[index]);
					}
				//depth
				if (this->depthAttachType == AttachType::Texture2D) {
					glBindTexture(GL_TEXTURE_2D, this->depthAttachment);
					glTexStorage2D(GL_TEXTURE_2D, 1, this->depthAttachFormat, this->_width, this->_height);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthAttachment, 0);
				}
				else if (this->depthAttachType == AttachType::RenderBuffer) {
					glBindRenderbuffer(GL_RENDERBUFFER, this->depthAttachment);
					glRenderbufferStorage(GL_RENDERBUFFER, this->depthAttachFormat, this->_width, this->_height);
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthAttachment);
				}
				//stencil
				if (this->stencilAttachType == AttachType::Texture2D) {
					glBindTexture(GL_TEXTURE_2D, this->stencilAttachment);
					glTexStorage2D(GL_TEXTURE_2D, 1, this->stencilAttachFormat, this->_width, this->_height);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, this->stencilAttachment, 0);
				}
				else if (this->stencilAttachType == AttachType::RenderBuffer) {
					glBindRenderbuffer(GL_RENDERBUFFER, this->stencilAttachment);
					glRenderbufferStorage(GL_RENDERBUFFER, this->stencilAttachFormat, this->_width, this->_height);
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->stencilAttachment);
				}
				return true;
			}
			size_t height(void) const {
				return this->_height;
			}
			size_t width(void) const {
				return this->_width;
			}
			bool clearColorAttachment(int index) {
				if (index < 0 || index >= this->maxAttachments)
					return false;
				this->bind();
				if (this->colorAttachType[index] == AttachType::Texture2D) {
					glBindTexture(GL_TEXTURE_2D, 0);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, 0, 0);
					glDeleteTextures(1, &this->colorAttachments[index]);
				}
				else if (this->colorAttachType[index] == AttachType::RenderBuffer) {
					glBindRenderbuffer(GL_RENDERBUFFER, 0);
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_RENDERBUFFER, 0);
					glDeleteRenderbuffers(1, &this->colorAttachments[index]);
				}
				else
					return false;
				this->colorAttachments[index] = 0;
				this->colorAttachFormat[index] = 0;
				this->colorAttachType[index] = AttachType::NotAttached;
				return true;
			}
			bool clearDepthAttachment(void) {
				this->bind();
				if (this->depthAttachType == AttachType::Texture2D) {
					glBindTexture(GL_TEXTURE_2D, 0);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
					glDeleteTextures(1, &this->depthAttachment);
				}
				else if (this->depthAttachType == AttachType::RenderBuffer) {
					glBindRenderbuffer(GL_RENDERBUFFER, 0);
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
					glDeleteRenderbuffers(1, &this->depthAttachment);
				}
				else
					return false;
				this->depthAttachment = 0;
				this->depthAttachFormat = 0;
				this->depthAttachType = AttachType::NotAttached;
				return true;
			}
			bool clearStencilAttachment(void) {
				this->bind();
				if (this->stencilAttachType == AttachType::Texture2D) {
					glBindTexture(GL_TEXTURE_2D, 0);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
					glDeleteTextures(1, &this->stencilAttachment);
				}
				else if (this->stencilAttachType == AttachType::RenderBuffer) {
					glBindRenderbuffer(GL_RENDERBUFFER, 0);
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
					glDeleteRenderbuffers(1, &this->stencilAttachment);
				}
				else
					return false;
				this->stencilAttachment = 0;
				this->stencilAttachFormat = 0;
				this->stencilAttachType = AttachType::NotAttached;
				return true;
			}
			bool setColorAttachment(int index, GLenum format, AttachType type) {
				if (index < 0 || index >= this->maxAttachments)
					return false;
				this->clearColorAttachment(index);
				if (type == AttachType::Texture2D) {
					glGenTextures(1, &this->colorAttachments[index]);
					glBindTexture(GL_TEXTURE_2D, this->colorAttachments[index]);
					glTexStorage2D(GL_TEXTURE_2D, 1, format, this->_width, this->_height);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, this->colorAttachments[index], 0);
				}
				else if (type == AttachType::RenderBuffer) {
					glGenRenderbuffers(1, &this->colorAttachments[index]);
					glBindRenderbuffer(GL_RENDERBUFFER, this->colorAttachments[index]);
					glRenderbufferStorage(GL_RENDERBUFFER, format, this->_width, this->_height);
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_RENDERBUFFER, this->colorAttachments[index]);
				}
				this->colorAttachFormat[index] = format;
				this->colorAttachType[index] = type;
				return true;
			}
			bool setDepthAttachment(GLenum format, AttachType type) {
				this->clearDepthAttachment();
				if (type == AttachType::Texture2D) {
					glGenTextures(1, &this->depthAttachment);
					glBindTexture(GL_TEXTURE_2D, this->depthAttachment);
					glTexStorage2D(GL_TEXTURE_2D, 1, format, this->_width, this->_height);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthAttachment, 0);
				}
				else if (type == AttachType::RenderBuffer) {
					glGenRenderbuffers(1, &this->depthAttachment);
					glBindRenderbuffer(GL_RENDERBUFFER, this->depthAttachment);
					glRenderbufferStorage(GL_RENDERBUFFER, format, this->_width, this->_height);
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthAttachment);
				}
				this->depthAttachFormat = format;
				this->depthAttachType = type;
				return true;
			}
			bool setStencilAttachment(GLenum format, AttachType type) {
				this->clearStencilAttachment();
				if (type == AttachType::Texture2D) {
					glGenTextures(1, &this->stencilAttachment);
					glBindTexture(GL_TEXTURE_2D, this->stencilAttachment);
					glTexStorage2D(GL_TEXTURE_2D, 1, format, this->_width, this->_height);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, this->stencilAttachment, 0);
				}
				else if (type == AttachType::RenderBuffer) {
					glGenRenderbuffers(1, &this->stencilAttachment);
					glBindRenderbuffer(GL_RENDERBUFFER, this->stencilAttachment);
					glRenderbufferStorage(GL_RENDERBUFFER, format, this->_width, this->_height);
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->stencilAttachment);
				}
				this->stencilAttachFormat = format;
				this->stencilAttachType = type;
				return true;
			}
			bool setDrawBuffers(std::initializer_list<GLenum> list) {
				std::vector<GLenum> indices;
				for (const auto& index : list) {
					if (index < 0 || index >= this->maxAttachments || this->colorAttachType[index] == AttachType::NotAttached)
						return false;
					indices.push_back(GL_COLOR_ATTACHMENT0 + index);
				}
				this->bind();
				glDrawBuffers(indices.size(), indices.data());
				return true;
			}
			template<class Iter> bool setDrawBuffers(const Iter begin, const Iter end) {
				std::vector<GLenum> indices;
				for (Iter iter = begin; iter != end; iter++) {
					if (*iter < 0 || *iter >= this->maxAttachments || this->colorAttachType[*iter] == AttachType::NotAttached)
						return false;
					indices.push_back(GL_COLOR_ATTACHMENT0 + *iter);
				}
				this->bind();
				glDrawBuffers(indices.size(), indices.data());
				return true;
			}
			bool checkStatus(void) {
				this->bind();
				return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
			}
			bool readColorAttachment(int index, GLenum format, GLenum type, void* dst) {
				if (index < 0 || index >= this->maxAttachments)
					return false;
				this->bind();
				if (this->colorAttachType[index] == AttachType::Texture2D) {
					glBindTexture(GL_TEXTURE_2D, this->colorAttachments[index]);
					glGetTexImage(GL_TEXTURE_2D, 0, format, type, dst);
				}
				else if (this->colorAttachType[index] == AttachType::RenderBuffer) {
					glReadBuffer(GL_COLOR_ATTACHMENT0 + index);
					glReadPixels(0, 0, this->_width, this->_height, format, type, dst);
				}
				else
					return false;
				return true;
			}
			bool readDepthAttachment(GLenum type, void* dst) {
				this->bind();
				if (this->depthAttachType == AttachType::Texture2D) {
					glBindTexture(GL_TEXTURE_2D, this->depthAttachment);
					glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, type, dst);
				}
				else if (this->depthAttachType == AttachType::RenderBuffer) {
					glReadPixels(0, 0, this->_width, this->_height, GL_DEPTH_COMPONENT, type, dst);
				}
				else
					return false;
				return true;
			}
			bool readStencilAttachment(GLenum format, GLenum type, void* dst) {
				this->bind();
				if (this->stencilAttachType == AttachType::Texture2D) {
					glBindTexture(GL_TEXTURE_2D, this->stencilAttachment);
					glGetTexImage(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX, type, dst);
				}
				else if (this->stencilAttachType == AttachType::RenderBuffer) {
					glReadPixels(0, 0, this->_width, this->_height, GL_STENCIL_INDEX, type, dst);
				}
				else
					return false;
				return true;
			}
			~FrameBuffer(void) {
				for (int i = 0; i < this->maxAttachments; i++)
					this->clearColorAttachment(i);
				this->clearDepthAttachment();
				this->clearStencilAttachment();
				glDeleteFramebuffers(1, &this->fbo);
			}

		private:
			GLsizei _width, _height;
			GLuint fbo;
			GLint maxAttachments;
			std::vector<GLuint> colorAttachments;
			std::vector<GLenum> colorAttachFormat;
			std::vector<AttachType> colorAttachType;
			GLuint depthAttachment, stencilAttachment;
			GLenum depthAttachFormat, stencilAttachFormat;
			AttachType depthAttachType, stencilAttachType;
		};

	}
}

#endif /* jjyou_gl_FrameBuffer_hpp */