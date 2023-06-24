/***********************************************************************
 * @file	HalfedgeMesh.hpp
 * @author	jjyou
 * @date	2023-6-19
 * @brief	This file implements HalfedgeMesh class.
***********************************************************************/
#ifndef jjyou_geo_HalfedgeMesh_hpp
#define jjyou_geo_HalfedgeMesh_hpp

namespace jjyou {
	namespace geo {

		/***********************************************************************
		 * @class HalfedgeMesh
		 * @brief Halfedge data structure of a mesh.
		 *
		 * This class stores meshes in halfedge data structure.
		 * 
		 * @sa			https://en.wikipedia.org/wiki/Doubly_connected_edge_list
		 * @sa			jjyou::geo::PolygonSoupMesh
		 ***********************************************************************/
		class HalfedgeMesh {

		public:

			/*============================================================
			 *                    Forward declarations
			 *============================================================*/
			template <int Type>
			class IndexBase;
			class VertexIndex;
			class HalfedgeIndex;
			class FaceIndex;
			class EdgeIndex;
			class VertexRange; class VertexIterator;
			class HalfedgeRange; class HalfedgeIterator;
			class FaceRange; class FaceIterator;
			class EdgeRange; class EdgeIterator;
			class VertexVertexRange; class VertexVertexIterator;
			class VertexHalfedgeRange; class VertexHalfedgeIterator;
			class VertexFaceRange; class VertexFaceIterator;
			class VertexEdgeRange; class VertexEdgeIterator;
			class FaceVertexRange; class FaceVertexIterator;
			class FaceHalfedgeRange; class FaceHalfedgeIterator;
			class FaceFaceRange; class FaceFaceIterator;
			class FaceEdgeRange; class FaceEdgeIterator;
			/*============================================================
			 *                 End of forward declarations
			 *============================================================*/

			/** @defgroup	IndexClasses
			  * @brief		Index classes for vertex, halfedge, face, and edge index.
			  *
			  * @{
			  */

			/***********************************************************************
			 * @class IndexBase
			 * @brief Base index class for vertex, halfedge, face, and edge index.
			 *
			 * This class is simply a wrapper of std::size_t. The reason for not using
			 * std::size_t as the index directly is to avoid mixed use of different element
			 * indices. With this wrapper class, users need to explicitly call conversion
			 * functions if they want conversions between an index and an built-in integer.
			 * @param Type	Index type.
			 ***********************************************************************/
			template <int Type>
			class IndexBase {
			public:

				/** @brief	Get the underlying index.
				  * @return	The underlying index.
				  */
				std::size_t idx(void) const {
					return this->_idx;
				}

				/** @brief	Check whether the index is valid.
				  * @return	`true` if the index is valid.
				  */
				bool valid(void) const {
					return this->_idx != std::numeric_limits<std::size_t>::max();
				}

				/** @brief	Reset to invalid.
				  */
				void reset(void) {
					this->_idx = std::numeric_limits<std::size_t>::max();
				}

			protected:

				/** @brief	Construct an index from an integer.
				  */
				explicit IndexBase(std::size_t idx = std::numeric_limits<std::size_t>::max()) : _idx(idx) {}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs == rhs.
				  */
				bool operator==(const IndexBase& other) const {
					return this->_idx == other._idx;
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs != rhs.
				  */
				bool operator!=(const IndexBase& other) const {
					return this->_idx != other._idx;
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs >= rhs.
				  */
				bool operator>=(const IndexBase& other) const {
					return this->_idx >= other._idx;
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs > rhs.
				  */
				bool operator>(const IndexBase& other) const {
					return this->_idx > other._idx;
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs <= rhs.
				  */
				bool operator<=(const IndexBase& other) const {
					return this->_idx <= other._idx;
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs < rhs.
				  */
				bool operator<(const IndexBase& other) const {
					return this->_idx < other._idx;
				}

				/** @brief	Increment the index.
				  * @return	Reference of current index after incrementing.
				  */
				IndexBase& operator++(void) {
					this->_idx += 1;
					return *this;
				}

				/** @brief	Increment the index.
				  * @return	Copy of current index before incrementing.
				  */
				IndexBase operator++(int) {
					IndexBase ret = *this;
					this->_idx += 1;
					return ret;
				}

				/** @brief	Decrement the index.
				  * @return	Reference of current index after decrementing.
				  */
				IndexBase& operator--(void) {
					this->_idx -= 1;
					return *this;
				}

				/** @brief	Decrement the index.
				  * @return	Copy of current index before decrementing.
				  */
				IndexBase operator--(int) {
					IndexBase ret = *this;
					this->_idx -= 1;
					return ret;
				}

				/** @brief	Add an integer to the index.
				  * @return	Reference of current index.
				  */
				IndexBase& operator+=(std::ptrdiff_t n) {
					this->_idx += n;
					return *this;
				}

				/** @brief	Perform addition with an index and an integer.
				  * @return	A new index after addition.
				  */
				IndexBase operator+(std::ptrdiff_t n) const {
					IndexBase ret = *this;
					ret._idx += n;
					return ret;
				}

				/** @brief	Subtract an integer from the index.
				  * @return	Reference of current index.
				  */
				IndexBase& operator-=(std::ptrdiff_t n) {
					this->_idx -= n;
					return *this;
				}

				/** @brief	Perform subtraction with an index and an integer.
				  * @return	A new index after subtraction.
				  */
				IndexBase operator-(std::ptrdiff_t n) const {
					IndexBase ret = *this;
					ret._idx -= n;
					return ret;
				}

			private:
				std::size_t _idx;
			};

			/***********************************************************************
			 * @class VertexIndex
			 * @brief Vertex index class.
			 ***********************************************************************/
			class VertexIndex : public IndexBase<0> {

			public:

				/** @brief	Construct an vertex index from an integer.
				  */
				explicit VertexIndex(std::size_t idx = std::numeric_limits<std::size_t>::max()) : IndexBase<0>(idx) {}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs == rhs.
				  */
				bool operator==(const VertexIndex& other) const {
					return this->IndexBase<0>::operator==(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs != rhs.
				  */
				bool operator!=(const VertexIndex& other) const {
					return this->IndexBase<0>::operator!=(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs >= rhs.
				  */
				bool operator>=(const VertexIndex& other) const {
					return this->IndexBase<0>::operator>=(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs > rhs.
				  */
				bool operator>(const VertexIndex& other) const {
					return this->IndexBase<0>::operator>(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs <= rhs.
				  */
				bool operator<=(const VertexIndex& other) const {
					return this->IndexBase<0>::operator<=(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs < rhs.
				  */
				bool operator<(const VertexIndex& other) const {
					return this->IndexBase<0>::operator<(other);
				}

				/** @brief	Increment the index.
				  * @return	Reference of current index after incrementing.
				  */
				VertexIndex& operator++(void) {
					this->IndexBase<0>::operator++();
					return *this;
				}

				/** @brief	Increment the index.
				  * @return	Copy of current index before incrementing.
				  */
				VertexIndex operator++(int) {
					VertexIndex ret = *this;
					this->IndexBase<0>::operator++(0);
					return ret;
				}

				/** @brief	Decrement the index.
				  * @return	Reference of current index after decrementing.
				  */
				VertexIndex& operator--(void) {
					this->IndexBase<0>::operator--();
					return *this;
				}

				/** @brief	Decrement the index.
				  * @return	Copy of current index before decrementing.
				  */
				VertexIndex operator--(int) {
					VertexIndex ret = *this;
					this->IndexBase<0>::operator--(0);
					return ret;
				}

				/** @brief	Add an integer to the index.
				  * @return	Reference of current index.
				  */
				VertexIndex& operator+=(std::ptrdiff_t n) {
					this->IndexBase<0>::operator+=(n);
					return *this;
				}

				/** @brief	Perform addition with an index and an integer.
				  * @return	A new index after addition.
				  */
				VertexIndex operator+(std::ptrdiff_t n) const {
					VertexIndex ret = *this;
					ret.operator+=(n);
					return ret;
				}

				/** @brief	Subtract an integer from the index.
				  * @return	Reference of current index.
				  */
				VertexIndex& operator-=(std::ptrdiff_t n) {
					this->IndexBase<0>::operator-=(n);
					return *this;
				}

				/** @brief	Perform subtraction with an index and an integer.
				  * @return	A new index after subtraction.
				  */
				VertexIndex operator-(std::ptrdiff_t n) const {
					VertexIndex ret = *this;
					ret.operator-=(n);
					return ret;
				}
				
				friend std::ostream& operator<<(std::ostream& out, const VertexIndex& index) {
					out << "v" << index.idx();
					return out;
				}

			};

			/***********************************************************************
			 * @class HalfedgeIndex
			 * @brief Halfedge index class.
			 ***********************************************************************/
			class HalfedgeIndex : public IndexBase<1> {

			public:

				/** @brief	Construct an vertex index from an integer.
				  */
				explicit HalfedgeIndex(std::size_t idx = std::numeric_limits<std::size_t>::max()) : IndexBase<1>(idx) {}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs == rhs.
				  */
				bool operator==(const HalfedgeIndex& other) const {
					return this->IndexBase<1>::operator==(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs != rhs.
				  */
				bool operator!=(const HalfedgeIndex& other) const {
					return this->IndexBase<1>::operator!=(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs >= rhs.
				  */
				bool operator>=(const HalfedgeIndex& other) const {
					return this->IndexBase<1>::operator>=(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs > rhs.
				  */
				bool operator>(const HalfedgeIndex& other) const {
					return this->IndexBase<1>::operator>(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs <= rhs.
				  */
				bool operator<=(const HalfedgeIndex& other) const {
					return this->IndexBase<1>::operator<=(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs < rhs.
				  */
				bool operator<(const HalfedgeIndex& other) const {
					return this->IndexBase<1>::operator<(other);
				}

				/** @brief	Increment the index.
				  * @return	Reference of current index after incrementing.
				  */
				HalfedgeIndex& operator++(void) {
					this->IndexBase<1>::operator++();
					return *this;
				}

				/** @brief	Increment the index.
				  * @return	Copy of current index before incrementing.
				  */
				HalfedgeIndex operator++(int) {
					HalfedgeIndex ret = *this;
					this->IndexBase<1>::operator++(0);
					return ret;
				}

				/** @brief	Decrement the index.
				  * @return	Reference of current index after decrementing.
				  */
				HalfedgeIndex& operator--(void) {
					this->IndexBase<1>::operator--();
					return *this;
				}

				/** @brief	Decrement the index.
				  * @return	Copy of current index before decrementing.
				  */
				HalfedgeIndex operator--(int) {
					HalfedgeIndex ret = *this;
					this->IndexBase<1>::operator--(0);
					return ret;
				}

				/** @brief	Add an integer to the index.
				  * @return	Reference of current index.
				  */
				HalfedgeIndex& operator+=(std::ptrdiff_t n) {
					this->IndexBase<1>::operator+=(n);
					return *this;
				}

				/** @brief	Perform addition with an index and an integer.
				  * @return	A new index after addition.
				  */
				HalfedgeIndex operator+(std::ptrdiff_t n) const {
					HalfedgeIndex ret = *this;
					ret.operator+=(n);
					return ret;
				}

				/** @brief	Subtract an integer from the index.
				  * @return	Reference of current index.
				  */
				HalfedgeIndex& operator-=(std::ptrdiff_t n) {
					this->IndexBase<1>::operator-=(n);
					return *this;
				}

				/** @brief	Perform subtraction with an index and an integer.
				  * @return	A new index after subtraction.
				  */
				HalfedgeIndex operator-(std::ptrdiff_t n) const {
					HalfedgeIndex ret = *this;
					ret.operator-=(n);
					return ret;
				}

				friend std::ostream& operator<<(std::ostream& out, const HalfedgeIndex& index) {
					out << "h" << index.idx();
					return out;
				}

			};

			/***********************************************************************
			 * @class FaceIndex
			 * @brief Face index class.
			 ***********************************************************************/
			class FaceIndex : public IndexBase<2> {

			public:

				/** @brief	Construct an vertex index from an integer.
				  */
				explicit FaceIndex(std::size_t idx = std::numeric_limits<std::size_t>::max()) : IndexBase<2>(idx) {}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs == rhs.
				  */
				bool operator==(const FaceIndex& other) const {
					return this->IndexBase<2>::operator==(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs != rhs.
				  */
				bool operator!=(const FaceIndex& other) const {
					return this->IndexBase<2>::operator!=(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs >= rhs.
				  */
				bool operator>=(const FaceIndex& other) const {
					return this->IndexBase<2>::operator>=(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs > rhs.
				  */
				bool operator>(const FaceIndex& other) const {
					return this->IndexBase<2>::operator>(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs <= rhs.
				  */
				bool operator<=(const FaceIndex& other) const {
					return this->IndexBase<2>::operator<=(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs < rhs.
				  */
				bool operator<(const FaceIndex& other) const {
					return this->IndexBase<2>::operator<(other);
				}

				/** @brief	Increment the index.
				  * @return	Reference of current index after incrementing.
				  */
				FaceIndex& operator++(void) {
					this->IndexBase<2>::operator++();
					return *this;
				}

				/** @brief	Increment the index.
				  * @return	Copy of current index before incrementing.
				  */
				FaceIndex operator++(int) {
					FaceIndex ret = *this;
					this->IndexBase<2>::operator++(0);
					return ret;
				}

				/** @brief	Decrement the index.
				  * @return	Reference of current index after decrementing.
				  */
				FaceIndex& operator--(void) {
					this->IndexBase<2>::operator--();
					return *this;
				}

				/** @brief	Decrement the index.
				  * @return	Copy of current index before decrementing.
				  */
				FaceIndex operator--(int) {
					FaceIndex ret = *this;
					this->IndexBase<2>::operator--(0);
					return ret;
				}

				/** @brief	Add an integer to the index.
				  * @return	Reference of current index.
				  */
				FaceIndex& operator+=(std::ptrdiff_t n) {
					this->IndexBase<2>::operator+=(n);
					return *this;
				}

				/** @brief	Perform addition with an index and an integer.
				  * @return	A new index after addition.
				  */
				FaceIndex operator+(std::ptrdiff_t n) const {
					FaceIndex ret = *this;
					ret.operator+=(n);
					return ret;
				}

				/** @brief	Subtract an integer from the index.
				  * @return	Reference of current index.
				  */
				FaceIndex& operator-=(std::ptrdiff_t n) {
					this->IndexBase<2>::operator-=(n);
					return *this;
				}

				/** @brief	Perform subtraction with an index and an integer.
				  * @return	A new index after subtraction.
				  */
				FaceIndex operator-(std::ptrdiff_t n) const {
					FaceIndex ret = *this;
					ret.operator-=(n);
					return ret;
				}

				friend std::ostream& operator<<(std::ostream& out, const FaceIndex& index) {
					out << "f" << index.idx();
					return out;
				}

			};

			/***********************************************************************
			 * @class EdgeIndex
			 * @brief Edge index class.
			 ***********************************************************************/
			class EdgeIndex : public IndexBase<3> {

			public:

				/** @brief	Construct an vertex index from an integer.
				  */
				explicit EdgeIndex(std::size_t idx = std::numeric_limits<std::size_t>::max()) : IndexBase<3>(idx) {}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs == rhs.
				  */
				bool operator==(const EdgeIndex& other) const {
					return this->IndexBase<3>::operator==(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs != rhs.
				  */
				bool operator!=(const EdgeIndex& other) const {
					return this->IndexBase<3>::operator!=(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs >= rhs.
				  */
				bool operator>=(const EdgeIndex& other) const {
					return this->IndexBase<3>::operator>=(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs > rhs.
				  */
				bool operator>(const EdgeIndex& other) const {
					return this->IndexBase<3>::operator>(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs <= rhs.
				  */
				bool operator<=(const EdgeIndex& other) const {
					return this->IndexBase<3>::operator<=(other);
				}

				/** @brief	Compare two indices.
				  * @return	`true` if lhs < rhs.
				  */
				bool operator<(const EdgeIndex& other) const {
					return this->IndexBase<3>::operator<(other);
				}

				/** @brief	Increment the index.
				  * @return	Reference of current index after incrementing.
				  */
				EdgeIndex& operator++(void) {
					this->IndexBase<3>::operator++();
					return *this;
				}

				/** @brief	Increment the index.
				  * @return	Copy of current index before incrementing.
				  */
				EdgeIndex operator++(int) {
					EdgeIndex ret = *this;
					this->IndexBase<3>::operator++(0);
					return ret;
				}

				/** @brief	Decrement the index.
				  * @return	Reference of current index after decrementing.
				  */
				EdgeIndex& operator--(void) {
					this->IndexBase<3>::operator--();
					return *this;
				}

				/** @brief	Decrement the index.
				  * @return	Copy of current index before decrementing.
				  */
				EdgeIndex operator--(int) {
					EdgeIndex ret = *this;
					this->IndexBase<3>::operator--(0);
					return ret;
				}

				/** @brief	Add an integer to the index.
				  * @return	Reference of current index.
				  */
				EdgeIndex& operator+=(std::ptrdiff_t n) {
					this->IndexBase<3>::operator+=(n);
					return *this;
				}

				/** @brief	Perform addition with an index and an integer.
				  * @return	A new index after addition.
				  */
				EdgeIndex operator+(std::ptrdiff_t n) const {
					EdgeIndex ret = *this;
					ret.operator+=(n);
					return ret;
				}

				/** @brief	Subtract an integer from the index.
				  * @return	Reference of current index.
				  */
				EdgeIndex& operator-=(std::ptrdiff_t n) {
					this->IndexBase<3>::operator-=(n);
					return *this;
				}

				/** @brief	Perform subtraction with an index and an integer.
				  * @return	A new index after subtraction.
				  */
				EdgeIndex operator-(std::ptrdiff_t n) const {
					EdgeIndex ret = *this;
					ret.operator-=(n);
					return ret;
				}

				friend std::ostream& operator<<(std::ostream& out, const EdgeIndex& index) {
					out << "e" << index.idx();
					return out;
				}

			};

			/** @}
			  */
			
			/** @defgroup	RangeClasses
			  * @brief		Range (and iterator) classes for vertices, halfedges, faces, edges,
			  *				vertex-vertices, vertex-halfedges, vertex-faces, vertex-edges,
			  *				face-vertices, face-halfedges, face-faces, face-edges.
			  *
			  * @{
			  */

			/***********************************************************************
			 * @class	VertexIterator
			 * @brief	Iterator class for VertexRange.
			 ***********************************************************************/
			class VertexIterator {

			public:

				using Self = VertexIterator;
				using DereferenceType = VertexIndex;

				/** @brief	Default constructor.
				  */
				VertexIterator(void) : pMesh(nullptr), pos() {}

				/** @brief	Check whether this iterator is valid or not.
				  * @return	`true` if this iterator is valid.
				  */
				bool valid(void) const {
					return (this->pMesh && this->pos.idx() < this->pMesh->numVertices());
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType& operator*() const {
					return this->pos;
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType* operator->() const {
					return &this->pos;
				}

				/** @brief	Increment the iterator.
				  * @return	Reference of the iterator after incrementing.
				  */
				Self& operator++(void) {
					this->pos++;
					return *this;
				}

				/** @brief	Increment the iterator.
				  * @return	Copy of the iterator before incrementing.
				  */
				Self operator++(int) {
					Self ret = *this;
					this->pos++;
					return ret;
				}

				/** @brief	Decrement the iterator.
				  * @return	Reference of the iterator after decrementing.
				  */
				Self& operator--(void) {
					this->pos--;
					return *this;
				}

				/** @brief	Decrement the iterator.
				  * @return	Copy of the iterator before decrementing.
				  */
				Self operator--(int) {
					Self ret = *this;
					this->pos--;
					return ret;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs == rhs.
				  */
				bool operator==(const Self& other) const {
					return this->pos == other.pos;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs != rhs.
				  */
				bool operator!=(const Self& other) const {
					return this->pos != other.pos;
				}

			private:
				VertexIterator(
					const HalfedgeMesh* pMesh,
					std::size_t pos
				) : pMesh(pMesh), pos(pos) {}
				const HalfedgeMesh* pMesh;
				DereferenceType pos;
				friend class VertexRange;
			};

			/***********************************************************************
			 * @class	VertexRange
			 * @brief	Vertex range class for iterating all vertices of a mesh.
			 ***********************************************************************/
			class VertexRange {

			public:

				using IteratorType = VertexIterator;

				/** @brief	Get the number of vertices.
				  * @return	The number of vertices.
				  */
				size_t size(void) const {
					return this->mesh.numVertices();
				}

				/** @brief	Get the iterator pointing to the first element.
				  * @return	An iterator pointing to the first element.
				  */
				IteratorType begin(void) const {
					return IteratorType(&this->mesh, 0);
				}

				/** @brief	Get the iterator referring to the past-the-end element.
				  * @return	An iterator referring to the past-the-end element.
				  */
				IteratorType end(void) const {
					return IteratorType(&this->mesh, this->mesh.numVertices());
				}

			private:
				const HalfedgeMesh& mesh;
				VertexRange(const HalfedgeMesh& mesh) : mesh(mesh) {}
				friend class HalfedgeMesh;
			};

			/***********************************************************************
			 * @class	HalfedgeIterator
			 * @brief	Iterator class for HalfedgeRange.
			 ***********************************************************************/
			class HalfedgeIterator {

			public:

				using Self = HalfedgeIterator;
				using DereferenceType = HalfedgeIndex;

				/** @brief	Default constructor.
				  */
				HalfedgeIterator(void) : pMesh(nullptr), pos() {}

				/** @brief	Check whether this iterator is valid or not.
				  * @return	`true` if this iterator is valid.
				  */
				bool valid(void) const {
					return (this->pMesh && this->pos.idx() < this->pMesh->numHalfedges());
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType& operator*() const {
					return this->pos;
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType* operator->() const {
					return &this->pos;
				}

				/** @brief	Increment the iterator.
				  * @return	Reference of the iterator after incrementing.
				  */
				Self& operator++(void) {
					this->pos++;
					return *this;
				}

				/** @brief	Increment the iterator.
				  * @return	Copy of the iterator before incrementing.
				  */
				Self operator++(int) {
					Self ret = *this;
					this->pos++;
					return ret;
				}

				/** @brief	Decrement the iterator.
				  * @return	Reference of the iterator after decrementing.
				  */
				Self& operator--(void) {
					this->pos--;
					return *this;
				}

				/** @brief	Decrement the iterator.
				  * @return	Copy of the iterator before decrementing.
				  */
				Self operator--(int) {
					Self ret = *this;
					this->pos--;
					return ret;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs == rhs.
				  */
				bool operator==(const Self& other) const {
					return this->pos == other.pos;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs != rhs.
				  */
				bool operator!=(const Self& other) const {
					return this->pos != other.pos;
				}

			private:
				const HalfedgeMesh* pMesh;
				DereferenceType pos;
				HalfedgeIterator(
					const HalfedgeMesh* pMesh,
					std::size_t pos
				) : pMesh(pMesh), pos(pos) {}
				friend class HalfedgeRange;
			};

			/***********************************************************************
			 * @class	HalfedgeRange
			 * @brief	Halfedge range class for iterating all halfedges of a mesh.
			 ***********************************************************************/
			class HalfedgeRange {

			public:

				using IteratorType = HalfedgeIterator;

				/** @brief	Get the number of halfedges.
				  * @return	The number of halfedges.
				  */
				size_t size(void) const {
					return this->mesh.numHalfedges();
				}

				/** @brief	Get the iterator pointing to the first element.
				  * @return	An iterator pointing to the first element.
				  */
				IteratorType begin(void) const {
					return IteratorType(&this->mesh, 0);
				}

				/** @brief	Get the iterator referring to the past-the-end element.
				  * @return	An iterator referring to the past-the-end element.
				  */
				IteratorType end(void) const {
					return IteratorType(&this->mesh, this->mesh.numHalfedges());
				}

			private:
				const HalfedgeMesh& mesh;
				HalfedgeRange(const HalfedgeMesh& mesh) : mesh(mesh) {}
				friend class HalfedgeMesh;
			};

			/***********************************************************************
			 * @class	FaceIterator
			 * @brief	Iterator class for FaceRange.
			 ***********************************************************************/
			class FaceIterator {

			public:

				using Self = FaceIterator;
				using DereferenceType = HalfedgeIndex;

				/** @brief	Default constructor.
				  */
				FaceIterator(void) : pMesh(nullptr), pos() {}

				/** @brief	Check whether this iterator is valid or not.
				  * @return	`true` if this iterator is valid.
				  */
				bool valid(void) const {
					return (this->pMesh && this->pos.idx() < this->pMesh->numFaces());
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType& operator*() const {
					return this->pos;
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType* operator->() const {
					return &this->pos;
				}

				/** @brief	Increment the iterator.
				  * @return	Reference of the iterator after incrementing.
				  */
				Self& operator++(void) {
					this->pos++;
					return *this;
				}

				/** @brief	Increment the iterator.
				  * @return	Copy of the iterator before incrementing.
				  */
				Self operator++(int) {
					Self ret = *this;
					this->pos++;
					return ret;
				}

				/** @brief	Decrement the iterator.
				  * @return	Reference of the iterator after decrementing.
				  */
				Self& operator--(void) {
					this->pos--;
					return *this;
				}

				/** @brief	Decrement the iterator.
				  * @return	Copy of the iterator before decrementing.
				  */
				Self operator--(int) {
					Self ret = *this;
					this->pos--;
					return ret;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs == rhs.
				  */
				bool operator==(const Self& other) const {
					return this->pos == other.pos;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs != rhs.
				  */
				bool operator!=(const Self& other) const {
					return this->pos != other.pos;
				}

			private:
				const HalfedgeMesh* pMesh;
				DereferenceType pos;
				FaceIterator(
					const HalfedgeMesh* pMesh,
					std::size_t pos
				) : pMesh(pMesh), pos(pos) {}
				friend class FaceRange;
			};
			
			/***********************************************************************
			 * @class	FaceRange
			 * @brief	Face range class for iterating all faces of a mesh.
			 ***********************************************************************/
			class FaceRange {

			public:

				using IteratorType = FaceIterator;

				/** @brief	Get the number of faces.
				  * @return	The number of faces.
				  */
				size_t size(void) const {
					return this->mesh.numFaces();
				}

				/** @brief	Get the iterator pointing to the first element.
				  * @return	An iterator pointing to the first element.
				  */
				IteratorType begin(void) const {
					return IteratorType(&this->mesh, 0);
				}

				/** @brief	Get the iterator referring to the past-the-end element.
				  * @return	An iterator referring to the past-the-end element.
				  */
				IteratorType end(void) const {
					return IteratorType(&this->mesh, this->mesh.numFaces());
				}

			private:
				const HalfedgeMesh& mesh;
				FaceRange(const HalfedgeMesh& mesh) : mesh(mesh) {}
				friend class HalfedgeMesh;
			};

			/***********************************************************************
			 * @class	EdgeIterator
			 * @brief	Iterator class for EdgeRange.
			 ***********************************************************************/
			class EdgeIterator {

			public:

				using Self = EdgeIterator;
				using DereferenceType = EdgeIndex;

				/** @brief	Default constructor.
				  */
				EdgeIterator(void) : pMesh(nullptr), pos() {}

				/** @brief	Check whether this iterator is valid or not.
				  * @return	`true` if this iterator is valid.
				  */
				bool valid(void) const {
					return (this->pMesh && this->pos.idx() < this->pMesh->numEdges());
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType& operator*() const {
					return this->pos;
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType* operator->() const {
					return &this->pos;
				}

				/** @brief	Increment the iterator.
				  * @return	Reference of the iterator after incrementing.
				  */
				Self& operator++(void) {
					this->pos++;
					return *this;
				}

				/** @brief	Increment the iterator.
				  * @return	Copy of the iterator before incrementing.
				  */
				Self operator++(int) {
					Self ret = *this;
					this->pos++;
					return ret;
				}

				/** @brief	Decrement the iterator.
				  * @return	Reference of the iterator after decrementing.
				  */
				Self& operator--(void) {
					this->pos--;
					return *this;
				}

				/** @brief	Decrement the iterator.
				  * @return	Copy of the iterator before decrementing.
				  */
				Self operator--(int) {
					Self ret = *this;
					this->pos--;
					return ret;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs == rhs.
				  */
				bool operator==(const Self& other) const {
					return this->pos == other.pos;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs != rhs.
				  */
				bool operator!=(const Self& other) const {
					return this->pos != other.pos;
				}

			private:
				const HalfedgeMesh* pMesh;
				DereferenceType pos;
				EdgeIterator(
					const HalfedgeMesh* pMesh,
					std::size_t pos
				) : pMesh(pMesh), pos(pos) {}
				friend class EdgeRange;
			};

			/***********************************************************************
			 * @class	EdgeRange
			 * @brief	Edge range class for iterating all edges of a mesh.
			 ***********************************************************************/
			class EdgeRange {

			public:

				using IteratorType = EdgeIterator;

				/** @brief	Get the number of edges.
				  * @return	The number of faces.
				  */
				size_t size(void) const {
					return this->mesh.numEdges();
				}

				/** @brief	Get the iterator pointing to the first element.
				  * @return	An iterator pointing to the first element.
				  */
				IteratorType begin(void) const {
					return IteratorType(&this->mesh, 0);
				}

				/** @brief	Get the iterator referring to the past-the-end element.
				  * @return	An iterator referring to the past-the-end element.
				  */
				IteratorType end(void) const {
					return IteratorType(&this->mesh, this->mesh.numEdges());
				}

			private:
				const HalfedgeMesh& mesh;
				EdgeRange(const HalfedgeMesh& mesh) : mesh(mesh) {}
				friend class HalfedgeMesh;
			};

			/***********************************************************************
			 * @class	VertexHalfedgeIterator
			 * @brief	Iterator class for VertexHalfedgeRange.
			 ***********************************************************************/
			class VertexHalfedgeIterator {

			public:

				using Self = VertexHalfedgeIterator;
				using DereferenceType = HalfedgeIndex;

				/** @brief	Default constructor.
				  */
				VertexHalfedgeIterator(void) :
					pMesh(nullptr),
					center(),
					outgoing(true),
					clockwise(true),
					end(true),
					start(),
					current()
				{}

				/** @brief	Check whether this iterator is valid or not.
				  * @return	`true` if this iterator is valid.
				  */
				bool valid(void) const {
					return !this->end;
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType& operator*() const {
					return this->current;
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType* operator->() const {
					return &this->current;
				}

				/** @brief	Increment the iterator.
				  * @return	Reference of the iterator after incrementing.
				  */
				Self& operator++(void) {
					if (!this->end) {
						switch ((int)this->outgoing << 1 | (int)this->clockwise) {
						case 0b00:
							this->current = this->pMesh->halfedgePreviousHalfedge(this->pMesh->halfedgeOppositeHalfedge(this->current));
							break;
						case 0b01:
							this->current = this->pMesh->halfedgeOppositeHalfedge(this->pMesh->halfedgeNextHalfedge(this->current));
							break;
						case 0b10:
							this->current = this->pMesh->halfedgeOppositeHalfedge(this->pMesh->halfedgePreviousHalfedge(this->current));
							break;
						case 0b11:
							this->current = this->pMesh->halfedgeNextHalfedge(this->pMesh->halfedgeOppositeHalfedge(this->current));
							break;
						}
						if (!this->current.valid() || this->current == this->start) {
							this->end = true;
						}
					}
					return *this;
				}

				/** @brief	Increment the iterator.
				  * @return	Copy of the iterator before incrementing.
				  */
				Self operator++(int) {
					Self ret = *this;
					this->operator++();
					return ret;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs == rhs.
				  */
				bool operator==(const Self& other) const {
					return (this->end == other.end && (this->end || this->current == other.current));
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs != rhs.
				  */
				bool operator!=(const Self& other) const {
					return !(this->operator==(other));
				}

			private:
				const HalfedgeMesh* pMesh;
				VertexIndex center;
				bool outgoing;
				bool clockwise;
				bool end;
				HalfedgeIndex start;
				HalfedgeIndex current;
				VertexHalfedgeIterator(
					const HalfedgeMesh* pMesh,
					VertexIndex center,
					bool outgoing,
					bool clockwise,
					bool end,
					HalfedgeIndex start
				) : pMesh(pMesh),
					center(center),
					outgoing(outgoing),
					clockwise(clockwise),
					end(end || !pMesh || center.idx() >= pMesh->numVertices()),
					start(
						(this->end ?
							HalfedgeIndex() :
							(outgoing ?
								(pMesh->halfedgeSourceVertex(start) == center ?
									start : pMesh->vertexOutgoingHalfedge(center)) :
								(pMesh->halfedgeTargetVertex(start) == center ?
									start : pMesh->vertexIngoingHalfedge(center))))
					),
					current(this->start)
				{
					if (!this->start.valid())
						this->end = true;
				}
				friend class VertexHalfedgeRange;
				friend class VertexVertexIterator;
				friend class VertexFaceIterator;
				friend class VertexEdgeIterator;
			};

			/***********************************************************************
			 * @class	VertexHalfedgeRange
			 * @brief	Vertex-halfedge range class for iterating all halfedges
			 *			starting from / pointing to a vertex of a mesh.
			 ***********************************************************************/
			class VertexHalfedgeRange {

			public:

				using IteratorType = VertexHalfedgeIterator;

				/** @brief	Get the iterator pointing to the first element.
				  * @return	An iterator pointing to the first element.
				  */
				IteratorType begin(void) const {
					return IteratorType(&this->mesh, this->center, this->outgoing, this->clockwise, false, this->start);
				}

				/** @brief	Get the iterator referring to the past-the-end element.
				  * @return	An iterator referring to the past-the-end element.
				  */
				IteratorType end(void) const {
					return IteratorType(&this->mesh, this->center, this->outgoing, this->clockwise, true, this->start);
				}

			private:
				const HalfedgeMesh& mesh;
				VertexIndex center;
				bool outgoing;
				bool clockwise;
				HalfedgeIndex start;
				VertexHalfedgeRange(const HalfedgeMesh& mesh, VertexIndex center, bool outgoing, bool clockwise, HalfedgeIndex start) :
					mesh(mesh),
					center(center),
					outgoing(outgoing),
					clockwise(clockwise),
					start(start)
				{}
				friend class HalfedgeMesh;
			};

			/***********************************************************************
			 * @class	VertexVertexIterator
			 * @brief	Iterator class for VertexVertexRange.
			 ***********************************************************************/
			class VertexVertexIterator {

			public:

				using Self = VertexVertexIterator;
				using DereferenceType = VertexIndex;

				/** @brief	Default constructor.
				  */
				VertexVertexIterator(void) :
					pMesh(nullptr),
					iterator(),
					current()
				{}

				/** @brief	Check whether this iterator is valid or not.
				  * @return	`true` if this iterator is valid.
				  */
				bool valid(void) const {
					return this->iterator.valid();
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType& operator*() const {
					return this->current;
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType* operator->() const {
					return &this->current;
				}

				/** @brief	Increment the iterator.
				  * @return	Reference of the iterator after incrementing.
				  */
				Self& operator++(void) {
					++this->iterator;
					this->current = this->pMesh->halfedgeTargetVertex(*this->iterator);
					return *this;
				}

				/** @brief	Increment the iterator.
				  * @return	Copy of the iterator before incrementing.
				  */
				Self operator++(int) {
					Self ret = *this;
					++this->iterator;
					this->current = this->pMesh->halfedgeTargetVertex(*this->iterator);
					return ret;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs == rhs.
				  */
				bool operator==(const Self& other) const {
					return this->iterator == other.iterator;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs != rhs.
				  */
				bool operator!=(const Self& other) const {
					return !(this->operator==(other));
				}

			private:
				const HalfedgeMesh* pMesh;
				VertexHalfedgeIterator iterator;
				DereferenceType current;
				VertexVertexIterator(
					const HalfedgeMesh* pMesh,
					VertexIndex center,
					bool clockwise,
					bool end,
					VertexIndex start
				) : pMesh(pMesh),
					iterator(pMesh, center, true, clockwise, end, HalfedgeIndex())
				{
					if (this->pMesh && center.idx() < this->pMesh->numVertices() && start.idx() < this->pMesh->numVertices()) {
						bool find = false;
						for (; this->iterator.valid(); ++this->iterator)
							if (this->pMesh->halfedgeTargetVertex(*this->iterator) == start) {
								find = true;
								break;
							}
						this->iterator = VertexHalfedgeIterator(pMesh, center, true, clockwise, end, find ? *this->iterator : HalfedgeIndex());
					}
					if (this->pMesh && center.idx() < this->pMesh->numVertices()) {
						this->current = this->pMesh->halfedgeTargetVertex(*this->iterator);
					}
				}
				friend class VertexVertexRange;
			};

			/***********************************************************************
			 * @class	VertexVertexRange
			 * @brief	Vertex-vertex range class for iterating all adjacent
			 *			vertices of a vertex of a mesh.
			 ***********************************************************************/
			class VertexVertexRange {

			public:

				using IteratorType = VertexVertexIterator;

				/** @brief	Get the iterator pointing to the first element.
				  * @return	An iterator pointing to the first element.
				  */
				IteratorType begin(void) const {
					return IteratorType(&this->mesh, this->center, this->clockwise, false, this->start);
				}

				/** @brief	Get the iterator referring to the past-the-end element.
				  * @return	An iterator referring to the past-the-end element.
				  */
				IteratorType end(void) const {
					return IteratorType(&this->mesh, this->center, this->clockwise, true, this->start);
				}

			private:
				const HalfedgeMesh& mesh;
				VertexIndex center;
				bool clockwise;
				VertexIndex start;
				VertexVertexRange(const HalfedgeMesh& mesh, VertexIndex center, bool clockwise, VertexIndex start) :
					mesh(mesh),
					center(center),
					clockwise(clockwise),
					start(start)
				{}
				friend class HalfedgeMesh;
			};

			/***********************************************************************
			 * @class	VertexFaceIterator
			 * @brief	Iterator class for VertexFaceRange.
			 ***********************************************************************/
			class VertexFaceIterator {

			public:

				using Self = VertexFaceIterator;
				using DereferenceType = FaceIndex;

				/** @brief	Default constructor.
				  */
				VertexFaceIterator(void) :
					pMesh(nullptr),
					iterator(),
					current()
				{}

				/** @brief	Check whether this iterator is valid or not.
				  * @return	`true` if this iterator is valid.
				  */
				bool valid(void) const {
					return this->iterator.valid();
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType& operator*() const {
					return this->current;
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType* operator->() const {
					return &this->current;
				}

				/** @brief	Increment the iterator.
				  * @return	Reference of the iterator after incrementing.
				  */
				Self& operator++(void) {
					++this->iterator;
					this->current = this->pMesh->halfedgeAssociatedFace(*this->iterator);
					return *this;
				}

				/** @brief	Increment the iterator.
				  * @return	Copy of the iterator before incrementing.
				  */
				Self operator++(int) {
					Self ret = *this;
					++this->iterator;
					this->current = this->pMesh->halfedgeAssociatedFace(*this->iterator);
					return ret;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs == rhs.
				  */
				bool operator==(const Self& other) const {
					return this->iterator == other.iterator;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs != rhs.
				  */
				bool operator!=(const Self& other) const {
					return !(this->operator==(other));
				}

			private:
				const HalfedgeMesh* pMesh;
				VertexHalfedgeIterator iterator;
				DereferenceType current;
				VertexFaceIterator(
					const HalfedgeMesh* pMesh,
					VertexIndex center,
					bool clockwise,
					bool end,
					FaceIndex start
				) : pMesh(pMesh),
					iterator(pMesh, center, true, clockwise, end, HalfedgeIndex())
				{
					if (this->pMesh && center.idx() < this->pMesh->numVertices() && start.idx() < this->pMesh->numFaces()) {
						bool find = false;
						for (; this->iterator.valid(); ++this->iterator)
							if (this->pMesh->halfedgeAssociatedFace(*this->iterator) == start) {
								find = true;
								break;
							}
						this->iterator = VertexHalfedgeIterator(pMesh, center, true, clockwise, end, find ? *this->iterator : HalfedgeIndex());
					}
					if (this->pMesh && center.idx() < this->pMesh->numVertices()) {
						this->current = this->pMesh->halfedgeAssociatedFace(*this->iterator);
					}
				}
				friend class VertexFaceRange;
			};

			/***********************************************************************
			 * @class	VertexFaceRange
			 * @brief	Vertex-face range class for iterating all adjacent faces of
			 *			a vertex of a mesh.
			 ***********************************************************************/
			class VertexFaceRange {

			public:

				using IteratorType = VertexFaceIterator;

				/** @brief	Get the iterator pointing to the first element.
				  * @return	An iterator pointing to the first element.
				  */
				IteratorType begin(void) const {
					return IteratorType(&this->mesh, this->center, this->clockwise, false, this->start);
				}

				/** @brief	Get the iterator referring to the past-the-end element.
				  * @return	An iterator referring to the past-the-end element.
				  */
				IteratorType end(void) const {
					return IteratorType(&this->mesh, this->center, this->clockwise, true, this->start);
				}

			private:
				const HalfedgeMesh& mesh;
				VertexIndex center;
				bool clockwise;
				FaceIndex start;
				VertexFaceRange(const HalfedgeMesh& mesh, VertexIndex center, bool clockwise, FaceIndex start) :
					mesh(mesh),
					center(center),
					clockwise(clockwise),
					start(start)
				{}
				friend class HalfedgeMesh;
			};

			/***********************************************************************
			 * @class	VertexEdgeIterator
			 * @brief	Iterator class for VertexEdgeRange.
			 ***********************************************************************/
			class VertexEdgeIterator {

			public:

				using Self = VertexEdgeIterator;
				using DereferenceType = EdgeIndex;

				/** @brief	Default constructor.
				  */
				VertexEdgeIterator(void) :
					pMesh(pMesh),
					iterator(),
					current()
				{}

				/** @brief	Check whether this iterator is valid or not.
				  * @return	`true` if this iterator is valid.
				  */
				bool valid(void) const {
					return this->iterator.valid();
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType& operator*() const {
					return this->current;
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType* operator->() const {
					return &this->current;
				}

				/** @brief	Increment the iterator.
				  * @return	Reference of the iterator after incrementing.
				  */
				Self& operator++(void) {
					++this->iterator;
					this->current = this->pMesh->halfedgeAssociatedEdge(*this->iterator);
					return *this;
				}

				/** @brief	Increment the iterator.
				  * @return	Copy of the iterator before incrementing.
				  */
				Self operator++(int) {
					Self ret = *this;
					++this->iterator;
					this->current = this->pMesh->halfedgeAssociatedEdge(*this->iterator);
					return ret;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs == rhs.
				  */
				bool operator==(const Self& other) const {
					return this->iterator == other.iterator;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs != rhs.
				  */
				bool operator!=(const Self& other) const {
					return !(this->operator==(other));
				}

			private:
				const HalfedgeMesh* pMesh;
				VertexHalfedgeIterator iterator;
				DereferenceType current;
				VertexEdgeIterator(
					const HalfedgeMesh* pMesh,
					VertexIndex center,
					bool clockwise,
					bool end,
					EdgeIndex start
				) : pMesh(pMesh),
					iterator(pMesh, center, true, clockwise, end, HalfedgeIndex())
				{
					if (this->pMesh && center.idx() < this->pMesh->numVertices() && start.idx() < this->pMesh->numEdges()) {
						bool find = false;
						for (; this->iterator.valid(); ++this->iterator)
							if (this->pMesh->halfedgeAssociatedEdge(*this->iterator) == start) {
								find = true;
								break;
							}
						this->iterator = VertexHalfedgeIterator(pMesh, center, true, clockwise, end, find ? *this->iterator : HalfedgeIndex());
					}
					if (this->pMesh && center.idx() < this->pMesh->numVertices()) {
						this->current = this->pMesh->halfedgeAssociatedEdge(*this->iterator);
					}
				}
				friend class VertexEdgeRange;
			};

			/***********************************************************************
			 * @class	VertexEdgeRange
			 * @brief	Vertex-edge range class for iterating all incident edges of
			 *			a vertex of a mesh.
			 ***********************************************************************/
			class VertexEdgeRange {

			public:

				using IteratorType = VertexEdgeIterator;

				/** @brief	Get the iterator pointing to the first element.
				  * @return	An iterator pointing to the first element.
				  */
				IteratorType begin(void) const {
					return IteratorType(&this->mesh, this->center, this->clockwise, false, this->start);
				}

				/** @brief	Get the iterator referring to the past-the-end element.
				  * @return	An iterator referring to the past-the-end element.
				  */
				IteratorType end(void) const {
					return IteratorType(&this->mesh, this->center, this->clockwise, true, this->start);
				}

			private:
				const HalfedgeMesh& mesh;
				VertexIndex center;
				bool clockwise;
				EdgeIndex start;
				VertexEdgeRange(const HalfedgeMesh& mesh, VertexIndex center, bool clockwise, EdgeIndex start) :
					mesh(mesh),
					center(center),
					clockwise(clockwise),
					start(start)
				{}
				friend class HalfedgeMesh;
			};

			/***********************************************************************
			 * @class	FaceHalfedgeIterator
			 * @brief	Iterator class for FaceHalfedgeRange.
			 ***********************************************************************/
			class FaceHalfedgeIterator {

			public:

				using Self = FaceHalfedgeIterator;
				using DereferenceType = HalfedgeIndex;

				/** @brief	Default constructor.
				  */
				FaceHalfedgeIterator(void) :
					pMesh(nullptr),
					center(),
					positiveOrder(true),
					end(true),
					start(),
					current()
				{}

				/** @brief	Check whether this iterator is valid or not.
				  * @return	`true` if this iterator is valid.
				  */
				bool valid(void) const {
					return !this->end;
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType& operator*() const {
					return this->current;
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType* operator->() const {
					return &this->current;
				}

				/** @brief	Increment the iterator.
				  * @return	Reference of the iterator after incrementing.
				  */
				Self& operator++(void) {
					if (!this->end) {
						if (this->positiveOrder)
							this->current = this->pMesh->halfedgeNextHalfedge(this->current);
						else
							this->current = this->pMesh->halfedgePreviousHalfedge(this->current);
						if (!this->current.valid() || this->current == this->start) {
							this->end = true;
						}
					}
					return *this;
				}

				/** @brief	Increment the iterator.
				  * @return	Copy of the iterator before incrementing.
				  */
				Self operator++(int) {
					Self ret = *this;
					this->operator++();
					return ret;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs == rhs.
				  */
				bool operator==(const Self& other) const {
					return (this->end == other.end && (this->end || this->current == other.current));
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs != rhs.
				  */
				bool operator!=(const Self& other) const {
					return !(this->operator==(other));
				}

			private:
				const HalfedgeMesh* pMesh;
				FaceIndex center;
				bool positiveOrder;
				bool end;
				HalfedgeIndex start;
				HalfedgeIndex current;
				FaceHalfedgeIterator(
					const HalfedgeMesh* pMesh,
					FaceIndex center,
					bool positiveOrder,
					bool end,
					HalfedgeIndex start
				) : pMesh(pMesh),
					center(center),
					positiveOrder(positiveOrder),
					end(end || !pMesh || center.idx() >= pMesh->numFaces()),
					start(
						(this->end ?
							HalfedgeIndex() :
							(pMesh->halfedgeAssociatedFace(start) == center ?
								start : pMesh->faceAssociatedHalfedge(center)))
					),
					current(this->start)
				{
					if (!this->start.valid())
						this->end = true;
				}
				friend class FaceHalfedgeRange;
				friend class FaceVertexIterator;
				friend class FaceFaceIterator;
				friend class FaceEdgeIterator;
			};

			/***********************************************************************
			 * @class	FaceHalfedgeRange
			 * @brief	Face-halfedge range class for iterating all halfedges
			 *			around a face of a mesh.
			 ***********************************************************************/
			class FaceHalfedgeRange {

			public:

				using IteratorType = FaceHalfedgeIterator;

				/** @brief	Get the iterator pointing to the first element.
				  * @return	An iterator pointing to the first element.
				  */
				IteratorType begin(void) const {
					return IteratorType(&this->mesh, this->center, this->positiveOrder, false, this->start);
				}

				/** @brief	Get the iterator referring to the past-the-end element.
				  * @return	An iterator referring to the past-the-end element.
				  */
				IteratorType end(void) const {
					return IteratorType(&this->mesh, this->center, this->positiveOrder, true, this->start);
				}

			private:
				const HalfedgeMesh& mesh;
				FaceIndex center;
				bool positiveOrder;
				HalfedgeIndex start;
				FaceHalfedgeRange(const HalfedgeMesh& mesh, FaceIndex center, bool positiveOrder, HalfedgeIndex start) :
					mesh(mesh),
					center(center),
					positiveOrder(positiveOrder),
					start(start)
				{}
				friend class HalfedgeMesh;
			};

			/***********************************************************************
			 * @class	FaceVertexIterator
			 * @brief	Iterator class for FaceVertexRange.
			 ***********************************************************************/
			class FaceVertexIterator {

			public:

				using Self = FaceVertexIterator;
				using DereferenceType = VertexIndex;

				/** @brief	Default constructor.
				  */
				FaceVertexIterator(void) :
					pMesh(nullptr),
					iterator(),
					current()
				{}

				/** @brief	Check whether this iterator is valid or not.
				  * @return	`true` if this iterator is valid.
				  */
				bool valid(void) const {
					return this->iterator.valid();
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType& operator*() const {
					return this->current;
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType* operator->() const {
					return &this->current;
				}

				/** @brief	Increment the iterator.
				  * @return	Reference of the iterator after incrementing.
				  */
				Self& operator++(void) {
					++this->iterator;
					this->current = this->pMesh->halfedgeTargetVertex(*this->iterator);
					return *this;
				}

				/** @brief	Increment the iterator.
				  * @return	Copy of the iterator before incrementing.
				  */
				Self operator++(int) {
					Self ret = *this;
					++this->iterator;
					this->current = this->pMesh->halfedgeTargetVertex(*this->iterator);
					return ret;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs == rhs.
				  */
				bool operator==(const Self& other) const {
					return this->iterator == other.iterator;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs != rhs.
				  */
				bool operator!=(const Self& other) const {
					return !(this->operator==(other));
				}

			private:
				const HalfedgeMesh* pMesh;
				FaceHalfedgeIterator iterator;
				DereferenceType current;
				FaceVertexIterator(
					const HalfedgeMesh* pMesh,
					FaceIndex center,
					bool positiveOrder,
					bool end,
					VertexIndex start
				) : pMesh(pMesh),
					iterator(pMesh, center, positiveOrder, end, HalfedgeIndex())
				{
					if (this->pMesh && center.idx() < this->pMesh->numFaces() && start.idx() < this->pMesh->numVertices()) {
						bool find = false;
						for (; this->iterator.valid(); ++this->iterator)
							if (this->pMesh->halfedgeTargetVertex(*this->iterator) == start) {
								find = true;
								break;
							}
						this->iterator = FaceHalfedgeIterator(pMesh, center, positiveOrder, end, find ? *this->iterator : HalfedgeIndex());
					}
					if (this->pMesh && center.idx() < this->pMesh->numFaces()) {
						this->current = this->pMesh->halfedgeTargetVertex(*this->iterator);
					}
				}
				friend class FaceVertexRange;
			};
			
			/***********************************************************************
			 * @class	FaceVertexRange
			 * @brief	Face-vertex range class for iterating all
			 *			vertices around a face of a mesh.
			 ***********************************************************************/
			class FaceVertexRange {

			public:

				using IteratorType = FaceVertexIterator;

				/** @brief	Get the iterator pointing to the first element.
				  * @return	An iterator pointing to the first element.
				  */
				IteratorType begin(void) const {
					return IteratorType(&this->mesh, this->center, this->positiveOrder, false, this->start);
				}

				/** @brief	Get the iterator referring to the past-the-end element.
				  * @return	An iterator referring to the past-the-end element.
				  */
				IteratorType end(void) const {
					return IteratorType(&this->mesh, this->center, this->positiveOrder, true, this->start);
				}

			private:
				const HalfedgeMesh& mesh;
				FaceIndex center;
				bool positiveOrder;
				VertexIndex start;
				FaceVertexRange(const HalfedgeMesh& mesh, FaceIndex center, bool positiveOrder, VertexIndex start) :
					mesh(mesh),
					center(center),
					positiveOrder(positiveOrder),
					start(start)
				{}
				friend class HalfedgeMesh;
			};

			/***********************************************************************
			 * @class	FaceFaceIterator
			 * @brief	Iterator class for FaceFaceRange.
			 ***********************************************************************/
			class FaceFaceIterator {

			public:

				using Self = FaceFaceIterator;
				using DereferenceType = FaceIndex;

				/** @brief	Default constructor.
				  */
				FaceFaceIterator(void) :
					pMesh(nullptr),
					iterator(),
					current()
				{}

				/** @brief	Check whether this iterator is valid or not.
				  * @return	`true` if this iterator is valid.
				  */
				bool valid(void) const {
					return this->iterator.valid();
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType& operator*() const {
					return this->current;
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType* operator->() const {
					return &this->current;
				}

				/** @brief	Increment the iterator.
				  * @return	Reference of the iterator after incrementing.
				  */
				Self& operator++(void) {
					++this->iterator;
					this->current = this->pMesh->halfedgeAssociatedFace(this->pMesh->halfedgeOppositeHalfedge(*this->iterator));
					return *this;
				}

				/** @brief	Increment the iterator.
				  * @return	Copy of the iterator before incrementing.
				  */
				Self operator++(int) {
					Self ret = *this;
					++this->iterator;
					this->current = this->pMesh->halfedgeAssociatedFace(this->pMesh->halfedgeOppositeHalfedge(*this->iterator));
					return ret;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs == rhs.
				  */
				bool operator==(const Self& other) const {
					return this->iterator == other.iterator;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs != rhs.
				  */
				bool operator!=(const Self& other) const {
					return !(this->operator==(other));
				}

			private:
				const HalfedgeMesh* pMesh;
				FaceHalfedgeIterator iterator;
				DereferenceType current;
				FaceFaceIterator(
					const HalfedgeMesh* pMesh,
					FaceIndex center,
					bool positiveOrder,
					bool end,
					FaceIndex start
				) : pMesh(pMesh),
					iterator(pMesh, center, positiveOrder, end, HalfedgeIndex())
				{
					if (this->pMesh && center.idx() < this->pMesh->numFaces() && start.idx() < this->pMesh->numFaces()) {
						bool find = false;
						for (; this->iterator.valid(); ++this->iterator)
							if (this->pMesh->halfedgeAssociatedFace(this->pMesh->halfedgeOppositeHalfedge(*this->iterator)) == start) {
								find = true;
								break;
							}
						this->iterator = FaceHalfedgeIterator(pMesh, center, positiveOrder, end, find ? *this->iterator : HalfedgeIndex());
					}
					if (this->pMesh && center.idx() < this->pMesh->numFaces()) {
						this->current = this->pMesh->halfedgeAssociatedFace(this->pMesh->halfedgeOppositeHalfedge(*this->iterator));
					}
				}
				friend class FaceFaceRange;
			};

			/***********************************************************************
			 * @class	FaceFaceRange
			 * @brief	Face-face range class for iterating all adjacent faces of
			 *			a face of a mesh.
			 ***********************************************************************/
			class FaceFaceRange {

			public:

				using IteratorType = FaceFaceIterator;

				/** @brief	Get the iterator pointing to the first element.
				  * @return	An iterator pointing to the first element.
				  */
				IteratorType begin(void) const {
					return IteratorType(&this->mesh, this->center, this->positiveOrder, false, this->start);
				}

				/** @brief	Get the iterator referring to the past-the-end element.
				  * @return	An iterator referring to the past-the-end element.
				  */
				IteratorType end(void) const {
					return IteratorType(&this->mesh, this->center, this->positiveOrder, true, this->start);
				}

			private:
				const HalfedgeMesh& mesh;
				FaceIndex center;
				bool positiveOrder;
				FaceIndex start;
				FaceFaceRange(const HalfedgeMesh& mesh, FaceIndex center, bool positiveOrder, FaceIndex start) :
					mesh(mesh),
					center(center),
					positiveOrder(positiveOrder),
					start(start)
				{}
				friend class HalfedgeMesh;
			};

			/***********************************************************************
			 * @class	FaceEdgeIterator
			 * @brief	Iterator class for FaceEdgeRange.
			 ***********************************************************************/
			class FaceEdgeIterator {

			public:

				using Self = FaceEdgeIterator;
				using DereferenceType = EdgeIndex;

				/** @brief	Default constructor.
				  */
				FaceEdgeIterator(void) :
					pMesh(nullptr),
					iterator(),
					current()
				{}

				/** @brief	Check whether this iterator is valid or not.
				  * @return	`true` if this iterator is valid.
				  */
				bool valid(void) const {
					return this->iterator.valid();
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType& operator*() const {
					return this->current;
				}

				/** @brief	Fetch the current element.
				  * @return	The current element.
				  */
				const DereferenceType* operator->() const {
					return &this->current;
				}

				/** @brief	Increment the iterator.
				  * @return	Reference of the iterator after incrementing.
				  */
				Self& operator++(void) {
					++this->iterator;
					this->current = this->pMesh->halfedgeAssociatedEdge(*this->iterator);
					return *this;
				}

				/** @brief	Increment the iterator.
				  * @return	Copy of the iterator before incrementing.
				  */
				Self operator++(int) {
					Self ret = *this;
					++this->iterator;
					this->current = this->pMesh->halfedgeAssociatedEdge(*this->iterator);
					return ret;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs == rhs.
				  */
				bool operator==(const Self& other) const {
					return this->iterator == other.iterator;
				}

				/** @brief	Compare two iterators.
				  * @return	`true` if lhs != rhs.
				  */
				bool operator!=(const Self& other) const {
					return !(this->operator==(other));
				}

			private:
				const HalfedgeMesh* pMesh;
				FaceHalfedgeIterator iterator;
				DereferenceType current;
				FaceEdgeIterator(
					const HalfedgeMesh* pMesh,
					FaceIndex center,
					bool positiveOrder,
					bool end,
					EdgeIndex start
				) : pMesh(pMesh),
					iterator(pMesh, center, positiveOrder, end, HalfedgeIndex())
				{
					if (this->pMesh && center.idx() < this->pMesh->numFaces() && start.idx() < this->pMesh->numEdges()) {
						bool find = false;
						for (; this->iterator.valid(); ++this->iterator)
							if (this->pMesh->halfedgeAssociatedEdge(*this->iterator) == start) {
								find = true;
								break;
							}
						this->iterator = FaceHalfedgeIterator(pMesh, center, positiveOrder, end, find ? *this->iterator : HalfedgeIndex());
					}
					if (this->pMesh && center.idx() < this->pMesh->numFaces()) {
						this->current = this->pMesh->halfedgeAssociatedEdge(*this->iterator);
					}
				}
				friend class FaceEdgeRange;
			};

			/***********************************************************************
			 * @class	FaceEdgeRange
			 * @brief	Face-edge range class for iterating all edges around
			 *			a face of a mesh.
			 ***********************************************************************/
			class FaceEdgeRange {

			public:

				/** @brief	Get the iterator pointing to the first element.
				  * @return	An iterator pointing to the first element.
				  */
				FaceEdgeIterator begin(void) const {
					return FaceEdgeIterator(&this->mesh, this->center, this->positiveOrder, false, this->start);
				}

				/** @brief	Get the iterator referring to the past-the-end element.
				  * @return	An iterator referring to the past-the-end element.
				  */
				FaceEdgeIterator end(void) const {
					return FaceEdgeIterator(&this->mesh, this->center, this->positiveOrder, true, this->start);
				}

			private:
				const HalfedgeMesh& mesh;
				FaceIndex center;
				bool positiveOrder;
				EdgeIndex start;
				FaceEdgeRange(const HalfedgeMesh& mesh, FaceIndex center, bool positiveOrder, EdgeIndex start) :
					mesh(mesh),
					center(center),
					positiveOrder(positiveOrder),
					start(start)
				{}
				friend class HalfedgeMesh;
			};
			
			/** @}
			  */

		public:

			std::size_t numVertices(void) const {
				return this->vertexInfo.size();
			}

			std::size_t numHalfedges(void) const {
				return this->halfedgeInfo.size();
			}

			std::size_t numFaces(void) const {
				return this->faceInfo.size();
			}

			std::size_t numEdges(void) const {
				return this->halfedgeInfo.size() / 2;
			}

			VertexRange vertices(void) const {
				return VertexRange(*this);
			}

			VertexIndex vertex(std::size_t pos) const {
				return VertexIndex(pos);
			}

			HalfedgeRange halfedges(void) const {
				return HalfedgeRange(*this);
			}

			HalfedgeIndex halfedge(std::size_t pos) const {
				return HalfedgeIndex(pos);
			}

			FaceRange faces(void) const {
				return FaceRange(*this);
			}

			FaceIndex face(std::size_t pos) const {
				return FaceIndex(pos);
			}

			EdgeRange edges(void) const {
				return EdgeRange(*this);
			}

			EdgeIndex edge(std::size_t pos) const {
				return EdgeIndex(pos);
			}

			VertexVertexRange vertexVertices(VertexIndex center, bool clockwise = true,VertexIndex start = VertexIndex()) const {
				return VertexVertexRange(*this, center, clockwise, start);
			}

			/** @brief	Get the range to iterate over all halfedges starting from / pointing to the vertex.
			  *
			  * @param	center		Vertex whose associated halfedges need to be iterated.
			  * @param	outgoing	`true` for outgoing halfedges and `false` for ingoing halfedges.
			  * @param	clockwise	`true` for clockwise order and `false` for anti-clockwise order.
			  * @param	start		The first halfedge to be iterated. If it is illegal then a default one will be chosen.
			  */
			VertexHalfedgeRange vertexHalfedges(VertexIndex center, bool outgoing, bool clockwise = true, HalfedgeIndex start = HalfedgeIndex()) const {
				return VertexHalfedgeRange(*this, center, outgoing, clockwise, start);
			}

			VertexFaceRange vertexFaces(VertexIndex center, bool clockwise = true, FaceIndex start = FaceIndex()) const {
				return VertexFaceRange(*this, center, clockwise, start);
			}

			VertexEdgeRange vertexEdges(VertexIndex center, bool clockwise = true, EdgeIndex start = EdgeIndex()) const {
				return VertexEdgeRange(*this, center, clockwise, start);
			}

			FaceVertexRange faceVertices(FaceIndex center, bool positiveOrder = true, VertexIndex start = VertexIndex()) const {
				return FaceVertexRange(*this, center, positiveOrder, start);
			}

			/** @brief	Constructor.
			  *
			  * @param	pMesh			Pointer to the halfedge mesh instance.
			  *							If it is `nullptr`, this iterator will be an ending iterator.
			  * @param	center			Face whose associated halfedges need to be iterated.
			  *							If it is invalid, this iterator will be an ending iterator.
			  * @param	positiveOrder	`true` for positive order and `false` for reverse order.
			  * @param	start			The first halfedge to be iterated. If it is illegal then a default one will be chosen.
			  * @param	end				`true` for constructing an ending iterator.
			  */
			FaceHalfedgeRange faceHalfedges(FaceIndex center, bool positiveOrder = true, HalfedgeIndex start = HalfedgeIndex()) const {
				return FaceHalfedgeRange(*this, center, positiveOrder, start);
			}

			FaceFaceRange faceFaces(FaceIndex center, bool positiveOrder = true, FaceIndex start = FaceIndex()) const {
				return FaceFaceRange(*this, center, positiveOrder, start);
			}

			FaceEdgeRange faceEdges(FaceIndex center, bool positiveOrder = true, EdgeIndex start = EdgeIndex()) const {
				return FaceEdgeRange(*this, center, positiveOrder, start);
			}

			HalfedgeIndex vertexOutgoingHalfedge(VertexIndex vertex) const {
				return vertex.idx() < this->numVertices() ?
					this->vertexInfo[vertex.idx()].halfedge :
					HalfedgeIndex();
			}

			HalfedgeIndex vertexIngoingHalfedge(VertexIndex vertex) const {
				return this->halfedgeOppositeHalfedge(this->vertexOutgoingHalfedge(vertex));
			}

			VertexIndex halfedgeSourceVertex(HalfedgeIndex halfedge) const {
				return this->halfedgeTargetVertex(this->halfedgeOppositeHalfedge(halfedge));
			}

			VertexIndex halfedgeTargetVertex(HalfedgeIndex halfedge) const {
				return halfedge.idx() < this->numHalfedges() ?
					this->halfedgeInfo[halfedge.idx()].target :
					VertexIndex();
			}

			HalfedgeIndex halfedgeOppositeHalfedge(HalfedgeIndex halfedge) const {
				return halfedge.idx() < this->numHalfedges() ?
					((halfedge.idx() % 2) ? (halfedge - 1) : (halfedge + 1)) :
					HalfedgeIndex();
			}

			HalfedgeIndex halfedgeNextHalfedge(HalfedgeIndex halfedge) const {
				return halfedge.idx() < this->numHalfedges() ? 
					this->halfedgeInfo[halfedge.idx()].next :
					HalfedgeIndex();
			}

			HalfedgeIndex halfedgePreviousHalfedge(HalfedgeIndex halfedge) const {
				return halfedge.idx() < this->numHalfedges() ?
					this->halfedgeInfo[halfedge.idx()].prev :
					HalfedgeIndex();
			}

			FaceIndex halfedgeAssociatedFace(HalfedgeIndex halfedge) const {
				return halfedge.idx() < this->numHalfedges() ?
					this->halfedgeInfo[halfedge.idx()].face :
					FaceIndex();
			}

			EdgeIndex halfedgeAssociatedEdge(HalfedgeIndex halfedge) const {
				return halfedge.idx() < this->numHalfedges() ?
					EdgeIndex(halfedge.idx() / 2) :
					EdgeIndex();
			}

			HalfedgeIndex faceAssociatedHalfedge(FaceIndex face) const {
				return face.idx() < this->numFaces() ?
					this->faceInfo[face.idx()].halfedge :
					HalfedgeIndex();
			}

			/** @brief	Get the associated halfedge of the edge.
			  *
			  * @param	edge		Vertex whose associated halfedges need to be iterated.
			  * @param	dir			`true` if the halfedge points from a smaller vertex to a bigger vertex.
			  */
			HalfedgeIndex edgeAssociatedHalfedge(EdgeIndex edge, bool dir) const {
				return edge.idx() < this->numEdges() ?
					(dir ?
						HalfedgeIndex(edge.idx() * 2) :
						HalfedgeIndex(edge.idx() * 2 + 1)) :
					HalfedgeIndex();
			}

			HalfedgeMesh(void) {}

			void reset(void) {

			}

			bool load(
				std::vector<Eigen::Vector3d> points,
				std::vector<std::vector<int>> faces
			) {
				this->reset();
				this->vertexInfo.resize(points.size());
				this->halfedgeInfo.reserve(faces.size() * 3);
				this->faceInfo.resize(faces.size());
				std::map<std::pair<VertexIndex, VertexIndex>, EdgeIndex> edges;
				for (int fi = 0; fi < faces.size(); fi++) {
					const auto& f = faces[fi];
					std::vector<HalfedgeIndex> faceHalfedges(f.size());
					for (int hi = 0; hi < f.size(); hi++) {
						VertexIndex v1(f[(hi - 1 + f.size()) % f.size()]);
						VertexIndex v2(f[hi]);
						bool dir;
						std::pair<VertexIndex, VertexIndex> key;
						if (v1 > v2) {
							dir = false;
							key = { v2, v1 };
						}
						else {
							dir = true;
							key = { v1, v2 };
						}
						EdgeIndex& edge = edges[key];
						if (!edge.valid()) {
							edge = EdgeIndex(edges.size() - 1);
							this->halfedgeInfo.emplace_back();
							this->halfedgeInfo.emplace_back();
						}
						faceHalfedges[hi] = this->edgeAssociatedHalfedge(edge, dir);
					}
					for (int hi = 0; hi < f.size(); hi++) {
						VertexIndex v1(f[(hi - 1 + f.size()) % f.size()]);
						VertexIndex v2(f[hi]);
						//If this halfedge's face is already set, the mesh is not a valid mesh for halfedge structure.
						if (this->halfedgeInfo[faceHalfedges[hi].idx()].face.valid()) {
							this->reset();
							return false;
						}
						//Set vertex v1 if its halfedge is not set
						if (!this->vertexInfo[v1.idx()].halfedge.valid())
							this->vertexInfo[v1.idx()].halfedge = faceHalfedges[hi];
						//Set halfedge hi
						this->halfedgeInfo[faceHalfedges[hi].idx()].next = faceHalfedges[(hi + 1) % f.size()];
						this->halfedgeInfo[faceHalfedges[hi].idx()].prev = faceHalfedges[(hi - 1 + f.size()) % f.size()];
						this->halfedgeInfo[faceHalfedges[hi].idx()].target = v2;
						this->halfedgeInfo[faceHalfedges[hi].idx()].face = FaceIndex(fi);
						//Set the opposite halfedge's vertices
						HalfedgeIndex oppoHalfedge = this->halfedgeOppositeHalfedge(faceHalfedges[hi]);
						this->halfedgeInfo[oppoHalfedge.idx()].target = v1;
					}
					//set face fi if its halfedge is not set
					this->faceInfo[fi].halfedge = faceHalfedges[0];
				}
				return true;
			}

		private:

			struct VertexInfo {
				HalfedgeIndex halfedge;
				bool removed;
				VertexInfo(void) : halfedge(), removed(false) {}
			};
			struct HalfedgeInfo {
				HalfedgeIndex next;
				HalfedgeIndex prev;
				//There's no need to define `HalfedgeIndex oppo;`.
				VertexIndex target;
				FaceIndex face;
				bool removed;
				HalfedgeInfo(void) : next(), prev(), target(), face(), removed(false) {}
			};
			struct FaceInfo {
				HalfedgeIndex halfedge;
				bool removed;
				FaceInfo(void) : halfedge(), removed(false) {}
			};

			std::vector<VertexInfo> vertexInfo;
			std::vector<HalfedgeInfo> halfedgeInfo;
			std::vector<FaceInfo> faceInfo;
			//There's no need to define `std::vector<EdgeInfo> _edges;`.

		};

	}
}




#endif /* jjyou_geo_HalfedgeMesh_hpp */