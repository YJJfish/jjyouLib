/***********************************************************************
 * @file	IndexedMesh.hpp
 * @author	jjyou
 * @date	2024-1-7
 * @brief	This file implements IndexedMesh class.
***********************************************************************/
#ifndef jjyou_geo_IndexedMesh_hpp
#define jjyou_geo_IndexedMesh_hpp

namespace jjyou {
	namespace geo {

		/***********************************************************************
		 * @class IndexedMesh
		 * @brief Indexed data structure of a mesh.
		 *
		 * This class stores meshes in indexed data structure. Different from
		 * jjyou::geo::HalfedgeMesh, this data structure is relatively simpler.
		 * It does not provide mesh modification functions. To modify the mesh,
		 * you need to directly modify its member variables.
		 *
		 * @sa			jjyou::geo::HalfedgeMesh
		 ***********************************************************************/
		template <class FP = float>
		class IndexedMesh {

			/*============================================================
			 *                    Forward declarations
			 *============================================================*/
		public:
			using Vec3 = Eigen::Vector<FP, 3>;
			using Vec2 = Eigen::Vector<FP, 2>;
			/*============================================================
			 *                 End of forward declarations
			 *============================================================*/

		public:

			class Vertex {

			public:

				/** @brief Position of the vertex.
				  */
				Vec3 position;

				/** @brief Default constructor.
				  */
				Vertex(void) : position(Vec3::Zero()) {}

				/** @brief Construct with position.
				  */
				Vertex(const Vec3& position) : position(position) {}

				/** @brief Helper printing function.
				  */
				friend std::ostream& operator<<(std::ostream& out, const Vertex& v) {
					out << "v(" << v.position.x << ", " << v.position.y << ", " << v.position.z << ")";
					return out;
				}

			private:

				friend class IndexedMesh;

			};

			struct Corner {

				std::uint32_t vIdx;

				Vec2 uv;

				Vec3 normal;

				Corner(void) : vIdx(0U), uv(Vec2::Zero()), normal(Vec3::Zero()) {}

				Corner(std::uint32_t vIdx) : vIdx(vIdx), uv(Vec2::Zero()), normal(Vec3::Zero()) {}

				Corner(std::uint32_t vIdx, const Vec2& uv, const Vec3& normal) : vIdx(vIdx), uv(uv), normal(normal) {}

			};

			class Face {

			public:

				/** @brief List of vertices, uv coordinates, and normals.
				  */
				std::vector<Corner> corners;

				/** @brief Default constructor.
				  */
				Face(void) : corners() {}

			private:

				friend class IndexedMesh;

			};

		};

	}
}

#endif /* jjyou_geo_IndexedMesh_hpp */