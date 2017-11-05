std::vector<GLfloat> COUNTERWEIGHT_EDGES = {
    // Main beam left
    0.f, 0.f, 0.05f,
    0.05f, 0.f, 0.05f,
    0.05f, 0.f, -0.05f,
    -0.3f, 0.f, -0.05f,

    // Weight lower
    -0.3f, 0.f, -0.2f,
    -0.4f, 0.f, -0.35f,
    -0.5f, 0.f, -0.35f,
    -0.6f, 0.f, -0.2f,

    // Main beam right
    -0.6f, 0.f, -0.05f,
    -0.8f, 0.f, -0.05f,
    -0.8f, 0.f, 0.05f,
    -0.3f, 0.f, 0.05f,

    // Weight upper
    -0.3f, 0.f, 0.2f,
    -0.2f, 0.f, 0.35f,
    -0.1f, 0.f, 0.35f,
    0.f, 0.f, 0.2f,
};

std::vector<GLfloat> COUNTERWEIGHT_TRIANGLES = {
    // Main beam quad
    0.05f, 0.f, -0.05f,
    0.05f, 0.f, 0.05f,
    -0.8f, 0.f, 0.05f,

    0.05f, 0.f, -0.05f,
    -0.8f, 0.f, -0.05f,
    -0.8f, 0.f, 0.05f,

    // === Counterweight upper ===
    // Base quad
    -0.3f, 0.f, 0.05f,
    -0.3f, 0.f, 0.2f,
    0.f, 0.f, 0.2f,

    0.f, 0.f, 0.2f,
    -0.3f, 0.f, 0.05f,
    0.f, 0.f, 0.05f,

    // Right tri
    -0.1f, 0.f, 0.35f,
    -0.1f, 0.f, 0.2f,
    0.f, 0.f, 0.2f,

    // Upper quad
    -0.1f, 0.f, 0.35f,
    -0.2f, 0.f, 0.2f,
    -0.1f, 0.f, 0.2f,

    -0.1f, 0.f, 0.35f,
    -0.2f, 0.f, 0.35f,
    -0.2f, 0.f, 0.2f,

    // Left tri
    -0.2f, 0.f, 0.35f,
    -0.3f, 0.f, 0.2f,
    -0.2f, 0.f, 0.2f,

    // === Counterweight lower ===
    // Base quad
    -0.6f, 0.f, -0.05f,
    -0.6f, 0.f, -0.2f,
    -0.3f, 0.f, -0.05f,

    -0.6f, 0.f, -0.2f,
    -0.3f, 0.f, -0.05f,
    -0.3f, 0.f, -0.2f,

    // Left tri
    -0.6f, 0.f, -0.2f,
    -0.5f, 0.f, -0.2f,
    -0.5f, 0.f, -0.35f,

    // Lower quad
    -0.5f, 0.f, -0.35f,
    -0.5f, 0.f, -0.2f,
    -0.4f, 0.f, -0.2f,

    -0.5f, 0.f, -0.35f,
    -0.4f, 0.f, -0.2f,
    -0.4f, 0.f, -0.35f,

    // Right tri
    -0.4f, 0.f, -0.35f,
    -0.4f, 0.f, -0.2f,
    -0.3f, 0.f, -0.2f,
};