#define M_PI 3.141595f
void JungleExploration::createSphereMesh(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx) {
	int latitudeLines = 40;
	int longitudeLines = 40;
	float radius = 0.2;
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 uv;
	// The primitive built here is a sphere of radius 1, centered in the origin, on which the Mars texture is applied seamless.
	for (int lat = 0; lat <= latitudeLines; ++lat) {
        float theta = lat * 3.14159f / latitudeLines;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon <= longitudeLines; ++lon) {
            float phi = lon * 2 * 3.14159f / longitudeLines;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;

            
            position[0] = radius * x;
            position[1] = radius * y ;
            position[2] = radius * z;

            normal[0] = x;
            normal[1] = y;
            normal[2] = z;

            // Calculate UV coordinates
            uv[0] = static_cast<float>(lon) / longitudeLines;
            uv[1] = static_cast<float>(lat) / latitudeLines;

            vDef.push_back({position, normal, uv});
        }
    }

    // Generate indices
    for (int lat = 0; lat < latitudeLines; ++lat) {
        for (int lon = 0; lon < longitudeLines; ++lon) {
            int first = lat * (longitudeLines + 1) + lon;
            int second = first + 1;
            int third = first + longitudeLines + 1;
            int fourth = third + 1;

            vIdx.push_back(first);
            vIdx.push_back(second);
            vIdx.push_back(third);

            vIdx.push_back(third);
            vIdx.push_back(second);
            vIdx.push_back(fourth);
        }
    }
	// The procedure fills array vPos with the positions of the vertices and of the normal vectors of the mesh
	//vDef.push_back({{0,0,0}, {0,0,1}, {0,0}});	// vertex 0 - Position, Normal and uv
	//vDef.push_back({{1,0,0}, {0,0,1}, {1,0}});	// vertex 1 - Position and Normal
	//vDef.push_back({{0,1,0}, {0,0,1}, {0,1}});	// vertex 2 - Position and Normal
	//vDef.push_back({{1,1,0}, {0,0,1}, {1,1}});// vertex 3 - Position and Normal
//
	//// The procedures also fill the array vIdx with the indices of the vertices of the triangles
	//vIdx.push_back(0); vIdx.push_back(1); vIdx.push_back(2);	// First triangle
	//vIdx.push_back(1); vIdx.push_back(2); vIdx.push_back(3);	// Second triangle
}

