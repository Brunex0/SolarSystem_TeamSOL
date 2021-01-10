void transferDataToGPU(INFO *a, VECTORS *Sphere, GLuint programID, const char* textureName, const char* objectName) {
	
	glGenVertexArrays(1, (&a->VertexArrayID));
	glBindVertexArray((a->VertexArrayID));
	// Get a handle for our "MVP" uniform
	a->MatrixID = glGetUniformLocation(programID, "MVP");

	// Load the texture
	a->Texture = loadDDS(textureName);

	// Get a handle for our "myTextureSampler" uniform
	a->TextureID = glGetUniformLocation(programID, "myTextureSampler");

	// Read our .obj file
	bool res = loadOBJ(objectName, Sphere->vertices, Sphere->uvs, Sphere->normals);
	
	
	// Load it into a VBO
	glGenBuffers(1, &a->vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, a->vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, (&Sphere->vertices)->size() * sizeof(glm::vec3), &Sphere->vertices[0][0], GL_STATIC_DRAW);

	glGenBuffers(1, &a->uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, a->uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, (&Sphere->uvs)->size() * sizeof(glm::vec2), &Sphere->uvs[0][0], GL_STATIC_DRAW);
}