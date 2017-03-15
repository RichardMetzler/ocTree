#include "mesh.h"

Mesh::Mesh(const bool &debug) : m_debug(debug)
{

}

// destructor
Mesh::~Mesh()
{
    //delete buffers
    if(m_vertexID)
        glDeleteBuffers(1,&m_vertexID);
    if(m_colorsID)
        glDeleteBuffers(1,&m_colorsID);
    if(m_normalID)
        glDeleteBuffers(1,&m_normalID);
    if(m_indicesID)
        glDeleteBuffers(1,&m_indicesID);
    for(std::vector<GLuint>::iterator it = m_tcoordIDs.begin(); it != m_tcoordIDs.end(); ++it){
        glDeleteBuffers(1,&(*it));
    }
    //texture buffers are not create here and consequently must be deleted elsewhere!

}

bool Mesh::saysomethingmeshy(void) {
	std::cout << "Mesh.cpp says hello" << std::endl;
	return true;
}

std::vector<glm::vec3> Mesh::get_m_vertices(void) {
	return m_vertices;
}

int Mesh::addTexture(const GLuint &id, const std::vector<glm::vec2> tcoords)
{
    if(tcoords.size() != m_vertices.size())
        return -1;

    int retValue = m_tCoords.size(); //new position in vector after push_back
    m_textureIDs.push_back(id);
    m_tCoords.push_back(tcoords);
    return retValue;
}

int Mesh::addTexture(const GLuint &id)
{
    int retValue = m_textureIDs.size(); //new position in vector after push_back
    m_textureIDs.push_back(id);

    return retValue;
}

bool Mesh::replaceTexture(const GLuint &id, const unsigned int &position)
{
    if(position >= m_textureIDs.size())
        return false;
    m_textureIDs[position] = id;
    return true;
}

bool Mesh::loadMesh(const aiMesh* mesh)
{
    m_vertices.resize(mesh->mNumVertices);
    float Xmax = 0., Xmin = 0., Ymax = 0., Ymin = 0., Zmax = 0., Zmin = 0.;

    for(int i = 0; i < mesh->mNumVertices; ++i) {
        m_vertices[i][0] = mesh->mVertices[i].x;
        m_vertices[i][1] = mesh->mVertices[i].y;
        m_vertices[i][2] = mesh->mVertices[i].z;

        if (m_vertices[i][0] < Xmin) {
        	Xmin = m_vertices[i][0];
        }
        if (m_vertices[i][0] > Xmax) {
        	Xmax = m_vertices[i][0];
        }
        if (m_vertices[i][1] < Ymin) {
			Ymin = m_vertices[i][1];
		}
        if (m_vertices[i][1] > Ymax) {
			Ymax = m_vertices[i][1];
		}
        if (m_vertices[i][2] < Zmin) {
			Zmin = m_vertices[i][2];
		}
        if (m_vertices[i][2] > Zmax) {
        	Zmax = m_vertices[i][2];
		}
    }

    if(mesh->HasTextureCoords(0)) { //check if first set of texture coordinates is available
        bool hasCoords = true;
        int curCoord = 0;
        while(hasCoords){
            if(mesh->HasTextureCoords(curCoord)) {
                std::vector<glm::vec2 > tmpvector;
                tmpvector.resize(mesh->mNumVertices);
                for(int i = 0; i < mesh->mNumVertices; ++i) {
                    tmpvector[i][0]= mesh->mTextureCoords[curCoord][i].x;
                    tmpvector[i][1] = mesh->mTextureCoords[curCoord][i].y;
                }
                ++curCoord;
                m_tCoords.push_back(tmpvector); //slow but sufficient

            } else {
                hasCoords = false;
            }
        }
    }

    if(mesh->HasNormals()){
        m_normals.resize(mesh->mNumVertices);
        for(int i = 0; i < mesh->mNumVertices; ++i) {
            m_normals[i][0] = mesh->mNormals[i].x;
            m_normals[i][1] = mesh->mNormals[i].y;
            m_normals[i][2] = mesh->mNormals[i].z;
        }
    }

//TODO: we only support one color channel (multiple need custom shaders)
    if(mesh->HasVertexColors(0)){
        m_colors.resize(mesh->mNumVertices);
        for(int i = 0; i < mesh->mNumVertices; ++i) {
            m_colors[i][0] = mesh->mColors[0][i].r;
            m_colors[i][1] = mesh->mColors[0][i].g;
            m_colors[i][2] = mesh->mColors[0][i].b;
            m_colors[i][3] = mesh->mColors[0][i].a;

        }
    }

    if(mesh->HasFaces()) {
        //only works if each face has same number of indices (otherwise it's a little "strange")
        m_indices.resize(mesh->mNumFaces * mesh->mFaces[0].mNumIndices);
        for(int i = 0; i < mesh->mNumFaces; ++i) {
            for(int j = 0; j < mesh->mFaces[i].mNumIndices; ++j){
                m_indices[i * mesh->mFaces[i].mNumIndices + j] = mesh->mFaces[i].mIndices[j];
            }
        }
        //check which type to draw
        switch(mesh->mFaces[0].mNumIndices) {
            case 1: m_drawMode = (GL_POINTS);break;
            case 2: m_drawMode = (GL_LINES);break;
            case 3: m_drawMode = (GL_TRIANGLES);break;
            case 4: m_drawMode = (GL_QUADS);break;
            default: m_drawMode = (GL_POLYGON);break;
        }
    }
    return true;
}

bool Mesh::setMaterialandTextures(const aiMaterial* mat, const std::map<std::string, GLuint>& textureIdMap, const std::string path)
{
    int texIndex = 0;
    aiReturn texFound;
    aiString texPath;	// filename

    texFound = mat->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath);

    if(texFound == AI_SUCCESS){
        while (texFound == AI_SUCCESS){
            std::stringstream absPath; absPath<<path<<texPath.C_Str();
            m_textureIDs.push_back(textureIdMap.at(absPath.str()));
            ++texIndex;
            texFound = mat->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath);

        }
    }

    aiColor4D tmpcolor;
    float shininess, strength;
    int wireframe;
    unsigned int max;

    if(AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &tmpcolor)){
        m_material.diffuse[0] = tmpcolor.r;
        m_material.diffuse[1] = tmpcolor.g;
        m_material.diffuse[2] = tmpcolor.b;
        m_material.diffuse[3] = tmpcolor.a;
    }
    if(AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &tmpcolor)){
        m_material.ambient[0] = tmpcolor.r;
        m_material.ambient[1] = tmpcolor.g;
        m_material.ambient[2] = tmpcolor.b;
        m_material.ambient[3] = tmpcolor.a;
    }
    if(AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &tmpcolor)){
        m_material.specular[0] = tmpcolor.r;
        m_material.specular[1] = tmpcolor.g;
        m_material.specular[2] = tmpcolor.b;
        m_material.specular[3] = tmpcolor.a;
    }
    if(AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, &tmpcolor)){
        m_material.emission[0] = tmpcolor.r;
        m_material.emission[1] = tmpcolor.g;
        m_material.emission[2] = tmpcolor.b;
        m_material.emission[3] = tmpcolor.a;
    }
    unsigned int ret1,ret2;
    max = 1;
    ret1 = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS, &shininess, &max);
    max = 1;
    ret2 = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
    if((ret1 == AI_SUCCESS) && (ret2 == AI_SUCCESS)){
        m_material.shininess = shininess;
        m_material.shininess_strength = strength;
    } else {
        m_material.shininess = 0.;
        m_material.specular[0] = 0.;m_material.specular[1] = 0.;m_material.specular[2] = 0.;m_material.specular[3] = 0.;
    }
    if(AI_SUCCESS == aiGetMaterialIntegerArray(mat, AI_MATKEY_ENABLE_WIREFRAME, &wireframe, &max))
        m_material.fill_mode = wireframe ? GL_LINE : GL_FILL;
    else
        m_material.fill_mode = GL_FILL;

    return true;
}

// bind voas, upload data; drawing only happens in the draw()
bool Mesh::uploadToGPU()
{
	// error handling
    if(m_vertices.size() == 0) {
    	std::cout << "mesh::uploadToGPU -> no vertices found" << std::endl;
    	return false;
    }

	// glGenBuffers(1, &m_vertexID); // old
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vertexID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexID);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &(m_vertices[0][0]), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//    if(m_textureIDs.size() != m_tCoords.size()){
//        std::cerr<<__PRETTY_FUNCTION__<<" number of textures not equal to number of texture coordinates!!!"<<std::endl;
//        std::cerr<<" There are "<<m_tCoords.size()<<" sets of texture coords and "<<m_textureIDs.size()<<" textures"<<std::endl;
//    }
//
//    upload colors
//    if(m_colors.size() != 0){
//        glGenBuffers(1, &m_colorsID);
//        glBindBuffer(GL_ARRAY_BUFFER, m_colorsID);
//        glBufferData(GL_ARRAY_BUFFER, m_colors.size() * sizeof(glm::vec4), &(m_colors[0][0]), GL_STATIC_DRAW);
//    }
//
//    upload normals
//    if(m_normals.size() != 0){
//        glGenBuffers(1, &m_normalID);
//        glBindBuffer(GL_ARRAY_BUFFER, m_normalID);
//        glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(glm::vec3), &(m_normals[0][0]), GL_STATIC_DRAW);
//    }
//
//    upload texture coords
//    for(std::vector<std::vector<glm::vec2> >::iterator it = m_tCoords.begin(); it != m_tCoords.end(); ++it){
//        GLuint tmpID;
//        glGenBuffers(1, &tmpID);
//        glBindBuffer(GL_ARRAY_BUFFER, tmpID);
//        glBufferData(GL_ARRAY_BUFFER, it->size() * sizeof(glm::vec2), &((*it)[0][0]), GL_STATIC_DRAW);
//        m_tcoordIDs.push_back(tmpID);
//    }
//
//    upload indices
    if(m_indices.size() != 0){
        glGenBuffers(1, &m_indicesID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &(m_indices[0]), GL_STATIC_DRAW);
    }
//
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//
//    glBindBuffer(GL_ARRAY_BUFFER,0);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    return true;
}

void Mesh::draw()
{
//	mat_from_obj *  m_SceneTrafo * m_ObjectTrafo;
//	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_material.diffuse);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_material.ambient);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_material.specular);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, m_material.emission);
//	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_material.shininess * m_material.shininess_strength);
//  glPolygonMode(GL_FRONT_AND_BACK, m_material.fill_mode);

//  glPushMatrix();
//  glMultMatrixf(glm::value_ptr(m_ObjectTrafo));
//	glMultMatrixf(glm::value_ptr(m_SceneTrafo));
//
//    if(m_vertexID){
//        glBindBuffer(GL_ARRAY_BUFFER,m_vertexID);
//        glEnableClientState(GL_VERTEX_ARRAY);//!
//        glVertexPointer(3,GL_FLOAT,0,0);//!
//    }
//    if(m_colorsID){
//    	std::cout << "colorsID" << std::endl;
//        glBindBuffer(GL_ARRAY_BUFFER, m_colorsID);
//        glEnableClientState(GL_COLOR_ARRAY);//!
//        glColorPointer(4,GL_FLOAT,0,0);//!
//    }
//    if(m_normalID){
//    	std::cout << "normalID" << std::endl;
//        glBindBuffer(GL_ARRAY_BUFFER, m_normalID);
//        glEnableClientState(GL_NORMAL_ARRAY);//!
//        glNormalPointer(GL_FLOAT,0,0);//!
//    }
//
//    std::vector<GLuint>::iterator tcIt =  m_tcoordIDs.begin();
//    std::vector<GLuint>::iterator tIt =  m_textureIDs.begin();
//    unsigned int index = 0;
//
//
//    // if there's more than 1 texture, do cool stuff @TODO: what exactly?
//    if(tIt != m_textureIDs.end()){
//        for(;tcIt != m_tcoordIDs.end();++tcIt){
//            glEnable(GL_TEXTURE_2D);
//            glClientActiveTexture(GL_TEXTURE0 + index);//!
//            glBindTexture(GL_TEXTURE_2D,(*tIt)); //Bind actual texture
//            glBindBuffer(GL_ARRAY_BUFFER,(*tcIt)); //bind coord buffer
//            glEnableClientState(GL_TEXTURE_COORD_ARRAY);//!
//            glTexCoordPointer(2,GL_FLOAT,0,0);//!
//            ++index;
//            ++tIt;
//            if(tIt == m_textureIDs.end()){ //if no more textures
//                break;
//            }
//        }
//    }
	glBindVertexArray(m_vao);
    if(m_indicesID){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_indicesID);
        glDrawElements(m_drawMode,m_indices.size(), GL_UNSIGNED_INT, (GLvoid*)0);
    } else {
        glDrawArrays(GL_TRIANGLES,0,m_vertices.size());
    }
	glBindVertexArray(0);
//
//
////unbind everything
//    glDisableClientState(GL_TEXTURE_COORD_ARRAY);//!
//    glDisableClientState(GL_VERTEX_ARRAY);//!
//    glDisableClientState(GL_NORMAL_ARRAY);//!
//    glDisableClientState(GL_COLOR_ARRAY);//!
//    glBindTexture(GL_TEXTURE_2D,0);
//
//    glBindBuffer(GL_ARRAY_BUFFER,0);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
//    glPopMatrix();

    GLenum err;
    if(m_debug){
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "################## after draw call: OpenGL error: " << err << std::endl;
        }
    }

}