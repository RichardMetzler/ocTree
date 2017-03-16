#include "ocTreeLeaf.h"
#include <iostream>
#include <array>


/**
 * constructor
 */
ocTree::ocTree (
		std::vector<glm::vec3> vertices,
		int parentLevel,
		int vertsMax,
		int splitsMax,
		float parentDimensions[],
		std::vector<bool> &identifierOld
	)
{
	isNode = false;
	myVertices = vertices;

	minX	= parentDimensions[0];
	maxX	= parentDimensions[1];
	meanX	= parentDimensions[2];
	minY	= parentDimensions[3];
	maxY	= parentDimensions[4];
	meanY	= parentDimensions[5];
	minZ	= parentDimensions[6];
	maxZ	= parentDimensions[7];
	meanZ	= parentDimensions[8];
}

// destructor
ocTree::~ocTree() {
	// @FIXME destroy everything
}

// getter for boolean attribute isNode
bool ocTree::getIsNode() {
	return isNode;
}

//bool Leaf::split(std::vector<Mesh*> meshes, bool parentName[], bool splitDirections[]) {
//
//
//	return true;
//}
