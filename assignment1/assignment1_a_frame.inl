std::vector<GLfloat> A_FRAME_EDGES = {
    // Top point (upper)
    0.f, 0.f, 0.05f,

    // Bottom-right
    0.25f, 0.f, -0.95f,
    0.2f, 0.f, -0.95f,

    // Top point (lower)
    0.f, 0.f, -0.05f,

    // Bottom-left
    -0.25f, 0.f, -0.95f,
    -0.2f, 0.f, -0.95f,
};

std::vector<GLfloat> A_FRAME_TRIANGLES = {
    // Right
    0.f, 0.f, 0.05f,
    0.f, 0.f, -0.05f,
    0.25f, 0.f, -0.95f,

    0.25f, 0.f, -0.95f,
    0.f, 0.f, -0.05f,
    0.2f, 0.f, -0.95f,

    // Left
    0.f, 0.f, 0.05f,
    -0.25f, 0.f, -0.95f,
    0.f, 0.f, -0.05f,

    -0.25f, 0.f, -0.95f,
    0.f, 0.f, -0.05f,
    -0.2f, 0.f, -0.95f,
};