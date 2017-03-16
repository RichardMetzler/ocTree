#include <glm/glm.hpp>
#include <mesh.h>
#include <string>
#include <vector>
#include <ocTreeLeaf.h>

#ifndef SRC_OCTREENODE_H_
#define SRC_OCTREENODE_H_


class ocTree {

	private:
		bool isNode;
		std::vector<glm::vec3> myVertices;						// vertices held by this node

		float minX, maxX, meanX;								// limits & mean in x dimension
		float minY, maxY, meanY;								// limits & mean in y dimension
		float minZ, maxZ, meanZ;								// limits & mean in z dimension

		std::vector<bool> identifier;							// bool vector describing the identifier of this node
//		ocTree* parent;											// pointer to parent-ocTree

	public:

		/**
		 * CONSTRUCTOR
		 *
		 * order of values in dimensions:
		 * dimensions[0] - minX, dimensions[1] - maxX, dimensions[2] - meanX
		 * dimensions[3] - minY, dimensions[4] - maxY, dimensions[5] - meanY
		 * dimensions[6] - minZ, dimensions[7] - maxZ, dimensions[8] - meanZ
		 *
		 * @PARAM std::vector<glm::vec3> vertices - vertices contained in the bounds of the parent node
		 * @PARAM int parentLevel - level of the parent node
		 * @PARAM int vertxMax - maximum number of vertices a leaf can hold
		 * @PARAM int splitsMax - maximum split-depth
		 * @PARAM float parentDimensions[] - array of the parent nodes' dimensions, see order of values in dimensions above
		 * @PARAM std::vector<bool> &identifierOld - identifier of the leaf that is being turned into a new subtree
		 */
		ocTree(
			std::vector<glm::vec3> vertices,
			int parentLevel,
			int vertsMax,
			int splitsMax,
			float parentDimensions[],
			std::vector<bool> &identifierOld
		);

		/**
		 * DESTRUCTOR
		 */
		~ocTree();

		/**
		 * determine whether this is a node or not
		 */
		bool getIsNode();
};


#endif /* SRC_OCTREENODE_H_ */
