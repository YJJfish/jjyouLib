/***********************************************************************
 * @file	CameraView.hpp
 * @author	jjyou
 * @date	2023-6-3
 * @brief	This file implements CameraView class.
***********************************************************************/
#ifndef jjyou_gl_CameraView_hpp
#define jjyou_gl_CameraView_hpp

#include <numbers>
#include <algorithm>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace jjyou {
	namespace gl {

		/***********************************************************************
		 * @class CameraView
		 * @brief Camera view class for computing the view matrix in OpenGL.
		 *
		 * This class provides C++ API for adjusting the pose of a camera and
		 * computing the corresponding view matrix for passing to the OpenGL shader.
		 * The position of the camera is determined by a 3-dimensional vector, and
		 * the orientation of the camera is determined by yaw, pitch and roll. \n
		 * Yaw, pitch and roll are defined as follows:
		 * \code
		 * Yaw:
		 *  +z
		 *  |   /
		 *  |  /
		 *  | /
		 *  |/ yaw
		 *  +---------- +x
		 * 
		 * Pitch:
		 *  +y  front
		 *  |   /
		 *  |  /
		 *  | /
		 *  |/ pitch
		 *  +---------- x/z plane
		 * 
		 * Roll:
		 *    up
		 *    |       /
		 *    |roll /
		 *    |   /
		 *    | /
		 *  front------- right
		 * \endcode
		 * By default, the camera is located at `(0,0,0)`, looking in the
		 * direction of `(0,0,-1)`, with its "up" vector equal to `(0,1,0)`.
		 ***********************************************************************/
		class CameraView {

		public:

			/** @brief Default constructor.
			  *
			  * Construct and set the camera pose to default.
			  */
			CameraView(void);

			/** @brief Reset the camera pose to default.
			  */
			void reset(void);

		public:

			/** @brief Get the position of the camera.
			  */
			const glm::vec3& getPos(void) const;

			/** @brief Set the position of the camera.
			  */
			void setPos(const glm::vec3& pos);

			/** @brief Set the position of the camera.
			  */
			void setPos(GLfloat posX, GLfloat posY, GLfloat posZ);

			/** @brief Get the "front" direction of the camera.
			  */
			const glm::vec3& getFront(void) const;

			/** @brief Get the "back" direction of the camera.
			  */
			glm::vec3 getBack(void) const;

			/** @brief Get the "up" direction of the camera.
			  */
			const glm::vec3& getUp(void) const;

			/** @brief Get the "down" direction of the camera.
			  */
			glm::vec3 getDown(void) const;

			/** @brief Get the "left" direction of the camera.
			  */
			const glm::vec3& getLeft(void) const;

			/** @brief Get the "right" direction of the camera.
			  */
			glm::vec3 getRight(void) const;

			/** @brief Get the yaw angle (in radians) of the camera.
			  */
			GLfloat getYaw(void) const;

			/** @brief Set the yaw angle (in radians) of the camera.
			  */
			void setYaw(GLfloat yaw);

			/** @brief Get the pitch angle (in radians) of the camera.
			  */
			GLfloat getPitch(void) const;

			/** @brief Set the pitch angle (in radians) of the camera.
			  */
			void setPitch(GLfloat pitch);

			/** @brief Get the roll angle (in radians) of the camera.
			  */
			GLfloat getRoll(void) const;

			/** @brief Set the roll angle (in radians) of the camera.
			  */
			void setRoll(GLfloat roll);

		public:
			
			/** @brief Set the orientation of the camera.
			  * @param dYaw		The increment of yaw.
			  * @param dPitch	The increment of pitch.
			  * @param dRoll	The increment of roll.
			  */
			void turn(GLfloat dYaw, GLfloat dPitch, GLfloat dRoll);

			/** @brief Set the orientation of the camera.
			  * @param yaw		The new value of yaw.
			  * @param pitch	The new value of pitch.
			  * @param roll		The new value of roll.
			  */
			void turnTo(GLfloat yaw, GLfloat pitch, GLfloat roll);

			/** @brief Set the position of the camera.
			  * @param dPos		The increment of position.
			  */
			void move(const glm::vec3& dPos);

			/** @brief Set the position of the camera.
			  * @param dPosX	The x component of the increment of position.
			  * @param dPosY	The y component of the increment of position.
			  * @param dPosZ	The z component of the increment of position.
			  */
			void move(GLfloat dPosX, GLfloat dPosY, GLfloat dPosZ);

			/** @brief Set the position of the camera.
			  * @param pos		The new value of position.
			  */
			void moveTo(const glm::vec3& pos);

			/** @brief Set the position of the camera.
			  * @param posX		The x component of the new value of position.
			  * @param posY		The y component of the new value of position.
			  * @param posZ		The z component of the new value of position.
			  */
			void moveTo(GLfloat posX, GLfloat posY, GLfloat posZ);

			/** @brief Move the camera along the "front" direction.
			  * @param dist		The length of movement.
			  */
			void moveFront(GLfloat dist);

			/** @brief Move the camera along the "back" direction.
			  * @param dist		The length of movement.
			  */
			void moveBack(GLfloat dist);

			/** @brief Move the camera along the "left" direction.
			  * @param dist		The length of movement.
			  */
			void moveLeft(GLfloat dist);

			/** @brief Move the camera along the "right" direction.
			  * @param dist		The length of movement.
			  */
			void moveRight(GLfloat dist);

			/** @brief Move the camera along the "up" direction.
			  * @param dist		The length of movement.
			  */
			void moveUp(GLfloat dist);

			/** @brief Move the camera along the "down" direction.
			  * @param dist		The length of movement.
			  */
			void moveDown(GLfloat dist);

			/** @brief Get the view matrix for passing to the OpenGL shader.
			  * @return		The view matrix for the current camera pose.
			  */
			glm::mat4 getViewMatrix(void) const;

		private:
			glm::vec3 pos;
			glm::vec3 front;
			glm::vec3 up;
			glm::vec3 left;
			GLfloat yaw, pitch, roll;
			void updateOrientation(void);
		};
	}
}


/*======================================================================
 | Implementation
 ======================================================================*/
 /// @cond


namespace jjyou {
	namespace gl {

		inline CameraView::CameraView(void) :
			pos(glm::vec3(0.0f)),
			yaw(std::numbers::pi_v<GLfloat> * 1.5f),
			pitch(0.0f),
			roll(0.0f)
		{
			this->updateOrientation();
		}

		inline void CameraView::reset(void) {
			constexpr GLfloat pi = std::numbers::pi_v<GLfloat>;
			this->pos = glm::vec3(0.0f);
			this->yaw = pi * 1.5f;
			this->pitch = 0.0f;
			this->roll = 0.0f;
			this->updateOrientation();
		}

		inline const glm::vec3& CameraView::getPos(void) const {
			return this->pos;
		}

		inline void CameraView::setPos(const glm::vec3& pos) {
			this->pos = pos;
		}

		inline void CameraView::setPos(GLfloat posX, GLfloat posY, GLfloat posZ) {
			this->pos.x = posX;
			this->pos.y = posY;
			this->pos.z = posZ;
		}

		inline const glm::vec3& CameraView::getFront(void) const {
			return this->front;
		}

		inline glm::vec3 CameraView::getBack(void) const {
			return -this->front;
		}

		inline const glm::vec3& CameraView::getUp(void) const {
			return this->up;
		}

		inline glm::vec3 CameraView::getDown(void) const {
			return -this->up;
		}

		inline const glm::vec3& CameraView::getLeft(void) const {
			return this->left;
		}

		inline glm::vec3 CameraView::getRight(void) const {
			return -this->left;
		}

		inline GLfloat CameraView::getYaw(void) const {
			return this->yaw;
		}

		inline void CameraView::setYaw(GLfloat yaw) {
			constexpr GLfloat pi = std::numbers::pi_v<GLfloat>;
			this->yaw = std::fmod(yaw, 2.0f * pi);
			this->updateOrientation();
		}

		inline GLfloat CameraView::getPitch(void) const {
			return this->pitch;
		}

		inline void CameraView::setPitch(GLfloat pitch) {
			constexpr GLfloat pi = std::numbers::pi_v<GLfloat>;
			this->pitch = std::clamp<GLfloat>(pitch, -pi / 2.0f, pi / 2.0f);
			this->updateOrientation();
		}

		inline GLfloat CameraView::getRoll(void) const {
			return this->roll;
		}

		inline void CameraView::setRoll(GLfloat roll) {
			constexpr GLfloat pi = std::numbers::pi_v<GLfloat>;
			this->roll = std::fmod(roll, 2.0f * pi);
			this->updateOrientation();
		}

		inline void CameraView::turn(GLfloat dYaw, GLfloat dPitch, GLfloat dRoll) {
			constexpr GLfloat pi = std::numbers::pi_v<GLfloat>;
			this->yaw = std::fmod(this->yaw + dYaw, 2.0f * pi);
			this->pitch = std::clamp<GLfloat>(this->pitch + dPitch, -pi / 2.0f, pi / 2.0f);
			this->roll = std::fmod(this->roll + dRoll, 2.0f * pi);
			this->updateOrientation();
		}

		inline void CameraView::turnTo(GLfloat yaw, GLfloat pitch, GLfloat roll) {
			GLfloat pi = std::numbers::pi_v<GLfloat>;
			this->yaw = std::fmod(yaw, 2.0f * pi);
			this->pitch = std::clamp<GLfloat>(pitch, -pi / 2.0f, pi / 2.0f);
			this->roll = std::fmod(roll, 2.0f * pi);
			this->updateOrientation();
		}

		inline void CameraView::move(const glm::vec3& dPos) {
			this->pos += dPos;
		}

		inline void CameraView::move(GLfloat dPosX, GLfloat dPosY, GLfloat dPosZ) {
			this->pos.x += dPosX;
			this->pos.y += dPosY;
			this->pos.z += dPosZ;
		}

		inline void CameraView::moveTo(const glm::vec3& pos) {
			this->pos = pos;
		}

		inline void CameraView::moveTo(GLfloat posX, GLfloat posY, GLfloat posZ) {
			this->pos.x = posX;
			this->pos.y = posY;
			this->pos.z = posZ;
		}

		inline void CameraView::moveFront(GLfloat dist) {
			this->pos += dist * this->front;
		}

		inline void CameraView::moveBack(GLfloat dist) {
			this->pos -= dist * this->front;
		}

		inline void CameraView::moveLeft(GLfloat dist) {
			this->pos += dist * this->left;
		}

		inline void CameraView::moveRight(GLfloat dist) {
			this->pos -= dist * this->left;
		}

		inline void CameraView::moveUp(GLfloat dist) {
			this->pos += dist * this->up;
		}

		inline void CameraView::moveDown(GLfloat dist) {
			this->pos -= dist * this->up;
		}

		inline glm::mat4 CameraView::getViewMatrix(void) const {
			return glm::lookAt(
				this->pos,
				this->pos + this->front,
				this->up
			);
		}

		inline void CameraView::updateOrientation(void) {
			this->front.x = std::cos(this->pitch) * std::cos(this->yaw);
			this->front.y = std::sin(this->pitch);
			this->front.z = std::cos(this->pitch) * std::sin(this->yaw);
			this->up.x = -std::sin(this->pitch) * std::cos(this->yaw);
			this->up.y = std::cos(this->pitch);
			this->up.z = -std::sin(this->pitch) * std::sin(this->yaw);
			this->up = glm::mat3(glm::rotate(glm::mat4(1.0f), this->roll, this->front)) * this->up;
			this->left = glm::cross(this->up, this->front);
		}

	}
}
/// @endcond

#endif /* jjyou_gl_CameraView_hpp */
