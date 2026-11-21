/*
 * 3D Dragon Transformations - C + OpenGL/GLUT + Assimp
 * Controls: WASD/QE translate, IJKL/UO rotate, +/- scale, R reset, Esc quit.
 * Model: Tarisland - Dragon (High Poly), by Doctor A., CC BY 4.0.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static const struct aiScene *scene = NULL;
static float tx = 0.0f, ty = 0.0f, tz = -8.0f;
static float rx = 15.0f, ry = 0.0f, rz = 0.0f;
static float modelScale = 1.0f;
static float centerX = 0.0f, centerY = 0.0f, centerZ = 0.0f;
static float normalizeScale = 1.0f;

static void resetTransform(void) {
    tx = 0.0f; ty = 0.0f; tz = -8.0f;
    rx = 15.0f; ry = 0.0f; rz = 0.0f;
    modelScale = 1.0f;
}

static void calculateBounds(void) {
    struct aiVector3D lo = { 1e30f, 1e30f, 1e30f };
    struct aiVector3D hi = { -1e30f, -1e30f, -1e30f };
    unsigned int m, v;
    for (m = 0; m < scene->mNumMeshes; ++m) {
        const struct aiMesh *mesh = scene->mMeshes[m];
        for (v = 0; v < mesh->mNumVertices; ++v) {
            struct aiVector3D p = mesh->mVertices[v];
            if (p.x < lo.x) lo.x = p.x; if (p.x > hi.x) hi.x = p.x;
            if (p.y < lo.y) lo.y = p.y; if (p.y > hi.y) hi.y = p.y;
            if (p.z < lo.z) lo.z = p.z; if (p.z > hi.z) hi.z = p.z;
        }
    }
    centerX = (lo.x + hi.x) * 0.5f;
    centerY = (lo.y + hi.y) * 0.5f;
    centerZ = (lo.z + hi.z) * 0.5f;
    {
        float dx = hi.x - lo.x, dy = hi.y - lo.y, dz = hi.z - lo.z;
        float largest = fmaxf(dx, fmaxf(dy, dz));
        normalizeScale = (largest > 0.0001f) ? 4.0f / largest : 1.0f;
    }
}

static void setMaterial(const struct aiMesh *mesh) {
    struct aiColor4D diffuse = { 0.75f, 0.35f, 0.22f, 1.0f };
    if (mesh->mMaterialIndex < scene->mNumMaterials) {
        const struct aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
        aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
    }
    glColor4f(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
    {
        GLfloat spec[] = { 0.25f, 0.25f, 0.25f, 1.0f };
        GLfloat shine = 32.0f;
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
    }
}

static void drawMesh(const struct aiMesh *mesh) {
    unsigned int f, j;
    setMaterial(mesh);
    glBegin(GL_TRIANGLES);
    for (f = 0; f < mesh->mNumFaces; ++f) {
        const struct aiFace *face = &mesh->mFaces[f];
        for (j = 0; j < face->mNumIndices; ++j) {
            unsigned int idx = face->mIndices[j];
            if (mesh->mNormals) {
                glNormal3f(mesh->mNormals[idx].x,
                           mesh->mNormals[idx].y,
                           mesh->mNormals[idx].z);
            }
            glVertex3f(mesh->mVertices[idx].x,
                       mesh->mVertices[idx].y,
                       mesh->mVertices[idx].z);
        }
    }
    glEnd();
}

static void display(void) {
    unsigned int m;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(tx, ty, tz);                 /* TRANSLATION */
    glRotatef(rx, 1.0f, 0.0f, 0.0f);          /* ROTATION X */
    glRotatef(ry, 0.0f, 1.0f, 0.0f);          /* ROTATION Y */
    glRotatef(rz, 0.0f, 0.0f, 1.0f);          /* ROTATION Z */
    glScalef(normalizeScale * modelScale,
             normalizeScale * modelScale,
             normalizeScale * modelScale);    /* SCALING */
    glTranslatef(-centerX, -centerY, -centerZ);

    for (m = 0; m < scene->mNumMeshes; ++m)
        drawMesh(scene->mMeshes[m]);

    glutSwapBuffers();
}

static void reshape(int w, int h) {
    if (h < 1) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 0.1, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

static void keyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    switch (key) {
        /* Translation */
        case 'a': tx -= 0.25f; break;
        case 'd': tx += 0.25f; break;
        case 'w': ty += 0.25f; break;
        case 's': ty -= 0.25f; break;
        case 'q': tz += 0.25f; break;
        case 'e': tz -= 0.25f; break;
        /* Rotation */
        case 'i': rx += 5.0f; break;
        case 'k': rx -= 5.0f; break;
        case 'j': ry -= 5.0f; break;
        case 'l': ry += 5.0f; break;
        case 'u': rz -= 5.0f; break;
        case 'o': rz += 5.0f; break;
        /* Scaling */
        case '+': case '=': modelScale *= 1.1f; break;
        case '-': if (modelScale > 0.12f) modelScale /= 1.1f; break;
        case 'r': case 'R': resetTransform(); break;
        case 27: aiReleaseImport(scene); exit(0);
        default: break;
    }
    glutPostRedisplay();
}

static void initGL(void) {
    GLfloat lightPos[] = { 4.0f, 6.0f, 8.0f, 1.0f };
    GLfloat ambient[] = { 0.28f, 0.28f, 0.28f, 1.0f };
    GLfloat diffuse[] = { 0.95f, 0.95f, 0.95f, 1.0f };
    glClearColor(0.025f, 0.03f, 0.055f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
}

int main(int argc, char **argv) {
    const char *modelPath = (argc > 1) ? argv[1] : "scene.gltf";
    scene = aiImportFile(modelPath,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_PreTransformVertices |
        aiProcess_ImproveCacheLocality);
    if (!scene || !scene->mNumMeshes) {
        fprintf(stderr, "Could not load model: %s\nAssimp error: %s\n",
                modelPath, aiGetErrorString());
        return 1;
    }
    calculateBounds();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 720);
    glutCreateWindow("3D Dragon - Translation, Rotation, Scaling");
    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    printf("Dragon loaded. Controls:\n"
           "W/A/S/D: translate X/Y | Q/E: depth\n"
           "I/K: rotate X | J/L: rotate Y | U/O: rotate Z\n"
           "+/-: scale | R: reset | Esc: quit\n");
    glutMainLoop();
    aiReleaseImport(scene);
    return 0;
}
