#include <ocTreeLeaf.h>

/**
 * root constructor
 */
ocTreeLeaf::ocTreeLeaf(std::vector<Mesh*> &meshesPointer, int maximumVertsPerLeaf, int maximumSplitDepth)
{
	std::cout << "root constructor called" << std::endl;
	myMeshes = meshesPointer;
	maxVerticesPerNode = maximumVertsPerLeaf;
	maxSplitDepth = maximumSplitDepth;
	level = 0;

	// initialize identifier
	std::vector<bool> id(maxSplitDepth*3);

	for (int i = 0; i <= id.size(); i++) {
		id[i] = false;
	}

	for (int i = 0; i <= id.size(); i++) {
		if (id[i] == false) {
//			std::cout << "root id indeed false" << std::endl;
		}
	}
	identifier = id;

	// calculate and set dimensions
	getRootDimensions();
	std::cout << "root identifier length: " << identifier.size() << std::endl;
}

/**
 * subTree constructor
 */
ocTreeLeaf::ocTreeLeaf(
		std::vector<glm::vec3> vertices,
		int parentLevel,
		int vertsMax,
		int splitsMax,
		float parentDimensions[],
		std::vector<bool> &parentIdentifier,
		std::vector<bool> &splitDirections)
{
	for(std::vector<bool>::iterator it = parentIdentifier.begin(); it != parentIdentifier.end(); ++it){
		if ((*it) == false) {
//			std::cout << "we got a false " << std::endl;	// funktioniert
		}
	}

	std::cout << "leaf constructor called" << std::endl;
	setDimensions(parentDimensions, splitDirections);
	maxVerticesPerNode = vertsMax;
	maxSplitDepth = vertsMax;
	level = parentLevel+1;

	// initialize identifier
	int identifierSize = parentIdentifier.size();
	std::vector<bool> id(identifierSize);
	for (int i = 0; i <= identifierSize; i++) {
		id[i] = parentIdentifier[i];
	}

	// set last 3 elements of identifier (splitDirections)
	id[identifierSize-1] = splitDirections[2];
	id[identifierSize-2] = splitDirections[1];
	id[identifierSize-3] = splitDirections[0];
	identifier = id;

	// debugging identifier as binary string for testing purposes
	debugIdentifierasString();

	/** BEWARE THE FOLLOWING LINE FOR IT BEARETH ONLY THE MOST UNSPEAKABLE OF HORRORS
	 * ITS NUMBER, UNHALLOWED AS THE NAME OF THE BEAST ITSELF, HAS EVEN THE MIGHTIEST OF SORCERES SHIVER FOR 'TIS WAS
	 * FORTOLD BY THE GREAT WIZARD OF AZAGTHOTH LONG BEFORE TIME, AS PUNY MORTALS HAVE TRIED FOR AEONS TO UNDERSTAND IT, EXISTED
	 */
//	 buildTreeRecursively(vertices);

}

// destructor
ocTreeLeaf::~ocTreeLeaf() {
	// @FIXME destroy everything
}

/**
 * get Root dimensions based on all vertices in the ocTreeLeaf
 */
void ocTreeLeaf::getRootDimensions(void) {
	minX = 0.;
	maxX = 0.;
	minY = 0.;
	maxY = 0.;
	minZ = 0.;
	maxZ = 0.;
	meanX = 0.;
	meanY = 0.;
	meanZ = 0.;

	std::vector<glm::vec3> vertices = getVerticesOfMesh(0);
		for (std::vector<glm::vec3>::iterator it = vertices.begin(); it != vertices.end(); ++it) {
			if ((*it).x < minX) {
				minX = (*it).x;
			}
			if ((*it).x > maxX) {
				maxX = (*it).x;
			}
			if ((*it).y < minY) {
				minY =(*it).y;
			}
			if ((*it).y > maxY) {
				maxY = (*it).y;
			}
			if ((*it).z < minZ) {
				minZ =(*it).z;
			}
			if ((*it).z > maxZ) {
				maxZ = (*it).z;
			}
		}
		meanX = (minX + maxX)/2;
		meanY = (minY + maxY)/2;
		meanZ = (minZ + maxZ)/2;
}

/**
 * setDimensions function for setting min, max and mean-values for x, y and z direction
 */
void ocTreeLeaf::setDimensions(float parentDimensions[], std::vector<bool> &splitDirections) {
	// splitDirectios::at(0)
	if (splitDirections.at(0) == false) {
		minZ = parentDimensions[8];	// minZ = p.meanZ
		maxZ = parentDimensions[7];	// maxZ = p.maxZ
	} else {
		minZ = parentDimensions[6];	// minZ = p.minZ
		maxZ = parentDimensions[8];	// maxZ = p.meanZ
	}

	// splitDirectios::at(1)
	if (splitDirections.at(1) == false) {
		minY = parentDimensions[5];	// minY = p.meanY
		maxY = parentDimensions[4];	// maxY = p.maxY
	} else {
		minY = parentDimensions[3];	// minY = p.minY
		maxY = parentDimensions[5];	// maxY = p.meanY
	}

	// splitDirectios::at(2)
	if (splitDirections.at(2) == false) {
		minX = parentDimensions[0];	// minX = p.minX
		maxX = parentDimensions[2];	// maxX = p.meanX
	} else {
		minX = parentDimensions[2];	// minX = p.meanX
		maxX = parentDimensions[1];	// maxX = p.maxX
	}

	// mean values:
	meanZ = (minZ+maxX)/2;
	meanY = (minY+maxY)/2;
	meanX = (minX+maxX)/2;
}

void ocTreeLeaf:: buildTreeRecursively (std::vector<glm::vec3> subsetVerts) {
	/**
	 * ###########################################################
	 * ######## BEGIN FOR-LOOP THROUGH ALL GIVEN VERTICES ########
	 * ###########################################################
	 */
	for(std::vector<glm::vec3>::iterator it = subsetVerts.begin(); it != subsetVerts.end(); ++it) {
		// if level is 0, include vertices that share a coordinate with the maximum values for x, y and z of this node
		if (level == 0) {
			if ((*it).x >= minX && (*it).x <= maxX) {
				if ((*it).y >= minY && (*it).y <= maxY) {
					if ((*it).z >= minZ && (*it).z <= maxZ) {
						verticesInBounds.push_back((*it));
					}
				}
			}
		} else {

			/**
			 * if the current not is on a level greater than 0, vertices that share x,y or z-values with
			 * the maximum values of the current node need to be treated differently
			 *
			 * whether a vertex with an 'extreme' value is assigned to a node or not is determined by the
			 * split directions, which can be retrieved via looking at the last, 2nd-to-last and 3rd-to-last
			 * values of a node's identifier.
			 */
			std::vector<bool>::iterator determineZ = identifier.end()--;
			std::vector<bool>::iterator determineY = determineZ--;
			std::vector<bool>::iterator determineX = determineY--;

			if (*determineZ == false) {									// 3rd-to-last	= false
				if (*determineY == false) {									// 2nd-to-last	= false
					if (*determineX == false) {									// identifier = 000
						if ((*it).x >= minX && (*it).x < maxX) {
							if ((*it).y >= minY && (*it).y < maxY) {
								if ((*it).z >= minZ && (*it).z <= maxZ) {
									verticesInBounds.push_back((*it));
								}
							}
						}
					} else {													// identifier = 001
						if ((*it).x >= minX && (*it).x <= maxX) {
							if ((*it).y >= minY && (*it).y < maxY) {
								if ((*it).z >= minZ && (*it).z <= maxZ) {
									verticesInBounds.push_back((*it));
								}
							}
						}
					}
				} else {
					if (*determineX == false) {									// identifier = 010
						if ((*it).x >= minX && (*it).x < maxX) {
							if ((*it).y >= minY && (*it).y <= maxY) {
								if ((*it).z >= minZ && (*it).z <= maxZ) {
									verticesInBounds.push_back((*it));
								}
							}
						}
					} else {													// identifier = 011
						if ((*it).x >= minX && (*it).x <= maxX) {
							if ((*it).y >= minY && (*it).y <= maxY) {
								if ((*it).z >= minZ && (*it).z <= maxZ) {
									verticesInBounds.push_back((*it));
								}
							}
						}
					}
				}
			} else {													// 3rd-to-last = true
				if (*determineY == false) {									// 2nd-to-last = false
					if (*determineX == false) {									// identifier = 100
						if ((*it).x >= minX && (*it).x < maxX) {
							if ((*it).y >= minY && (*it).y < maxY) {
								if ((*it).z >= minZ && (*it).z < maxZ) {
									verticesInBounds.push_back((*it));
								}
							}
						}
					} else {													// identifier = 101
						if ((*it).x >= minX && (*it).x <= maxX) {
							if ((*it).y >= minY && (*it).y < maxY) {
								if ((*it).z >= minZ && (*it).z < maxZ) {
									verticesInBounds.push_back((*it));
								}
							}
						}
					}
				} else {
					if (*determineX == false) {									// identifier = 110
						if ((*it).x >= minX && (*it).x < maxX) {
							if ((*it).y >= minY && (*it).y <= maxY) {
								if ((*it).z >= minZ && (*it).z < maxZ) {
									verticesInBounds.push_back((*it));
								}
							}
						}
					} else {													// identifier = 111
						if ((*it).x >= minX && (*it).x <= maxX) {
							if ((*it).y >= minY && (*it).y <= maxY) {
								if ((*it).z >= minZ && (*it).z < maxZ) {
									verticesInBounds.push_back((*it));
								}
							}
						}
					}
				}
			}
		}
	}
	/**
	 * #########################################################
	 * ######## END FOR-LOOP THROUGH ALL GIVEN VERTICES ########
	 * #########################################################
	 */
	if (verticesInBounds.size() > maxVerticesPerNode) {
		if (level <= maxSplitDepth) {
			split();
		} else {
			// @FIXME do something
		}
	}
}

bool ocTreeLeaf::split() {
	float parDimensions[9] = {minX, maxX, meanX, minY, maxY, maxY, minZ, maxZ, meanZ};
	std::vector<bool> split0 = {false,	false,	false};
	std::vector<bool> split1 = {false,	false,	true};
	std::vector<bool> split2 = {false,	true,	false};
	std::vector<bool> split3 = {false,	true,	true};
	std::vector<bool> split4 = {true,	false,	false};
	std::vector<bool> split5 = {true,	false,	true};
	std::vector<bool> split6 = {true,	true,	false};
	std::vector<bool> split7 = {true,	true,	true};

	// turning the leaf into a node
	ocTree * newOcTreee = new ocTree(
		verticesInBounds,
		level,
		maxVerticesPerNode,
		maxSplitDepth,
		parDimensions,
		identifier
	);

	ocTreeLeaf* chidlLeaf0 = new ocTreeLeaf(verticesInBounds, level, maxVerticesPerNode, maxSplitDepth, parDimensions, identifier, split0);
	ocTreeLeaf* chidlLeaf1 = new ocTreeLeaf(verticesInBounds, level, maxVerticesPerNode, maxSplitDepth, parDimensions, identifier, split1);
	ocTreeLeaf* chidlLeaf2 = new ocTreeLeaf(verticesInBounds, level, maxVerticesPerNode, maxSplitDepth, parDimensions, identifier, split2);
	ocTreeLeaf* chidlLeaf3 = new ocTreeLeaf(verticesInBounds, level, maxVerticesPerNode, maxSplitDepth, parDimensions, identifier, split3);
	ocTreeLeaf* chidlLeaf4 = new ocTreeLeaf(verticesInBounds, level, maxVerticesPerNode, maxSplitDepth, parDimensions, identifier, split4);
	ocTreeLeaf* chidlLeaf5 = new ocTreeLeaf(verticesInBounds, level, maxVerticesPerNode, maxSplitDepth, parDimensions, identifier, split5);
	ocTreeLeaf* chidlLeaf6 = new ocTreeLeaf(verticesInBounds, level, maxVerticesPerNode, maxSplitDepth, parDimensions, identifier, split6);
	ocTreeLeaf* chidlLeaf7 = new ocTreeLeaf(verticesInBounds, level, maxVerticesPerNode, maxSplitDepth, parDimensions, identifier, split7);

	turnIntoNode();
	// ~ocTreeLeaf();
	return true;
}

bool  ocTreeLeaf::turnIntoNode() {
//	myVertices.swap(verticesInBounds);
	return true;
}

std::vector<Mesh*> ocTreeLeaf::getMyMeshes (void) {
	return myMeshes;
}

std::vector<Mesh*>::iterator ocTreeLeaf::getMesh (int n) {
	int count = 0;
	for(std::vector<Mesh*>::iterator it = myMeshes.begin(); it != myMeshes.end(); ++it) {
		if (count == n) {
			return(it);
		}
	}
}

std::vector <glm::vec3> ocTreeLeaf::getVertices() {
	std::vector<glm::vec3> result;
	for(std::vector<Mesh*>::iterator it = myMeshes.begin(); it != myMeshes.end(); ++it) {
		std::vector<glm::vec3> itVerts = (*it)->get_m_vertices(); 		// get vertices of current Mesh
		result.insert(result.end(), itVerts.begin(), itVerts.end());	// insert vertices into result vector
	}
	return result;
}

std::vector<glm::vec3> ocTreeLeaf::getVerticesOfMesh (int n) {
	int count = 0;
	for(std::vector<Mesh*>::iterator it = myMeshes.begin(); it != myMeshes.end(); ++it) {
		if (count == n) {
			return(*it)->get_m_vertices();
		}
	}
}

// @FIXME: keep this up to date!
void ocTreeLeaf::debugTreeInfo(void) {
	std::cout << "==========[ocTreeLeaf INFO]==========" << std::endl;
	std::cout << "total number of vertices in this ocTreeLeaf:" << (*getMesh(0))->getNumVertices() << std::endl;
	std::cout << "maximum number of vertices per leaf node: " << maxVerticesPerNode << std::endl;
	std::cout << "maximum split depth: " << maxSplitDepth << std::endl;
	std::cout << "dimension x: " << minX << " to " << maxX << ", mean value: " << meanX << std::endl;
	std::cout << "dimension y: " << minY << " to " << maxY << ", mean value: " << meanY << std::endl;
	std::cout << "dimension z: " << minZ << " to " << maxZ << ", mean value: " << meanZ << std::endl;
	std::cout << "total number of nodes: [@FIXME]" << std::endl;
	std::cout << "total number of leafs: [@FIXME]" << std::endl;
	std::cout << "average number of vertices / leaf: [@FIXME]" << std::endl;
	std::cout << "highest number of vertices in one leaf: [@FIXME]" << std::endl;
	std::cout << "lowest number of vertices in one leaf: [@FIXME]" << std::endl;
	std::cout << "========[ocTreeLeaf INFO END]========" << std::endl;

}

void ocTreeLeaf::debugFirstVertex(void) {
	std::vector<Mesh*>::iterator firstMesh = getMesh(0);
	std::vector<glm::vec3> vertices = (*firstMesh)->get_m_vertices();
	std::cout << "First vertex(x,y,z): " << vertices[0].x <<", "<< vertices[0].y <<", "<< vertices[0].z <<", "<< std::endl;
}

void ocTreeLeaf::debugAllVertices(void) {
	for(std::vector<Mesh*>::iterator it = myMeshes.begin(); it != myMeshes.end(); ++it) {
		std::vector<glm::vec3> vertices = (*it)->get_m_vertices();
		for (int i=0; i <= (*it)->getNumVertices(); i++) {
			std::cout << "x, y, z of vertex #" << i <<": "
			<< vertices[i].x <<", "
			<< vertices[i].y <<", "
			<< vertices[i].z
			<< std::endl;
		}
	}
}

void ocTreeLeaf::debugIdentifierasString(void) {
	int identifierSize = identifier.size();
	std::cout << "my Identifier: ";
	for (int i = 0; i < identifierSize; i++) {
		if (identifier[i] == false) {
			std::cout << "0";
		} else {
			std::cout << "1";
		}
	}
	std::cout << std::endl;
}
