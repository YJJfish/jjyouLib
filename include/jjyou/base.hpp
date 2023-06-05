#ifndef jjyou_base_hpp
#define jjyou_base_hpp

#include <stdexcept>
#include <string>
#include <chrono>

namespace jjyou {
	
	template<class Type, size_t Size>
	struct ArrayBase {
		ArrayBase() {};
		~ArrayBase() {};
		size_t size() const {
			return Size;
		}
		Type& operator[](int pos) {
			return ((Type*)this)[pos];
		}
		const Type& operator[](int pos) const {
			return ((const Type*)this)[pos];
		}
		Type& at(int pos) {
			if (pos < 0 || pos >= Size)
				throw std::out_of_range("pos " + std::to_string(pos) + " is out of range [0.." + std::to_string(Size) + ")");
			return ((Type*)this)[pos];
		}
		const Type& at(int pos) const {
			if (pos < 0 || pos >= Size)
				throw std::out_of_range("pos " + std::to_string(pos) + " is out of range [0.." + std::to_string(Size) + ")");
			return ((const Type*)this)[pos];
		}
	};
	template<class Type>
	struct Point2 : public ArrayBase<Type, 2> {
		Type x, y;
		Point2() : x(0), y(0) {}
		Point2(Type x, Type y) : x(x), y(y) {}
	};
	template<class Type>
	struct Point3 : public ArrayBase<Type, 3> {
		Type x, y, z;
		Point3() : x(0), y(0), z(0) {}
		Point3(Type x, Type y, Type z) : x(x), y(y), z(z) {}
	};
	template<class Type>
	struct Point4 : public ArrayBase<Type, 4> {
		Type x, y, z, w;
		Point4() : x(0), y(0), z(0), w(0) {}
		Point4(Type x, Type y, Type z, Type w) : x(x), y(y), z(z), w(w) {}
	};
	typedef Point2<int> Point2i;
	typedef Point3<int> Point3i;
	typedef Point4<int> Point4i;
	typedef Point2<float> Point2f;
	typedef Point3<float> Point3f;
	typedef Point4<float> Point4f;
	typedef Point2<double> Point2d;
	typedef Point3<double> Point3d;
	typedef Point4<double> Point4d;
	typedef Point2<float> Normal2f;
	typedef Point3<float> Normal3f;
	typedef Point2<double> Normal2d;
	typedef Point3<double> Normal3d;
	
	template<class Type>
	struct ColorRGB : ArrayBase<Type, 3> {
		Type r, g, b;
		ColorRGB() : r(0), g(0), b(0) {}
		ColorRGB(Type r, Type g, Type b) : r(r), g(g), b(b) {}
	};
	template<class Type>
	struct ColorRGBA : ArrayBase<Type, 4> {
		Type r, g, b, a;
		ColorRGBA() : r(0), g(0), b(0), a(0) {}
		ColorRGBA(Type r, Type g, Type b, Type a) : r(r), g(g), b(b), a(a) {}
	};
	typedef ColorRGB<float> ColorRGBf;
	typedef ColorRGBA<float> ColorRGBAf;
	typedef ColorRGB<double> ColorRGBd;
	typedef ColorRGBA<double> ColorRGBAd;
	typedef ColorRGB<unsigned char> ColorRGBu;
	typedef ColorRGBA<unsigned char> ColorRGBAu;
}

#endif /* jjyou_base_hpp */