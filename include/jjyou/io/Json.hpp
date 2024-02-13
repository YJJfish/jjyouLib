/***********************************************************************
 * @file	Json.hpp
 * @author	jjyou
 * @date	2024-2-9
 * @brief	This file implements Json class.
***********************************************************************/
#ifndef jjyou_io_Json_hpp
#define jjyou_io_Json_hpp

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <stack>
#include <map>
#include <exception>
#include "GenericLexer.hpp"
#include "../utils.hpp"

namespace jjyou {

	namespace io {

		/*============================================================
		 *                    Forward declarations
		 *============================================================*/
		enum class JsonType;
		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy> class Json;
		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy> class JsonIterator;
		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy> class JsonConstIterator;
		class JsonLexer;
		/*============================================================
		 *                 End of forward declarations
		 *============================================================*/

		/** @brief	JsonType enum class. It used to identify the type of value stored in a json container.
		  */
		enum class JsonType {
			Null = 0,
			Integer = 1,
			Floating = 2,
			String = 3,
			Bool = 4,
			Array = 5,
			Object = 6
		};

		/** @brief	Helper function to convert JsonType to std::string.
		  */
		inline std::string to_string(JsonType type);

		/** @brief	Helper function to write JsonType to output stream.
		  */
		inline std::ostream& operator<<(std::ostream& out, JsonType type);

		/** @brief	Helper function to print Json to output stream.
		  */
		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline std::ostream& operator<<(std::ostream& out, const Json<IntegerTy, FloatingTy, StringTy, BoolTy>& json);

		/** @brief	Helper function to print Json to a string.
		  * @note	This function is different from `Json::operator StringType(void) const`.
		  */
		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline StringTy to_string(const Json<IntegerTy, FloatingTy, StringTy, BoolTy>& json);

		/** @brief	Json class for reading/writing json files.
		  *
		  * @tparam	IntegerTy	The integer type. Default is `int`.
		  * @tparam	FloatingTy	The floating point type. Default is `float`.
		  * @tparam	StringTy	The string type. Default is `std::string`. It must meet
		  *						these requirements:
		  *						 - strict weak orderable (i.e. StringTy::operator< is properly implemented.)
		  * @tparam	BoolTy		The boolean type. Default is `bool`.
		  */
		template <
			class IntegerTy = int,
			class FloatingTy = float,
			class StringTy = std::string,
			class BoolTy = bool
		>
		class Json {

		public:

			/** @name	Type definitions and inline constants.
			  */
			//@{
			using value_type = Json;
			using size_type = std::size_t;
			using reference = value_type&;
			using const_reference = const value_type&;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			using iterator = JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>;
			using const_iterator = JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>;
			using LexerType = JsonLexer; // will be replaced by `jjyou::io::GenericLexer` in the future.
			using IntegerType = IntegerTy;
			using FloatingType = FloatingTy;
			using StringType = StringTy;
			using BoolType = BoolTy;
			using ArrayType = std::vector<Json>;
			using ObjectType = std::map<StringTy, Json>;
			//@}

		public:

			/** @brief	Parse a string.
			  */
			static Json parse(const StringType& string);
			template <class T>
			static Json parse(T* string);

			/** @brief	Read and parse a file.
			  */
			static Json parse(const std::filesystem::path& path);

			/** @brief	Default constructor. Create a "null" json container.
			  */
			Json(void) : _type(JsonType::Null), dummy(nullptr) {}

			/** @brief	Construct a json container from an integer.
			  */
			Json(IntegerType integer) : _type(JsonType::Integer), pInteger(new IntegerType(integer)) {}

			/** @brief	Construct a json container from a floating point.
			  */
			Json(FloatingType floating) : _type(JsonType::Floating), pFloating(new FloatingType(floating)) {}

			/** @brief	Construct a json container from a string.
			  */
			Json(const StringType& string) : _type(JsonType::String), pString(new StringType(string)) {}

			/** @brief	Construct a json container from a string.
			  */
			Json(StringType&& string) : _type(JsonType::String), pString(new StringType(std::move(string))) {}

			/** @brief	Delegating constructor. Construct a string json.
			  * @note	Without this function, Json("123") will result in a Bool json.
			  */
			template <class T>
			Json(T* string) : _type(JsonType::String), pString(new StringType(string)) {}


			/** @brief	Construct a json container from a boolean.
			  */
			Json(BoolType boolean) : _type(JsonType::Bool), pBool(new BoolType(boolean)) {}

			/** @brief	Construct a json container from an array.
			  */
			Json(const ArrayType& array) : _type(JsonType::Array), pArray(new ArrayType(array)) {}

			/** @brief	Construct a json container from an array.
			  */
			Json(ArrayType&& array) : _type(JsonType::Array), pArray(new ArrayType(std::move(array))) {}

			/** @brief	Construct a json container from an initializer list.
			  */
			template <class T>
			Json(std::initializer_list<T> array) : _type(JsonType::Array), pArray(new ArrayType()) {
				this->pArray->reserve(array.size());
				for (const T& v : array)
					this->pArray->emplace_back(v);
			}
			
			/** @brief	Construct a json container from an object.
			  */
			Json(const ObjectType& object) : _type(JsonType::Object), pObject(new ObjectType(object)) {}

			/** @brief	Construct a json container from an object.
			  */
			Json(ObjectType&& object) : _type(JsonType::Object), pObject(new ObjectType(std::move(object))) {}
			
			/** @brief	Construct a json container from an initializer list of pairs.
			  */
			template <class S, class T>
			Json(std::initializer_list<std::pair<S, T>> object) : _type(JsonType::Object), pObject(new ObjectType()) {
				for (const std::pair<S, T>& p : object)
					this->pObject->emplace(p.first, p.second);
			}
			
			/** @brief	Construct a json container of the specified type with default value.
			  */
			explicit Json(JsonType type) : _type(JsonType::Null), dummy(nullptr) {
				this->_create(type);
			}

			/** @brief	Copy constructor.
			  */
			Json(const Json& json) : _type(JsonType::Null), dummy(nullptr) {
				this->_assign(json);
			}

			/** @brief	Move constructor.
			  */
			Json(Json&& json) : _type(JsonType::Null), dummy(nullptr) {
				this->_assign(std::move(json));
			}

			/** @brief	Copy assignment.
			  */
			Json& operator=(const Json& json) {
				this->_reset();
				this->_assign(json);
			}

			/** @brief	Move assignment.
			  */
			Json& operator=(Json&& json) {
				this->_reset();
				this->_assign(std::move(json));
			}
			
			/** @brief	Destructor
			  */
			~Json(void) {
				this->_reset();
			}

			/** @brief	Create a json container of the specified type with default value.
			  *			The old values in this container will be cleared.
			  */
			void create(JsonType type) {
				this->_reset();
				this->_create(type);
			}

			/** @brief	Reset to null.
			  */
			void reset(void) {
				this->_reset();
			}

			/** @brief	Get the value type stored in the json container.
			  * @return	The value type of type `jjyou::io::JsonType`.
			  */
			JsonType type(void) const { return this->_type; }

			/** @brief	Check whether the Json container is Null.
			  * @return `true` if the Json container is Null.
			  */
			bool isNull(void) const { return this->_type == JsonType::Null; }

			/** @brief	Convert the Json to an integer.
			  *
			  *			This function is valid only if the Json's type is `JsonType::Integer`,
			  *			`JsonType::Floating`, or `JsonType::Bool`.
			  *			Otherwise, an exception of type std::runtime_error is thrown.
			  */
			explicit operator IntegerType(void) const {
				switch (this->_type) {
				case JsonType::Integer:
					return *this->pInteger;
				case JsonType::Floating:
					return static_cast<IntegerType>(*this->pFloating);
				case JsonType::Bool:
					return static_cast<IntegerType>(*this->pBool);
				case JsonType::Null:
				case JsonType::String:
				case JsonType::Array:
				case JsonType::Object:
					throw std::runtime_error("This function is valid only if the Json container is an integer, floating, or bool.");
				}
			}

			/** @brief	Convert the Json to a floating point number.
			  *
			  *			This function is valid only if the Json's type is `JsonType::Integer`,
			  *			`JsonType::Floating`, or `JsonType::Bool`.
			  *			Otherwise, an exception of type std::runtime_error is thrown.
			  */
			explicit operator FloatingType(void) const {
				switch (this->_type) {
				case JsonType::Integer:
					return static_cast<FloatingType>(*this->pInteger);
				case JsonType::Floating:
					return *this->pFloating;
				case JsonType::Bool:
					return static_cast<FloatingType>(*this->pBool);
				case JsonType::Null:
				case JsonType::String:
				case JsonType::Array:
				case JsonType::Object:
					throw std::runtime_error("This function is valid only if the Json container is an integer, floating, or bool.");
				}
			}

			/** @brief	Convert the Json to a string.
			  *
			  *			This function is valid only if the Json's type is `JsonType::String`.
			  *			Otherwise, an exception of type std::runtime_error is thrown.
			  */
			explicit operator StringType(void) const {
				if (this->_type == JsonType::String)
					return *this->pString;
				else
					throw std::runtime_error("This function is valid only if the Json container is a string.");
			}

			/** @brief	Convert the Json to a bool.
			  *
			  *			This function is valid only if the Json's type is `JsonType::Integer`,
			  *			`JsonType::Floating`, or `JsonType::Bool`.
			  *			Otherwise, an exception of type std::runtime_error is thrown.
			  */
			explicit operator BoolType(void) const {
				switch (this->_type) {
				case JsonType::Integer:
					return static_cast<BoolType>(*this->pInteger);
				case JsonType::Floating:
					return static_cast<BoolType>(*this->pFloating);
				case JsonType::Bool:
					return *this->pBool;
				case JsonType::Null:
				case JsonType::String:
				case JsonType::Array:
				case JsonType::Object:
					throw std::runtime_error("This function is valid only if the Json container is an integer, floating, or bool.");
				}
			}

			/** @brief	Convert the Json to a std::vector.
			  *
			  *			This function is valid only if the Json's type is `JsonType::Array`,
			  *			and all the elements in the array (whose type are also Json) can be
			  *			converted to the template type `T`.
			  *			Otherwise, an exception of type std::runtime_error is thrown.
			  */
			template <class T>
			explicit operator std::vector<T>(void) const {
				if (this->_type == JsonType::Array) {
					std::vector<T> res; res.reserve(this->pArray->size());
					for (const Json& v : (*this->pArray))
						res.emplace_back(v);
					return res;
				}
				else
					throw std::runtime_error("This function is valid only if the Json container is an array.");
			}

			/** @brief	Convert the Json to a std::map.
			  *
			  *			This function is valid only if the Json's type is `JsonType::Object`,
			  *			and all the elements in the object (whose type are also Json) can be
			  *			converted to the template type `T`.
			  *			Otherwise, an exception of type std::runtime_error is thrown.
			  */
			template <class T>
			explicit operator std::map<StringType, T>(void) const {
				if (this->_type == JsonType::Object) {
					std::map<StringType, T> res;
					for (auto cIter = this->pObject->cbegin(); cIter != this->pObject->cend(); ++cIter)
						res.emplace(cIter->first, cIter->second);
					return res;
				}
				else
					throw std::runtime_error("This function is valid only if the Json container is an object.");
			}

			/** @brief	Get the integer stored in the container. If the type does not match,
			  *			the behavior is undefined.
			  */
			IntegerType& integer(void) {
				return *this->pInteger;
			}
			const IntegerType& integer(void) const {
				return *this->pInteger;
			}

			/** @brief	Get the floating stored in the container. If the type does not match,
			  *			the behavior is undefined.
			  */
			FloatingType& floating(void) {
				return *this->pFloating;
			}
			const FloatingType& floating(void) const {
				return *this->pFloating;
			}

			/** @brief	Get the string stored in the container. If the type does not match,
			  *			the behavior is undefined.
			  */
			StringType& string(void) {
				return *this->pString;
			}
			const StringType& string(void) const {
				return *this->pString;
			}

			/** @brief	Get the bool stored in the container. If the type does not match,
			  *			the behavior is undefined.
			  */
			BoolType& boolean(void) {
				return *this->pBool;
			}
			const BoolType& boolean(void) const {
				return *this->pBool;
			}

			/** @brief	Get the array stored in the container. If the type does not match,
			  *			the behavior is undefined.
			  */
			ArrayType& array(void) {
				return *this->pArray;
			}
			const ArrayType& array(void) const {
				return *this->pArray;
			}

			/** @brief	Get the array stored in the container. If the type does not match,
			  *			the behavior is undefined.
			  */
			ObjectType& object(void) {
				return *this->pObject;
			}
			const ObjectType& object(void) const {
				return *this->pObject;
			}

			/** @brief	Get the reference to the value at a given position.
			  * 
			  * The index starts from 0. This function is valid only if the Json's
			  * type is `JsonType::Array`. Otherwise, the behavior is undefined.
			  * @return	The reference to the value at a given position.
			  */
			reference operator[](size_type pos) {
				return (*this->pArray)[pos];
			}

			/** @brief	Get the const reference to the value at a given position.
			  *
			  * The index starts from 0. This function is valid only if the Json's
			  * type is `JsonType::Array`. Otherwise, the behavior is undefined.
			  * @return	The const reference to the value at a given position.
			  */
			const_reference operator[](size_type pos) const {
				return (*this->pArray)[pos];
			}

			/** @brief	Get the reference to the value that is mapped to the given key,
			  *			performing an insertion if such key does not already exist.
			  *
			  * This function is valid only if the Json's type is `JsonType::Object`.
			  * Otherwise, the behavior is undefined.
			  * @return	The reference to the value that is mapped to the given key.
			  */
			reference operator[](const StringType& key) {
				return (*this->pObject)[key];
			}
			template <class T>
			reference operator[](T* key) {
				return (*this)[StringType(key)];
			}

			/** @brief	Get the const reference to the value that is mapped to the given key,
			  *
			  * This function is valid only if the Json's type is `JsonType::Object`, and the Json
			  * contains the given key.
			  * Otherwise, the behavior is undefined.
			  * @return	The const reference to the value that is mapped to the given key.
			  */
			const_reference operator[](const StringType& key) const {
				return this->pObject->find(key)->second;
			}
			template <class T>
			const_reference operator[](T* key) const {
				return (*this)[StringType(key)];
			}

			/** @brief	Get the reference to the value at a given position, with bounds checking.
			  *
			  * The index starts from 0. This function is valid only if the Json's
			  * type is `JsonType::Array`. Otherwise, an exception of type std::out_of_range is thrown.
			  * @return	The reference to the value at a given position.
			  */
			reference at(size_type pos) {
				if (this->_type != JsonType::Array)
					throw std::out_of_range("This function is valid only if the Json container is an array.");
				return this->pArray->at(pos);
			}

			/** @brief	Get the const reference to the value at a given position, with bounds checking.
			  *
			  * The index starts from 0. This function is valid only if the Json's
			  * type is `JsonType::Array`. Otherwise, an exception of type std::out_of_range is thrown.
			  * @return	The const reference to the value at a given position.
			  */
			const_reference at(size_type pos) const {
				if (this->_type != JsonType::Array)
					throw std::out_of_range("This function is valid only if the Json container is an array.");
				return this->pArray->at(pos);
			}

			/** @brief	Get the reference to the value that is mapped to the given key, with bounds checking.
			  *
			  * This function is valid only if the Json's type is `JsonType::Object`.
			  * Otherwise, an exception of type std::out_of_range is thrown.
			  * @return	The reference to the value that is mapped to the given key.
			  */
			reference at(const StringType& key) {
				if (this->_type != JsonType::Object)
					throw std::out_of_range("This function is valid only if the Json container is an object.");
				return this->pObject->at(key);
			}

			/** @brief	Get the const reference to the value that is mapped to the given key, with bounds checking.
			  *
			  * This function is valid only if the Json's type is `JsonType::Object`.
			  * Otherwise, an exception of type std::out_of_range is thrown.
			  * @return	The const reference to the value that is mapped to the given key.
			  */
			const_reference at(const StringType& key) const {
				if (this->_type != JsonType::Object)
					throw std::out_of_range("This function is valid only if the Json container is an object.");
				return this->pObject->at(key);
			}

			/** @brief	Get the size of the json container.
			  *
			  * For null, the size is always 0.
			  * For integer/floating/string/bool, the size is always 1.
			  * For array, the size is the array length.
			  * For object, the size is the number of key&value pairs.
			  * @return	The size of the json container.
			  */
			std::size_t size(void) const;

			/** @name	Iterator-related methods.
			  * @brief	Get the iterator pointing to the specified position.
			  */
			//@{
			iterator begin(void);
			const_iterator begin(void) const;
			const_iterator cbegin(void) const;
			iterator end(void);
			const_iterator end(void) const;
			const_iterator cend(void) const;
			//@}

			/** @brief	Find the value that is mapped to the given key.
			  *
			  * This function is valid only if the Json's type is `JsonType::Object`.
			  * Otherwise, an exception of type std::out_of_range is thrown.
			  * @return	The iterator to the value that is mapped to the given key. If not found,
			  *			an iterator pointing to the end of container will be returned.
			  */
			iterator find(const StringType& key);

			/** @brief	Find the value that is mapped to the given key.
			  *
			  * This function is valid only if the Json's type is `JsonType::Object`.
			  * Otherwise, an exception of type std::out_of_range is thrown.
			  * @return	The iterator to the value that is mapped to the given key. If not found,
			  *			an iterator pointing to the end of container will be returned.
			  */
			const_iterator find(const StringType& key) const;

			friend std::ostream& operator<< <IntegerTy, FloatingTy, StringTy, BoolTy>(std::ostream& out, const Json& json);

			friend StringTy to_string<IntegerTy, FloatingTy, StringTy, BoolTy>(const Json& json);

		private:

			void _reset(void);
			void _assign(const Json& json);
			void _assign(Json&& json);
			void _create(JsonType type);
			void _print(std::ostream& out, int indent) const;
			static Json _parse(JsonLexer& lexer);
			JsonType _type;
			union {
				void* dummy;
				IntegerType* pInteger;
				FloatingType* pFloating;
				StringType* pString;
				BoolType* pBool;
				ArrayType* pArray;
				ObjectType* pObject;
			};

		};

		/** @brief	Compare two iterators.
		  * @note	DO NOT compare two iterators belonging to different Json instances.
		  * @return	`true` if two iterators are considered to be equal.
		  */
		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy> inline bool operator==(const JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2);
		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy> inline bool operator==(const JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2);
		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy> inline bool operator==(const JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2);
		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy> inline bool operator==(const JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2);

		/** @brief	Compare two iterators.
		  * @note	DO NOT compare two iterators belonging to different Json instances.
		  * @return	`true` if two iterators are considered to be unequal.
		  */
		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy> inline bool operator!=(const JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2);
		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy> inline bool operator!=(const JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2);
		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy> inline bool operator!=(const JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2);
		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy> inline bool operator!=(const JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2);

		/** @brief	Iterator type for Json class.
		  */
		template <
			class IntegerTy,
			class FloatingTy,
			class StringTy,
			class BoolTy
		>
		class JsonIterator {

		public:

			/** @name	Type definitions and inline constants.
			  */
			//@{
			using value_type = Json<IntegerTy, FloatingTy, StringTy, BoolTy>;
			using reference = value_type&;
			using pointer = value_type*;
			//@}

			/** @name	Constructors, destructor and assignments.
			  */
			//@{
			JsonIterator(void) = default;
			JsonIterator(const JsonIterator&) = default;
			JsonIterator(JsonIterator&&) = default;
			~JsonIterator(void) = default;
			JsonIterator& operator=(const JsonIterator&) = default;
			JsonIterator& operator=(JsonIterator&&) = default;
			//@}

			/** @brief	Increment the iterator.
			  * @return	Copy of the iterator before being incremented.
			  */
			JsonIterator operator++(int);

			/** @brief	Increment the iterator.
			  * @return	Reference of the iterator after being incremented.
			  */
			JsonIterator& operator++(void);

			/** @brief	Decrement the iterator.
			  * @return	Copy of the iterator before being deremented.
			  */
			JsonIterator operator--(int);

			/** @brief	Decrement the iterator.
			  * @return	Reference of the iterator after being decremented.
			  */
			JsonIterator& operator--(void);

			/** @brief	Fetch the current element.
			  * @return	Reference of the current element.
			  */
			reference operator*() const;

			/** @brief	Fetch the current element.
			  * @return	Pointer to the current element.
			  */
			pointer operator->() const;

			/** @brief	Fetch the current element's key. This function is valid only if
			  *			the Json's type is `JsonType::Object`. Otherwise, an exception
			  *			of type std::logic_error is thrown.
			  * @return	The current element's key.
			  */
			StringTy key() const {
				if (this->pJson->type() == JsonType::Object)
					return this->objectIter->first;
				else
					throw std::logic_error("This function is valid only if the Json container is an Object.");
			}

			/** @brief	Fetch the current element's value. Same as operator*.
			  * @return	Reference of the current element.
			  */
			reference value() const {
				return (**this);
			}

		private:

			pointer pJson = nullptr;
			int pos = 0;
			typename value_type::ObjectType::iterator objectIter;
			typename value_type::ArrayType::iterator arrayIter;
			JsonIterator(pointer pJson, int pos) : pJson(pJson), pos(pos) {}
			JsonIterator(pointer pJson, typename value_type::ObjectType::iterator objectIter) : pJson(pJson), objectIter(objectIter) {}
			JsonIterator(pointer pJson, typename value_type::ArrayType::iterator arrayIter) : pJson(pJson), arrayIter(arrayIter) {}
			friend class Json<IntegerTy, FloatingTy, StringTy, BoolTy>;
			friend bool operator==<IntegerTy, FloatingTy, StringTy, BoolTy>(const JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2);
			friend bool operator==<IntegerTy, FloatingTy, StringTy, BoolTy>(const JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2);
			friend bool operator==<IntegerTy, FloatingTy, StringTy, BoolTy>(const JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2);
			friend bool operator==<IntegerTy, FloatingTy, StringTy, BoolTy>(const JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2);

		};

		/** @brief	Const iterator type for Json class.
		  */
		template <
			class IntegerTy,
			class FloatingTy,
			class StringTy,
			class BoolTy
		>
		class JsonConstIterator {

		public:

			/** @name	Type definitions and inline constants.
			  */
			//@{
			using value_type = Json<IntegerTy, FloatingTy, StringTy, BoolTy>;
			using reference = const value_type&;
			using pointer = const value_type*;
			//@}

			/** @name	Constructors, destructor and assignments.
			  */
			//@{
			JsonConstIterator(void) = default;
			JsonConstIterator(const JsonConstIterator&) = default;
			JsonConstIterator(JsonConstIterator&&) = default;
			~JsonConstIterator(void) = default;
			JsonConstIterator& operator=(const JsonConstIterator&) = default;
			JsonConstIterator& operator=(JsonConstIterator&&) = default;
			//@}

			/** @brief	Increment the iterator.
			  * @return	Copy of the iterator before being incremented.
			  */
			JsonConstIterator operator++(int);

			/** @brief	Increment the iterator.
			  * @return	Reference of the iterator after being incremented.
			  */
			JsonConstIterator& operator++(void);

			/** @brief	Decrement the iterator.
			  * @return	Copy of the iterator before being deremented.
			  */
			JsonConstIterator operator--(int);

			/** @brief	Decrement the iterator.
			  * @return	Reference of the iterator after being decremented.
			  */
			JsonConstIterator& operator--(void);

			/** @brief	Fetch the current element.
			  * @return	Reference of the current element.
			  */
			reference operator*() const;

			/** @brief	Fetch the current element.
			  * @return	Pointer to the current element.
			  */
			pointer operator->() const;

			/** @brief	Fetch the current element's key. This function is valid only if
			  *			the Json's type is `JsonType::Object`. Otherwise, an exception
			  *			of type std::logic_error is thrown.
			  * @return	The current element's key.
			  */
			StringTy key() const {
				if (this->pJson->type() == JsonType::Object)
					return this->objectIter->first;
				else
					throw std::logic_error("This function is valid only if the Json container is an Object.");
			}

			/** @brief	Fetch the current element's value. Same as operator*.
			  * @return	Reference of the current element.
			  */
			reference value() const {
				return (**this);
			}

		private:

			pointer pJson = nullptr;
			int pos = 0;
			typename value_type::ObjectType::const_iterator objectIter;
			typename value_type::ArrayType::const_iterator arrayIter;
			JsonConstIterator(pointer pJson, int pos) : pJson(pJson), pos(pos) {}
			JsonConstIterator(pointer pJson, typename value_type::ObjectType::const_iterator objectIter) : pJson(pJson), objectIter(objectIter) {}
			JsonConstIterator(pointer pJson, typename value_type::ArrayType::const_iterator arrayIter) : pJson(pJson), arrayIter(arrayIter) {}
			friend class Json<IntegerTy, FloatingTy, StringTy, BoolTy>;
			friend bool operator==<IntegerTy, FloatingTy, StringTy, BoolTy>(const JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2);
			friend bool operator==<IntegerTy, FloatingTy, StringTy, BoolTy>(const JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2);
			friend bool operator==<IntegerTy, FloatingTy, StringTy, BoolTy>(const JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2);
			friend bool operator==<IntegerTy, FloatingTy, StringTy, BoolTy>(const JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2);

		};

	}

}



/*======================================================================
 | Implementation
 ======================================================================*/
/// @cond

namespace jjyou {

	namespace io {

		inline std::string to_string(JsonType type) {
			switch (type) {
			case JsonType::Null:
				return "Null";
			case JsonType::Integer:
				return "Integer";
			case JsonType::Floating:
				return "Floating";
			case JsonType::String:
				return "String";
			case JsonType::Bool:
				return "Bool";
			case JsonType::Array:
				return "Array";
			case JsonType::Object:
				return "Object";
			}
		}

		inline std::ostream& operator<<(std::ostream& out, JsonType type) {
			return (out << to_string(type));
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline std::ostream& operator<<(std::ostream& out, const Json<IntegerTy, FloatingTy, StringTy, BoolTy>& json) {
			json._print(out, 0);
			return out;
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline StringTy to_string(const Json<IntegerTy, FloatingTy, StringTy, BoolTy>& json) {
			std::basic_stringstream<typename StringTy::value_type> sstream;
			sstream << json;
			return sstream.str();
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline std::size_t Json<IntegerTy, FloatingTy, StringTy, BoolTy>::size(void) const {
			switch (this->_type) {
			case JsonType::Null:
				return 0ULL;
			case JsonType::Integer:
				return 1ULL;
			case JsonType::Floating:
				return 1ULL;
			case JsonType::String:
				return 1ULL;
			case JsonType::Bool:
				return 1ULL;
			case JsonType::Array:
				return this->pArray->size();
			case JsonType::Object:
				return this->pObject->size();
			}
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline typename Json<IntegerTy, FloatingTy, StringTy, BoolTy>::iterator Json<IntegerTy, FloatingTy, StringTy, BoolTy>::begin(void) {
			switch (this->_type) {
			case JsonType::Null:
			case JsonType::Integer:
			case JsonType::Floating:
			case JsonType::String:
			case JsonType::Bool:
				return iterator(this, 0);
			case JsonType::Array:
				return iterator(this, this->pArray->begin());
			case JsonType::Object:
				return iterator(this, this->pObject->begin());
			}
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline typename Json<IntegerTy, FloatingTy, StringTy, BoolTy>::const_iterator Json<IntegerTy, FloatingTy, StringTy, BoolTy>::begin(void) const {
			switch (this->_type) {
			case JsonType::Null:
			case JsonType::Integer:
			case JsonType::Floating:
			case JsonType::String:
			case JsonType::Bool:
				return const_iterator(this, 0);
			case JsonType::Array:
				return const_iterator(this, this->pArray->cbegin());
			case JsonType::Object:
				return const_iterator(this, this->pObject->cbegin());
			}
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline typename Json<IntegerTy, FloatingTy, StringTy, BoolTy>::const_iterator Json<IntegerTy, FloatingTy, StringTy, BoolTy>::cbegin(void) const {
			return this->begin();
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline typename Json<IntegerTy, FloatingTy, StringTy, BoolTy>::iterator Json<IntegerTy, FloatingTy, StringTy, BoolTy>::end(void) {
			switch (this->_type) {
			case JsonType::Null:
				return iterator(this, 0);
			case JsonType::Integer:
			case JsonType::Floating:
			case JsonType::String:
			case JsonType::Bool:
				return iterator(this, 1);
			case JsonType::Array:
				return iterator(this, this->pArray->end());
			case JsonType::Object:
				return iterator(this, this->pObject->end());
			}
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline typename Json<IntegerTy, FloatingTy, StringTy, BoolTy>::const_iterator Json<IntegerTy, FloatingTy, StringTy, BoolTy>::end(void) const {
			switch (this->_type) {
			case JsonType::Null:
				return const_iterator(this, 0);
			case JsonType::Integer:
			case JsonType::Floating:
			case JsonType::String:
			case JsonType::Bool:
				return const_iterator(this, 1);
			case JsonType::Array:
				return const_iterator(this, this->pArray->cend());
			case JsonType::Object:
				return const_iterator(this, this->pObject->cend());
			}
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline typename Json<IntegerTy, FloatingTy, StringTy, BoolTy>::const_iterator Json<IntegerTy, FloatingTy, StringTy, BoolTy>::cend(void) const {
			return this->end();
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline typename Json<IntegerTy, FloatingTy, StringTy, BoolTy>::iterator Json<IntegerTy, FloatingTy, StringTy, BoolTy>::find(const StringType& key) {
			if (this->_type != JsonType::Object)
				throw std::out_of_range("This function is valid only if the Json container is an object.");
			return iterator(this, this->pObject->find(key));
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline typename Json<IntegerTy, FloatingTy, StringTy, BoolTy>::const_iterator Json<IntegerTy, FloatingTy, StringTy, BoolTy>::find(const StringType& key) const {
			if (this->_type != JsonType::Object)
				throw std::out_of_range("This function is valid only if the Json container is an object.");
			return const_iterator(this, this->pObject->find(key));
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline void Json<IntegerTy, FloatingTy, StringTy, BoolTy>::_reset(void) {
			switch (this->_type) {
			case JsonType::Null:
				break;
			case JsonType::Integer:
				delete this->pInteger;
				break;
			case JsonType::Floating:
				delete this->pFloating;
				break;
			case JsonType::String:
				delete this->pString;
				break;
			case JsonType::Bool:
				delete this->pBool;
				break;
			case JsonType::Array:
				delete this->pArray;
				break;
			case JsonType::Object:
				delete this->pObject;
				break;
			}
			this->_type = JsonType::Null;
			this->dummy = nullptr;
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline void Json<IntegerTy, FloatingTy, StringTy, BoolTy>::_assign(const Json& json) {
			switch (json._type) {
			case JsonType::Null:
				this->dummy = nullptr;
				break;
			case JsonType::Integer:
				this->pInteger = new IntegerType(*json.pInteger);
				break;
			case JsonType::Floating:
				this->pFloating = new FloatingType(*json.pFloating);
				break;
			case JsonType::String:
				this->pString = new StringType(*json.pString);
				break;
			case JsonType::Bool:
				this->pBool = new BoolType(*json.pBool);
				break;
			case JsonType::Array:
				this->pArray = new ArrayType(*json.pArray);
				break;
			case JsonType::Object:
				this->pObject = new ObjectType(*json.pObject);
				break;
			}
			this->_type = json._type;
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline void Json<IntegerTy, FloatingTy, StringTy, BoolTy>::_assign(Json&& json) {
			this->_type = json._type;
			this->dummy = json.dummy;
			json._type = JsonType::Null;
			json.dummy = nullptr;
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline void Json<IntegerTy, FloatingTy, StringTy, BoolTy>::_create(JsonType type) {
			switch (type) {
			case JsonType::Null:
				this->dummy = nullptr;
				break;
			case JsonType::Integer:
				this->pInteger = new IntegerType();
				break;
			case JsonType::Floating:
				this->pFloating = new FloatingType();
				break;
			case JsonType::String:
				this->pString = new StringType();
			case JsonType::Bool:
				this->pBool = new BoolType();
				break;
			case JsonType::Array:
				this->pArray = new ArrayType();
				break;
			case JsonType::Object:
				this->pObject = new ObjectType();
				break;
			}
			this->_type = type;
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		void Json<IntegerTy, FloatingTy, StringTy, BoolTy>::_print(std::ostream& out, int indent) const {
			switch (this->_type) {
			case JsonType::Null:
				out << StringType(indent, '\t') << "null";
				break;
			case JsonType::Integer:
				out << StringType(indent, '\t') << *this->pInteger;
				break;
			case JsonType::Floating:
				out << StringType(indent, '\t') << *this->pFloating;
				break;
			case JsonType::String:
				out << StringType(indent, '\t') << '\"' << *this->pString << '\"';
				break;
			case JsonType::Bool:
				out << StringType(indent, '\t') << std::boolalpha << *this->pBool;
				break;
			case JsonType::Array:
				out << StringType(indent, '\t') << "[" << std::endl;
				for (auto iter = this->pArray->cbegin(); iter != this->pArray->cend();) {
					iter->_print(out, indent + 1);
					++iter;
					if (iter != this->pArray->cend()) out << ",";
					out << std::endl;
				}
				out << StringType(indent, '\t') << "]";
				break;
			case JsonType::Object:
				out << StringType(indent, '\t') << "{" << std::endl;
				for (auto iter = this->pObject->cbegin(); iter != this->pObject->cend();) {
					out << StringType(indent + 1, '\t') << '\"' << iter->first << '\"' << " : ";
					if (iter->second._type == JsonType::Array || iter->second._type == JsonType::Object) {
						out << std::endl;
						iter->second._print(out, indent + 2);
					}
					else {
						iter->second._print(out, 0);
					}
					++iter;
					if (iter != this->pObject->cend()) out << ",";
					out << std::endl;
				}
				out << StringType(indent, '\t') << "}" << std::endl;
				break;
			}
		}

		#define JJYOU_IO_JSON_ITERATOR_EQUAL_IMPL(IterTy1, IterTy2)																							\
		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>																			\
		inline bool operator==(const IterTy1<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const IterTy2<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2)\
		{																																					\
			return																																			\
				(iter1.pJson == iter2.pJson &&																												\
					(																																		\
						(iter1.pJson == nullptr) ||																											\
						(iter1.pJson->type() == JsonType::Array && iter1.arrayIter == iter2.arrayIter) ||													\
						(iter1.pJson->type() == JsonType::Object && iter1.objectIter == iter2.objectIter) ||												\
						(iter1.pJson->type() != JsonType::Array && iter1.pJson->type() != JsonType::Object && iter1.pos == iter2.pos)						\
						)																																	\
					);																																		\
		}

		#define JJYOU_IO_JSON_ITERATOR_UNEQUAL_IMPL(IterTy1, IterTy2)																						\
		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>																			\
		inline bool operator!=(const IterTy1<IntegerTy, FloatingTy, StringTy, BoolTy>& iter1, const IterTy2<IntegerTy, FloatingTy, StringTy, BoolTy>& iter2)\
		{																																					\
			return !(iter1 == iter2);																														\
		}

		JJYOU_IO_JSON_ITERATOR_EQUAL_IMPL(JsonIterator, JsonIterator);
		JJYOU_IO_JSON_ITERATOR_EQUAL_IMPL(JsonIterator, JsonConstIterator);
		JJYOU_IO_JSON_ITERATOR_EQUAL_IMPL(JsonConstIterator, JsonIterator);
		JJYOU_IO_JSON_ITERATOR_EQUAL_IMPL(JsonConstIterator, JsonConstIterator);

		JJYOU_IO_JSON_ITERATOR_UNEQUAL_IMPL(JsonIterator, JsonIterator);
		JJYOU_IO_JSON_ITERATOR_UNEQUAL_IMPL(JsonIterator, JsonConstIterator);
		JJYOU_IO_JSON_ITERATOR_UNEQUAL_IMPL(JsonConstIterator, JsonIterator);
		JJYOU_IO_JSON_ITERATOR_UNEQUAL_IMPL(JsonConstIterator, JsonConstIterator);

		#undef JJYOU_IO_JSON_ITERATOR_EQUAL_IMPL
		#undef JJYOU_IO_JSON_ITERATOR_UNEQUAL_IMPL

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy> JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>::operator++(int) {
			JsonIterator ret = *this;
			++(*this);
			return ret;
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>::operator++(void) {
			if (this->pJson) {
				if (this->pJson->type() == JsonType::Array)
					++this->arrayIter;
				else if (this->pJson->type() == JsonType::Object)
					++this->objectIter;
				else
					++this->pos;
			}
			return *this;
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy> JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>::operator--(int) {
			JsonIterator ret = *this;
			--(*this);
			return ret;
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>::operator--(void) {
			if (this->pJson) {
				if (this->pJson->type() == JsonType::Array)
					--this->arrayIter;
				else if (this->pJson->type() == JsonType::Object)
					--this->objectIter;
				else
					--this->pos;
			}
			return *this;
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline typename JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>::reference JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>::operator*() const {
			if (this->pJson->type() == JsonType::Array)
				return *this->arrayIter;
			else if (this->pJson->type() == JsonType::Object)
				return this->objectIter->second;
			else
				return *this->pJson;
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline typename JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>::pointer JsonIterator<IntegerTy, FloatingTy, StringTy, BoolTy>::operator->() const {
			if (this->pJson->type() == JsonType::Array)
				return &*this->arrayIter;
			else if (this->pJson->type() == JsonType::Object)
				return &this->objectIter->second;
			else
				return this->pJson;
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy> JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>::operator++(int) {
			JsonConstIterator ret = *this;
			++(*this);
			return ret;
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>::operator++(void) {
			if (this->pJson) {
				if (this->pJson->type() == JsonType::Array)
					++this->arrayIter;
				else if (this->pJson->type() == JsonType::Object)
					++this->objectIter;
				else
					++this->pos;
			}
			return *this;
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy> JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>::operator--(int) {
			JsonConstIterator ret = *this;
			--(*this);
			return ret;
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>& JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>::operator--(void) {
			if (this->pJson) {
				if (this->pJson->type() == JsonType::Array)
					--this->arrayIter;
				else if (this->pJson->type() == JsonType::Object)
					--this->objectIter;
				else
					--this->pos;
			}
			return *this;
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline typename JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>::reference JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>::operator*() const {
			if (this->pJson->type() == JsonType::Array)
				return *this->arrayIter;
			else if (this->pJson->type() == JsonType::Object)
				return this->objectIter->second;
			else
				return *this->pJson;
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline typename JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>::pointer JsonConstIterator<IntegerTy, FloatingTy, StringTy, BoolTy>::operator->() const {
			if (this->pJson->type() == JsonType::Array)
				return &*this->arrayIter;
			else if (this->pJson->type() == JsonType::Object)
				return &this->objectIter->second;
			else
				return this->pJson;
		}

		class JsonToken {
		private:
			enum class Type {
				Unexpected = -1,
				End = 0,
				Null,
				Integer,
				Floating,
				String,
				BoolT,
				BoolF,
				Comma,
				Colon,
				Lbracket,
				Rbracket,
				Lbrace,
				Rbrace
			};
			Type type = Type::End;
			std::string_view data{};
			JsonToken(void) = default;
			JsonToken(JsonToken::Type type) : type(type), data() {}
			JsonToken(JsonToken::Type type, const char* begin, std::size_t length) : type(type), data(begin, length) {}
			friend class JsonLexer;
			friend std::string to_string(JsonToken::Type type);
			friend std::ostream& operator<<(std::ostream& out, JsonToken::Type type);
			template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy> friend class Json;
		};

		inline std::string to_string(JsonToken::Type type) {
			switch (type) {
			case JsonToken::Type::Unexpected: return "Unexpected";
			case JsonToken::Type::End: return "End";
			case JsonToken::Type::Null: return "Null";
			case JsonToken::Type::Integer: return "Integer";
			case JsonToken::Type::Floating: return "Floating";
			case JsonToken::Type::String: return "String";
			case JsonToken::Type::BoolT: return "BoolT";
			case JsonToken::Type::BoolF: return "BoolF";
			case JsonToken::Type::Comma: return "Comma";
			case JsonToken::Type::Colon: return "Colon";
			case JsonToken::Type::Lbracket: return "Lbracket";
			case JsonToken::Type::Rbracket: return "Rbracket";
			case JsonToken::Type::Lbrace: return "Lbrace";
			case JsonToken::Type::Rbrace: return "Rbrace";
			}
		}

		inline std::ostream& operator<<(std::ostream& out, JsonToken::Type type) {
			return (out << to_string(type));
		}

		//https://www.json.org/json-en.html
		class JsonLexer {
		private:
			using Type = JsonToken::Type;
			static bool isWhitespace(char c) {
				switch (c) {
				case ' ':
				case '\t':
				case '\r':
				case '\n':
					return true;
				default:
					return false;
				}
			}
			static bool isDigit(char c) {
				switch (c) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					return true;
				default:
					return false;
				}
			}
			JsonLexer(const std::string& buffer) : buffer(buffer), pos(0), ungets{} {}
			char curr(void) const { return this->buffer[this->pos]; }
			char get(void) { return this->buffer[this->pos++]; }
			void ungetToken(JsonToken token) { this->ungets.push(token); }
			JsonToken forward(Type type, std::size_t length) { JsonToken token(type, &this->buffer[this->pos], length); this->pos += length; return token; }
			JsonToken number(void) {
				Type type = Type::Integer;
				std::size_t start = this->pos;
				while (this->pos < this->buffer.length() && (this->isDigit(this->curr()) || this->curr() == '+' || this->curr() == '-' || this->curr() == '.' || this->curr() == 'e')) {
					if (this->curr() == '.')
						type = Type::Floating;
					this->get();
				}
				return JsonToken(type, &this->buffer[start], this->pos - start);
			}
			JsonToken string(void) {
				std::size_t start = ++this->pos;
				bool findEnd = false;
				while (this->pos < this->buffer.length()) {
					if (this->curr() == '\"') {
						this->get();
						findEnd = true;
						break;
					}
					else if (this->curr() == '\\') {
						this->get();
						if (this->pos == this->buffer.length())
							break;
						this->get();
					}
					else {
						this->get();
					}
				}
				if (findEnd)
					return JsonToken(Type::String, &this->buffer[start], this->pos - start - 1);
				else
					return JsonToken(Type::Unexpected, &this->buffer[start], this->pos - start);
			}
			JsonToken getToken(void) {
				if (!this->ungets.empty()) {
					JsonToken res = this->ungets.top();
					this->ungets.pop();
					return res;
				}
				while (this->pos < this->buffer.length() && this->isWhitespace(this->curr())) this->get();
				if (this->pos == this->buffer.length())
					return JsonToken(Type::End);
				switch (this->curr()) {
				case '+':
				case '-':
				case '.':
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					return this->number();
				case '\"':
					return this->string();
				case 't':
					if (this->buffer.substr(this->pos, 4) == "true")
						return this->forward(Type::BoolT, 4);
					else
						return this->forward(Type::Unexpected, 1);
				case 'f':
					if (this->buffer.substr(this->pos, 5) == "false")
						return this->forward(Type::BoolF, 5);
					else
						return this->forward(Type::Unexpected, 1);
				case 'n':
					if (this->buffer.substr(this->pos, 4) == "null")
						return this->forward(Type::Null, 4);
					else
						return this->forward(Type::Unexpected, 1);
				case ',':
					return this->forward(Type::Comma, 1);
				case ':':
					return this->forward(Type::Colon, 1);
				case '[':
					return this->forward(Type::Lbracket, 1);
				case ']':
					return this->forward(Type::Rbracket, 1);
				case '{':
					return this->forward(Type::Lbrace, 1);
				case '}':
					return this->forward(Type::Rbrace, 1);
				default:
					return this->forward(Type::Unexpected, 1);
				}
			}
			std::string buffer;
			std::size_t pos;
			std::stack<JsonToken> ungets;
			template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy> friend class Json;
		};

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		template <class T>
		inline Json<IntegerTy, FloatingTy, StringTy, BoolTy> Json<IntegerTy, FloatingTy, StringTy, BoolTy>::parse(T* string) {
			JsonLexer lexer(string);
			return Json::_parse(lexer);
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline Json<IntegerTy, FloatingTy, StringTy, BoolTy> Json<IntegerTy, FloatingTy, StringTy, BoolTy>::parse(const StringType& string) {
			JsonLexer lexer(string);
			return Json::_parse(lexer);
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		inline Json<IntegerTy, FloatingTy, StringTy, BoolTy> Json<IntegerTy, FloatingTy, StringTy, BoolTy>::parse(const std::filesystem::path& path) {
			std::ifstream fin(path, std::ios::in);
			std::basic_stringstream<typename StringType::value_type> buffer;
			buffer << fin.rdbuf();
			return Json::parse(buffer.str());
		}

		template <class IntegerTy, class FloatingTy, class StringTy, class BoolTy>
		Json<IntegerTy, FloatingTy, StringTy, BoolTy> Json<IntegerTy, FloatingTy, StringTy, BoolTy>::_parse(JsonLexer& lexer) {
			JsonToken token = lexer.getToken();
			switch (token.type) {
			case JsonToken::Type::Unexpected:
			{
				throw std::runtime_error("Unexpected character \"" + std::string(token.data) + "\".");
			}
			case JsonToken::Type::End:
			{
				throw std::runtime_error("Unexpected EOF.");
			}
			case JsonToken::Type::Null:/* Null */
			{
				Json json{};
				return json;
			}
			case JsonToken::Type::Integer:/* Integer */
			{
				Json json(::jjyou::utils::string2Number<IntegerType>(StringType(token.data)));
				return json;
			}
			case JsonToken::Type::Floating:/* Floating */
			{
				Json json(::jjyou::utils::string2Number<FloatingType>(StringType(token.data)));
				return json;
			}
			case JsonToken::Type::String:/* String */
			{
				// Currently, don't care about escaping character
				Json json(StringType(token.data));
				return json;
			}
			case JsonToken::Type::BoolT:/* Bool true */
			{
				Json json(true);
				return json;
			}
			case JsonToken::Type::BoolF:/* Bool false */
			{
				Json json(false);
				return json;
			}
			case JsonToken::Type::Lbracket: /* Array */
			{
				Json json(Json::ArrayType{});
				while ((token = lexer.getToken()).type != JsonToken::Type::Rbracket) {
					if (token.type == JsonToken::Type::End)
						throw std::runtime_error("Unexpected EOF.");
					if (token.type == JsonToken::Type::Unexpected)
						throw std::runtime_error("Unexpected character \"" + std::string(token.data) + "\".");
					if (json.pArray->empty()) // First element in the array
						lexer.ungetToken(token);
					else if (token.type != JsonToken::Type::Comma)
						throw std::runtime_error("Objects in array must be separated by commas.");
					Json element = Json::_parse(lexer);
					json.pArray->push_back(element);
				}
				return json;
			}
			case JsonToken::Type::Lbrace: /* Object */
			{
				Json json(Json::ObjectType{});
				while ((token = lexer.getToken()).type != JsonToken::Type::Rbrace) {
					if (token.type == JsonToken::Type::End)
						throw std::runtime_error("Unexpected EOF.");
					if (token.type == JsonToken::Type::Unexpected)
						throw std::runtime_error("Unexpected character \"" + std::string(token.data) + "\".");
					if (json.pObject->empty()) // First key-value pair in the object
						lexer.ungetToken(token);
					else if (token.type != JsonToken::Type::Comma)
						throw std::runtime_error("Key-value pairs in objects must be separated by commas.");
					token = lexer.getToken(); // Key
					if (token.type == JsonToken::Type::End)
						throw std::runtime_error("Unexpected EOF.");
					if (token.type == JsonToken::Type::Unexpected)
						throw std::runtime_error("Unexpected character \"" + std::string(token.data) + "\".");
					if (token.type != JsonToken::Type::String)
						throw std::runtime_error("Object's key must be a string.");
					StringType key = StringType(token.data);
					token = lexer.getToken();
					if (token.type == JsonToken::Type::End)
						throw std::runtime_error("Unexpected EOF.");
					if (token.type == JsonToken::Type::Unexpected)
						throw std::runtime_error("Unexpected character \"" + std::string(token.data) + "\".");
					if (token.type != JsonToken::Type::Colon)
						throw std::runtime_error("Key and value must be separated by a colon.");
					json.pObject->emplace(key, Json::_parse(lexer));
				}
				return json;
			}
			}
		}

	}

}

/// @endcond

#endif /* jjyou_io_Json_hpp */