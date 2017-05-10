#include <ocTreeLeaf.h>

/**
 * root constructor
 */
ocTreeLeaf::ocTreeLeaf(std::vector<Mesh*> &meshesPointer, int maximumVertsPerLeaf, int maximumSplitDepth)
{
	isLeaf = true;
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

	identifier = id;

	// calculate and set dimensions
	getRootDimensions();
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
		std::vector<bool> &parId,
		std::vector<bool> &splitDirections)
{
	isLeaf = true;
	setDimensions(parentDimensions, splitDirections);
	maxVerticesPerNode = vertsMax;
	maxSplitDepth = splitsMax;
	level = parentLevel+1;
	parentIdentifier = parId;

	// initialize identifier
	int identifierSize = parId.size();
	int levelOffset = level*3-3;
	std::vector<bool> id(identifierSize);
	std::cout << "level: " << level << ", identifierBuilt: ";
	for (int i = 0; i < levelOffset; i++) {
		  id[i] = parId[i];
		  if (id[i] == true) {
			  std::cout << "1";
		  } else {
			  std::cout << "0";
		  }
	}

	// set the latest values of the identifier, the given splitDirections
	id[levelOffset] = splitDirections[0];
		if (id[levelOffset] == true) {
		  std::cout << "1";
		} else {
		  std::cout << "0";
		}
	id[levelOffset+1] = splitDirections[1];
		if (id[levelOffset+1] == true) {
		  std::cout << "1";
		} else {
		  std::cout << "0";
		}
	id[levelOffset+2] = splitDirections[2];
		if (id[levelOffset+2] == true) {
		  std::cout << "1";
		} else {
		  std::cout << "0";
		}
	for (int j = levelOffset+3; j < identifierSize; j++) {
		id[j] = false;
		if (id[j] == false) {
			std::cout << "0";
		}
		if (id[j] == true) {
			std::cout << "1";
		}
	}
	std::cout<<std::endl;
	identifier = id;

	// debugging identifier as binary string for testing purposes
	// debugIdentifierasString();

	// next line is very important
	buildTreeRecursively(vertices);
}

// destructor
ocTreeLeaf::~ocTreeLeaf() {
	//  @FIXME do something
}

// getter for boolean attribute isLeaf
bool ocTreeLeaf::getisLeaf() {
	return isLeaf;
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
		meanX = (float)(minX + maxX)/2;
		meanY = (float)(minY + maxY)/2;
		meanZ = (float)(minZ + maxZ)/2;
}

/**
 * setDimensions function for setting min, max and mean-values for x, y and z direction
 *
 * parDimensions: {[0]minX, [1]maxX, [2]meanX, [3]minY, [4]maxY, [5]meanY, [6]minZ, [7]maxZ, [8]meanZ};
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
	meanZ = (minZ+maxZ)/2;
	meanY = (minY+maxY)/2;
	meanX = (minX+maxX)/2;
}

void ocTreeLeaf:: buildTreeRecursively (std::vector<glm::vec3> subsetVerts) {
	/**
	 * ###########################################################
	 * ######## BEGIN FOR-LOOP THROUGH ALL GIVEN VERTICES ########
	 * ###########################################################
	 */
	// @FIXME delete
	// std::cout << "number of vertices passed: " << subsetVerts.size() << std::endl;
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

//			std::vector<bool>::iterator determineZ = identifier.end()--;
//			std::vector<bool>::iterator determineY = determineZ--;
//			std::vector<bool>::iterator determineX = determineY--;

			bool determineZ = identifier[level];
			bool determineY = identifier[level+1];
			bool determineX = identifier[level+2];

			if (determineZ == false) {									// 3rd-to-last	= false
				if (determineY == false) {									// 2nd-to-last	= false
					if (determineX == false) {									// identifier = 000
						if ((*it).x >= minX && (*it).x <= maxX) {
							if ((*it).y > minY && (*it).y <= maxY) {
								if ((*it).z > minZ && (*it).z <= maxZ) {
									verticesInBounds.push_back((*it));
									if ((*it).x==minX||(*it).x==maxX||(*it).y==minY||(*it).y==maxY||(*it).z==minZ||(*it).z==maxZ){
										// std::cout << "DELETING EDGECASE" << std::endl;
										subsetVerts.erase(it);
									}
								}
							}
						}
					} else {													// identifier = 001
						if ((*it).x > minX && (*it).x <= maxX) {
							if ((*it).y >= minY && (*it).y <= maxY) {
								if ((*it).z > minZ && (*it).z <= maxZ) {
							 		verticesInBounds.push_back((*it));
							 		if ((*it).x==minX||(*it).x==maxX||(*it).y==minY||(*it).y==maxY||(*it).z==minZ||(*it).z==maxZ){
							 			// std::cout << "DELETING EDGECASE" << std::endl;
							 			subsetVerts.erase(it);
									}
								}
							}
						}
					}
				} else {
					if (determineX == false) {									// identifier = 010
						if ((*it).x >= minX && (*it).x <= maxX) {
							if ((*it).y >= minY && (*it).y <= maxY) {
								if ((*it).z > minZ && (*it).z <= maxZ) {
									verticesInBounds.push_back((*it));
									if ((*it).x==minX||(*it).x==maxX||(*it).y==minY||(*it).y==maxY||(*it).z==minZ||(*it).z==maxZ){
										// std::cout << "DELETING EDGECASE" << std::endl;
										subsetVerts.erase(it);
									}
								}
							}
						}
					} else {													// identifier = 011
						if ((*it).x > minX && (*it).x <= maxX) {
							if ((*it).y >= minY && (*it).y <= maxY) {
								if ((*it).z > minZ && (*it).z <= maxZ) {
									verticesInBounds.push_back((*it));
									if ((*it).x==minX||(*it).x==maxX||(*it).y==minY||(*it).y==maxY||(*it).z==minZ||(*it).z==maxZ){
										// std::cout << "DELETING EDGECASE" << std::endl;
										subsetVerts.erase(it);
									}
								}
							}
						}
					}
				}
			} else {													// 3rd-to-last = true
				if (determineY == false) {									// 2nd-to-last = false
					if (determineX == false) {									// identifier = 100
						if ((*it).x >= minX && (*it).x <= maxX) {
							if ((*it).y > minY && (*it).y <= maxY) {
								if ((*it).z >= minZ && (*it).z <= maxZ) {
									verticesInBounds.push_back((*it));
									if ((*it).x==minX||(*it).x==maxX||(*it).y==minY||(*it).y==maxY||(*it).z==minZ||(*it).z==maxZ){
										// std::cout << "DELETING EDGECASE" << std::endl;
										subsetVerts.erase(it);
									}
								}
							}
						}
					} else {													// identifier = 101
						if ((*it).x > minX && (*it).x <= maxX) {
							if ((*it).y >= minY && (*it).y <= maxY) {
								if ((*it).z >= minZ && (*it).z <= maxZ) {
									verticesInBounds.push_back((*it));
									if ((*it).x==minX||(*it).x==maxX||(*it).y==minY||(*it).y==maxY||(*it).z==minZ||(*it).z==maxZ){
										// std::cout << "DELETING EDGECASE" << std::endl;
										subsetVerts.erase(it);
									}
								}
							}
						}
					}
				} else {
					if (determineX == false) {									// identifier = 110
						if ((*it).x >= minX && (*it).x <= maxX) {
							if ((*it).y >= minY && (*it).y <= maxY) {
								if ((*it).z >= minZ && (*it).z <= maxZ) {
									verticesInBounds.push_back((*it));
									if ((*it).x==minX||(*it).x==maxX||(*it).y==minY||(*it).y==maxY||(*it).z==minZ||(*it).z==maxZ){
										// std::cout << "DELETING EDGECASE" << std::endl;
										subsetVerts.erase(it);
									}
								}
							}
						}
					} else {													// identifier = 111
						if ((*it).x > minX && (*it).x <= maxX) {
							if ((*it).y >= minY && (*it).y <= maxY) {
								if ((*it).z >= minZ && (*it).z <= maxZ) {
									verticesInBounds.push_back((*it));
									if ((*it).x==minX||(*it).x==maxX||(*it).y==minY||(*it).y==maxY||(*it).z==minZ||(*it).z==maxZ){
										// std::cout << "DELETING EDGECASE" << std::endl;
										subsetVerts.erase(it);
									}
								}
							}
						}
					}
				}
			}
		}
	}
	std::cout << "I contain " << verticesInBounds.size() << " vertices." << std::endl;

	/**
	 * #########################################################
	 * ######## END FOR-LOOP THROUGH ALL GIVEN VERTICES ########
	 * #########################################################
	 */
	if (verticesInBounds.size() > maxVerticesPerNode) {
		if (level < maxSplitDepth) {
			std::cout << "!!! WILL BE DELETED !!!" << std::endl;
			split();
		} else {
			// @FIXME do something
		}
	}
	std::vector<bool> check = {false,false,true,false,false,false,false,false,false};	// 001000000
	getNodeByIdentifierArray(check, true);
}

bool ocTreeLeaf::split() {
	std::cout << "split() called" << std::endl;
	float parDimensions[9] = {minX, maxX, meanX, minY, maxY, meanY, minZ, maxZ, meanZ};
	std::vector<bool> split0 = {false,	false,	false};		// 000
	std::vector<bool> split1 = {false,	false,	true};		// 001
	std::vector<bool> split2 = {false,	true,	false};		// 010
	std::vector<bool> split3 = {false,	true,	true};		// 011
	std::vector<bool> split4 = {true,	false,	false};		// 100
	std::vector<bool> split5 = {true,	false,	true};		// 101
	std::vector<bool> split6 = {true,	true,	false};		// 110
	std::vector<bool> split7 = {true,	true,	true};		// 111

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

//	~ocTreeLeaf();
	return true;
}

bool  ocTreeLeaf::turnIntoNode() {
	isLeaf = false;
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
	if (isLeaf == true) {
		int identifierSize = identifier.size();
		std::cout << "LEAF:: my level: " << level << std::endl;
		std::cout << "parentIdentir: ";
		for (int i = 0; i < identifierSize; i++) {
			if (parentIdentifier[i] == false) {
				std::cout << "0";
			} else {
				std::cout << "1";
			}
		}
		std::cout << std::endl;
		std::cout << "my identifier: ";
		for (int i = 0; i < identifierSize; i++) {
			if (identifier[i] == false) {
				std::cout << "0";
			} else {
				std::cout << "1";
			}
		}
		std::cout << std::endl;
	}
}

ocTreeLeaf* ocTreeLeaf::getNodeByIdentifierArray(std::vector<bool> compare, bool debugInfo) {
	if (this->getisLeaf() == false) {	// current node is not a leaf -> skip
		return NULL;
	} else {
		int checksum = 0;
		for (int i = 0; i != 15; i++) {
			if (identifier[i] == compare[i]) {
				checksum++;
			}
		}
		if (checksum == 15) {
			if (debugInfo == true) {
				debugIdentifierasString();
				std::cout << "leaf found!" << std::endl;
				std::cout << "minX: " << minX << ", meanX: " << meanX << ",maxX: " << maxX << std::endl;
				std::cout << "minY: " << minY << ", meanY: " << meanY << ",maxY: " << maxY << std::endl;
				std::cout << "minZ: " << minZ << ", meanZ: " << meanZ << ",maxZ: " << maxZ << std::endl;
			}
			ocTreeLeaf *result = new ocTreeLeaf(*this);
			return result;
		}
	}
}

ocTreeLeaf* ocTreeLeaf::getNodeByCoordinates(float x, float y, float z, bool debugInfo) {
	// if one of the coordinates is less than minimum / higher than maximum values of this, return NULL
	if (x < minX || x > maxX || y < minY || y > maxY || z < minZ || z > maxZ) {
		return NULL;
	}

	if (x < meanX) {				//

	} else {

	}

	ocTreeLeaf *result = new ocTreeLeaf(*this);
	return result;
}
