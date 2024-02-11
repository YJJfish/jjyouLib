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
#include <vector>
#include <map>
#include <exception>
#include "GenericLexer.hpp"

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

		/** @brief	Helper function to write JsonType to output stream.
		  */
		inline std::ostream& operator<<(std::ostream& out, JsonType type) {
			return (out << to_string(type));
		}

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
			using IntegerType = IntegerTy;
			using FloatingType = FloatingTy;
			using StringType = StringTy;
			using BoolType = BoolTy;
			using ArrayType = std::vector<Json>;
			using ObjectType = std::map<StringTy, Json>;
			//@}

		public:

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

			/** @brief	Construct a json container from a boolean.
			  */
			Json(BoolType boolean) : _type(JsonType::Bool), pBool(new BoolType(boolean)) {}

			/** @brief	Construct a json container from an array.
			  */
			Json(const ArrayType& array) : _type(JsonType::Array), pArray(new ArrayType(array)) {}

			/** @brief	Construct a json container from an array.
			  */
			Json(ArrayType&& array) : _type(JsonType::Array), pArray(new ArrayType(std::move(array))) {}

			/** @brief	Construct a json container from an object.
			  */
			Json(const ObjectType& object) : _type(JsonType::Object), pObject(new ObjectType(object)) {}

			/** @brief	Construct a json container from an object.
			  */
			Json(ObjectType&& object) : _type(JsonType::Object), pObject(new ObjectType(std::move(object))) {}
			
			/** @brief	Construct a json container of the specified type with default value.
			  */
			Json(JsonType type) : _type(JsonType::Null), dummy(nullptr) {
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
			reference operator[](const StringType& pos) {
				return (*this->pObject)[pos];
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
			reference at(const StringType& pos) {
				if (this->_type != JsonType::Object)
					throw std::out_of_range("This function is valid only if the Json container is an object.");
				return this->pObject->at(pos);
			}

			/** @brief	Get the const reference to the value that is mapped to the given key, with bounds checking.
			  *
			  * This function is valid only if the Json's type is `JsonType::Object`.
			  * Otherwise, an exception of type std::out_of_range is thrown.
			  * @return	The const reference to the value that is mapped to the given key.
			  */
			const_reference at(const StringType& pos) const {
				if (this->_type != JsonType::Object)
					throw std::out_of_range("This function is valid only if the Json container is an object.");
				return this->pObject->at(pos);
			}

			/** @brief	Get the size of the json container.
			  *
			  * For null, the size is always 0.
			  * For integer/floating/string/bool, the size is always 1.
			  * For array, the size is the array length.
			  * For object, the size is the number of key&value pairs.
			  * @return	The size of the json container.
			  */
			size_t size(void) const {
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

			iterator begin(void) {
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

			const_iterator begin(void) const {
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

			const_iterator cbegin(void) const {
				return this->begin();
			}

			iterator end(void) {
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

			const_iterator end(void) const {
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

			const_iterator cend(void) const {
				return this->end();
			}

		private:

			void _reset(void) {
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

			void _assign(const Json& json) {
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

			void _assign(Json&& json) {
				this->_type = json._type;
				this->dummy = json.dummy;
				json._type = JsonType::Null;
				json.dummy = nullptr;
			}

			void _create(JsonType type) {
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
			JsonIterator operator++(int) {
				JsonIterator ret = *this;
				++(*this);
				return ret;
			}

			/** @brief	Increment the iterator.
			  * @return	Reference of the iterator after being incremented.
			  */
			JsonIterator& operator++(void) {
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

			/** @brief	Decrement the iterator.
			  * @return	Copy of the iterator before being deremented.
			  */
			JsonIterator operator--(int) {
				JsonIterator ret = *this;
				--(*this);
				return ret;
			}

			/** @brief	Decrement the iterator.
			  * @return	Reference of the iterator after being decremented.
			  */
			JsonIterator& operator--(void) {
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

			/** @brief	Fetch the current element.
			  * @return	Reference of the current element.
			  */
			reference operator*() const {
				if (this->pJson->type() == JsonType::Array)
					return *this->arrayIter;
				else if (this->pJson->type() == JsonType::Object)
					return this->objectIter->second;
				else
					return *this->pJson;
			}

			/** @brief	Fetch the current element.
			  * @return	Pointer to the current element.
			  */
			pointer operator->() const {
				if (this->pJson->type() == JsonType::Array)
					return &*this->arrayIter;
				else if(this->pJson->type() == JsonType::Object)
					return &this->objectIter->second;
				else
					return this->pJson;
			}

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
				return (*this);
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
			JsonConstIterator operator++(int) {
				JsonConstIterator ret = *this;
				++(*this);
				return ret;
			}

			/** @brief	Increment the iterator.
			  * @return	Reference of the iterator after being incremented.
			  */
			JsonConstIterator& operator++(void) {
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

			/** @brief	Decrement the iterator.
			  * @return	Copy of the iterator before being deremented.
			  */
			JsonConstIterator operator--(int) {
				JsonConstIterator ret = *this;
				--(*this);
				return ret;
			}

			/** @brief	Decrement the iterator.
			  * @return	Reference of the iterator after being decremented.
			  */
			JsonConstIterator& operator--(void) {
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

			/** @brief	Fetch the current element.
			  * @return	Reference of the current element.
			  */
			reference operator*() const {
				if (this->pJson->type() == JsonType::Array)
					return *this->arrayIter;
				else if (this->pJson->type() == JsonType::Object)
					return this->objectIter->second;
				else
					return *this->pJson;
			}

			/** @brief	Fetch the current element.
			  * @return	Pointer to the current element.
			  */
			pointer operator->() const {
				if (this->pJson->type() == JsonType::Array)
					return &*this->arrayIter;
				else if (this->pJson->type() == JsonType::Object)
					return &this->objectIter->second;
				else
					return this->pJson;
			}

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
				return (*this);
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

	}

}

/// @endcond

#endif /* jjyou_io_Json_hpp */