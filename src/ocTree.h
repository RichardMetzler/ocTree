// Sorted alphabetically or by c system header -> c++ system header -> 3rd party header -> internal header
#include <array>    // For std::array (use std::array instead of vector whenver the size is fixed)
#include <glm/glm.hpp>
//#include <memory>    // For shared_ptr and unique_ptr
#include <mesh.h>
#include <ocTreeLeaf.h>
#include <string>
#include <utilities>    // For std::move
#include <vector>

// Header guards are fine, also "#pragma once" can be used since it is supported by most compilers.
#ifndef SRC_OCTREENODE_H_
#define SRC_OCTREENODE_H_

// Use spaces instead of tabs. You can set your editor that it expands tabs to spaces. (One tab is 4
// spaces). No need for indention in namespaces.
// Do one indent for private and public, otherwise your lines get too
// long, and it's a waste of space. Try to limit the line length to 100 characters (To be compatible
// with most screens). Use namespaces wherever you can to protect collisions with your methods and
// classes.
// Google C++ style guide is a good coding convention for C++

/**
* namespace for different data structures so it does not collide with data structures from different
* libraries in the future. This means the ocTree has to be called with data_struct::ocTree(..)
*/
namespace data_struct {

class ocTree {
 private:
    bool isNode;                                            // check if it is a node
    std::vector<glm::vec3> myVertices;						// vertices held by this node

    float minX, maxX, meanX;								// limits & mean in x dimension
    float minY, maxY, meanY;								// limits & mean in y dimension
    float minZ, maxZ, meanZ;								// limits & mean in z dimension

    std::vector<bool> identifier;							// bool vector describing the identifier of this node

    // Whenever you can use smart pointers instead of raw pointers (Raw pointers are pretty unsafe)
 // std::unique_ptr<ocTree> parent;							// Shared or uniquepointer to parent-ocTree

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
    * @PARAM std::array<float,9> parentDimensions - array of the parent nodes' dimensions, see order of values in dimensions above
    * @PARAM std::vector<bool> &identifierOld - identifier of the leaf that is being turned into a new subtree
    */
    // Whenever you have a parameter which is larger than a size_t, use a pass by reference.
    // Exception: When you need to make a copy of the object in the method, then pass by value.
    // See: Move constructor and copy constructor.
    ocTree(std::vector<glm::vec3> &vertices,
           int parentLevel,
           int vertsMax,
           int splitsMax,
           std::array<float, 9> &parentDimensions,    // Since the size is fixed use std::array
           std::vector<bool> &identifierOld);         // No need for a new line with );

    // Add missing constructors (c++ Rule of 5).
    // 1. Copy Constructor
    // 2. Copy Assignment Constructor
    // 3. Move Constructor
    // 4. Move Assignment Constructor
    // 5. Destructor
    // In this particular case the compiler gives you a
    // default one which is sufficient since you do not have pointers. As soon as you have a raw
    // pointer (I can see you want to use ocTree as a pointer in your tree, you need a copy
    // constructor to make a deep copy of the pointer (Copy the object pointed to). Otherwise if the
    // object it points to is deleted by another pointer, it's gone and your copied tree is pointed
    // to empty memory.

    /**
    * Copy constructor
    *
    * This constructor takes care of copying a tree when the constructor is called with another tree
    * object.
    *
    * @PARAM ocTree& Object to be copied.
    */
    // Use parameter initialization list.
    ocTree(const ocTree& other) : vertices(other.vertices),
                                 parentLevel(other.parentLevel),
                                 vertsMax(other.vertsMax),
                                 splitsMax(other.splitsMax),
                                 parentDimensions(other.parentDimensions),
                                 identifierOld(other.identifierOld) { }

    /**
    * Move constructor
    *
    * @PARAM ocTree&& Object to be moved.
    */
    // Assign the values and move the structures.
    ocTree(ocTree&& other) : vertices(std::move(other.vertices)),
                             parentLevel(other.parentLevel),
                             vertsMax(other.vertsMax),
                             splitsMax(other.splitsMax),
                             parentDimensions(std::move(other.parentDimensions)),
                             identifierOld(std::move(other.identifierOld)) {
        // Reset the other resources, std::move does that already.
        other.parentLevel = 0;
        other.vertsMax    = 0;
        other.splitsMax   = 0;
    }

    /**
    * Copy assignment operator
    *
    * This constructor takes care of copying a tree when the constructor is called with another tree
    * object.
    *
    * @PARAM ocTree& Reference to ocTree object to be copied.
    */
    ocTree& operator=(ocTree& other) {
        vertices = other.vertices;
        parentLevel = other.parentLevel;
        vertsMax = other.vertsMax;
        splitsMax = other.splitsMax;
        parentDimensions = other.parentDimensions;
        identifierOld = other.identifierOld;

        return *this;
    }

    /**
    * Move assignment operator
    *
    * This constructor takes care of copying a tree when the constructor is called with another tree
    * object.
    *
    * @PARAM ocTree&& ocTree object to be moved.
    * @RETURN ocTree& returns a reference to the object.
    */
    ocTree& operator=(ocTree&& other) {
        vertices = std::move(other.vertices);
        parentLevel = other.parentLevel;
        vertsMax = other.vertsMax;
        splitsMax = other.splitsMax;
        parentDimensions = std::move(other.parentDimensions);
        identifierOld = std::move(other.identifierOld);

        // Reset the other resources, std::move does that already.
        other.parentLevel = 0;
        other.vertsMax    = 0;
        other.splitsMax   = 0;

        return *this;
    }

    /**
    * DESTRUCTOR
    */
    ~ocTree();

    /**
    * determine whether this is a node or not
    */
    bool getIsNode();
};
} // End of namespace data_struct

#endif /* SRC_OCTREENODE_H_ */
