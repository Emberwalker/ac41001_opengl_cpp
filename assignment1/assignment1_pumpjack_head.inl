std::vector<GLfloat> PUMPJACK_HEAD_EDGES = {
    // Beam quad
    -0.4f, 0.f, 0.05f,
    0.5f, 0.f, 0.05f,
    0.5f, 0.f, -0.05f,
    -0.4f, 0.f, -0.05f,

    // Head (Lower Half)
    -0.4f, 0.f, -0.15f,
    -0.425f, 0.f, -0.15f,
    -0.5f, 0.f, -0.05f,

    // Head (Upper Half)
    -0.5f, 0.f, 0.075f,
    -0.45f, 0.f, 0.15f,
    -0.4f, 0.f, 0.15f,
    // Loop to start of beam quad. END.
};

std::vector<GLfloat> PUMPJACK_HEAD_TRIANGLES = {
    // Beam quad
    -0.4f, 0.f, 0.05f,
    -0.4f, 0.f, -0.05f,
    0.5f, 0.f, 0.05f,

    0.5f, 0.f, 0.05f,
    -0.4f, 0.f, -0.05f,
    0.5f, 0.f, -0.05f,

    // Head (Lower-right quad)
    -0.4f, 0.f, -0.05f,
    -0.425f, 0.f, -0.05f,
    -0.4f, 0.f, -0.15f,

    -0.4f, 0.f, -0.15f,
    -0.425f, 0.f, -0.05f,
    -0.425f, 0.f, -0.15f,

    // Head (Lower-left tri)
    -0.425f, 0.f, -0.15f,
    -0.425f, 0.f, -0.05f,
    -0.5f, 0.f, -0.05f,

    // Head (Mid-section quad)
    -0.5f, 0.f, -0.05f,
    -0.5f, 0.f, 0.075f,
    -0.4f, 0.f, -0.05f,

    -0.5f, 0.f, 0.075f,
    -0.4f, 0.f, 0.075f,
    -0.4f, 0.f, -0.05f,

    // Head (Upper-left tri)
    -0.5f, 0.f, 0.075f,
    -0.45f, 0.f, 0.15f,
    -0.45f, 0.f, 0.075f,

    // Head (Upper-right quad)
    -0.45f, 0.f, 0.15f,
    -0.4f, 0.f, 0.15f,
    -0.45f, 0.f, 0.075f,

    -0.4f, 0.f, 0.15f,
    -0.45f, 0.f, 0.075f,
    -0.4f, 0.f, 0.075f,
};