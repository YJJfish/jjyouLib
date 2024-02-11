/***********************************************************************
 * @file	qua.hpp
 * @author	jjyou
 * @date	2024-2-2
 * @brief	This file implements jjyou::glsl::qua<T> class.
***********************************************************************/

#ifndef jjyou_glsl_qua_hpp
#define jjyou_glsl_qua_hpp

#include <array>

namespace jjyou{

	namespace glsl {

		template <class T>
		class qua {

		public:

			/** @name	Type definitions and inline constants.
			  */
			//@{
			using value_type = T;
			using length_type = std::size_t;
			using reference = value_type&;
			using const_reference = const value_type&;
			static inline length_type length = 4;
			//@}

		public:

			/** @name	Data storage.
			  */
			//@{
			union {
				std::array<value_type, 4> data;
				struct {
					value_type x, y, z, w;
				};
			};
			//@}

		public:

			/** @name	Constructors.
			  */
			//@{
			constexpr qua(void) : data() {}
			qua(const qua& q) : data(q.data) {}
			qua(qua&& q) : data(std::move(q.data)) {}
			qua(value_type x, value_type y, value_type z, value_type w) : x(x), y(y), z(z), w(w) {}
			/** @brief	Initialize each component of the vector to a scalar.
			  */
			
			//@}

			/** @name	Conversion to matrix.
			  */
			//@{
			operator mat<T, 3, 3>(void) const {
				constexpr T one = static_cast<T>(1.0);
				constexpr T two = static_cast<T>(2.0);
				T qxx(this->x * this->x);
				T qxy(this->x * this->y);
				T qxz(this->x * this->z);
				T qyy(this->y * this->y);
				T qyz(this->y * this->z);
				T qzz(this->z * this->z);
				T qwx(this->w * this->x);
				T qwy(this->w * this->y);
				T qwz(this->w * this->z);
				T qww(this->w * this->w);
				T s = one / (qxx + qyy + qzz + qww);
				mat<T, 3, 3> res(
					one - s * two * (qyy + qzz), two * s * (qxy + qwz), two * s * (qxz - qwy),
					two * s * (qxy - qwz), one - two * s * (qxx + qzz), two * s * (qyz + qwx),
					two * s * (qxz + qwy), two * s * (qyz - qwx), one - two * s * (qxx + qyy)
				);
				return res;
			}
			operator mat<T, 4, 4>(void) const {
				return mat<T, 4, 4>(this->operator mat<T, 3, 3>());
			}
			//@}

		};

		/** @name	Type definitions for convenience.
		  */
		//@{
		using quat = qua<float>;
		using dquat = qua<double>;
		//@}

	}

}



#endif /* jjyou_glsl_qua_hpp */