#ifndef jjyou_utils_hpp
#define jjyou_utils_hpp

#include <string>
#include <chrono>
#include <type_traits>

namespace jjyou {
	namespace utils {
		//Clock class
		class Clock {
		public:
			void begin(void) {
				clockBegin = std::chrono::steady_clock::now();
			}
			double end(void) {
				clockEnd = std::chrono::steady_clock::now();
				return std::chrono::duration_cast<std::chrono::duration<double>>(clockEnd - clockBegin).count();
			}
		private:
			std::chrono::steady_clock::time_point clockBegin, clockEnd;
		};

		//trim from start (in place)
		static inline void ltrim(std::string& s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
				return !std::isspace(ch);
				}));
		}

		//trim from end (in place)
		static inline void rtrim(std::string& s) {
			s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
				return !std::isspace(ch);
				}).base(), s.end());
		}

		//trim from both ends (in place)
		static inline void trim(std::string& s) {
			ltrim(s);
			rtrim(s);
		}

		//trim from start (copying)
		static inline std::string ltrim_copy(std::string s) {
			ltrim(s);
			return s;
		}

		//trim from end (copying)
		static inline std::string rtrim_copy(std::string s) {
			rtrim(s);
			return s;
		}

		//trim from both ends (copying)
		static inline std::string trim_copy(std::string s) {
			trim(s);
			return s;
		}

		template <class T> static inline T string2Number(const std::string& str);
		template <> static inline unsigned char string2Number<unsigned char>(const std::string& str) {
			return std::stoul(str);
		}
		template <> static inline char string2Number<char>(const std::string& str) {
			return std::stoi(str);
		}
		template <> static inline unsigned short string2Number<unsigned short>(const std::string& str) {
			return std::stoul(str);
		}
		template <> static inline short string2Number<short>(const std::string& str) {
			return std::stoi(str);
		}
		template <> static inline unsigned int string2Number<unsigned int>(const std::string& str) {
			return std::stoul(str);
		}
		template <> static inline int string2Number<int>(const std::string& str) {
			return std::stoi(str);
		}
		template <> static inline unsigned long string2Number<unsigned long>(const std::string& str) {
			return std::stoul(str);
		}
		template <> static inline long string2Number<long>(const std::string& str) {
			return std::stol(str);
		}
		template <> static inline unsigned long long string2Number<unsigned long long>(const std::string& str) {
			return std::stoull(str);
		}
		template <> static inline long long string2Number<long long>(const std::string& str) {
			return std::stoll(str);
		}
		template <> static inline float string2Number<float>(const std::string& str) {
			return std::stof(str);
		}
		template <> static inline double string2Number<double>(const std::string& str) {
			return std::stod(str);
		}
		template <> static inline long double string2Number<long double>(const std::string& str) {
			return std::stold(str);
		}

		//convert between little endian and big endian
		template <class T>
		inline void byteswap(T& value) {
			std::reverse((char*)&value, (char*)(&value + 1));
		}
		inline void byteswap(void* valuePtr, size_t bytes) {
			std::reverse((char*)valuePtr, (char*)valuePtr + bytes);
		}
		
		//type conversion for colors, from integral to floating point
		template<class SrcT, class DstT>
		typename std::enable_if_t<std::is_integral_v<SrcT> && std::is_floating_point_v<DstT>, DstT>
			color_cast(const SrcT& v) {
			return static_cast<DstT>(v / DstT(255.0));
		}

		//type conversion for colors, from floating point to integral
		template<class SrcT, class DstT>
		typename std::enable_if_t<std::is_floating_point_v<SrcT> && std::is_integral_v<DstT>, DstT>
			color_cast(const SrcT& v) {
			return static_cast<DstT>(std::clamp(std::round(v * SrcT(255.0)), SrcT(0.0), SrcT(255.0)));
		}
		
		//type conversion for colors, from integral to integral
		template<class SrcT, class DstT>
		typename std::enable_if_t<std::is_integral_v<SrcT> && std::is_integral_v<DstT>, DstT>
			color_cast(const SrcT& v) {
			return static_cast<DstT>(std::clamp(v, SrcT(0), SrcT(255)));
		}

		//type conversion for colors, from floating point to floating point
		template<class SrcT, class DstT>
		typename std::enable_if_t<std::is_floating_point_v<SrcT>&& std::is_floating_point_v<DstT>, DstT>
			color_cast(const SrcT& v) {
			return static_cast<DstT>(std::clamp(v, SrcT(0.0), SrcT(1.0)));
		}
	}
}

#endif /* jjyou_utils_hpp */