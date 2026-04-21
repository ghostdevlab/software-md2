//
// Created by Artur Duch on 01/09/2023.
//

#ifndef ENGINE_Q2MODEL_H
#define ENGINE_Q2MODEL_H


#pragma pack(push, 1)

#include <cstdio>

typedef struct Q2Header {
    int ident;                  /* magic number: "IDP2" */
    int version;                /* version: must be 8 */

    int skinwidth;              /* texture width */
    int skinheight;             /* texture height */

    int framesize;              /* size in bytes of a frame */

    int num_skins;              /* number of skins */
    int num_vertices;           /* number of vertices per frame */
    int num_st;                 /* number of texture coordinates */
    int num_tris;               /* number of triangles */
    int num_glcmds;             /* number of opengl commands */
    int num_frames;             /* number of frames */

    int offset_skins;           /* offset skin data */
    int offset_st;              /* offset texture coordinate data */
    int offset_tris;            /* offset triangle data */
    int offset_frames;          /* offset frame data */
    int offset_glcmds;          /* offset OpenGL command data */
    int offset_end;             /* offset end of file */
} TQ2Header;

typedef float vec3_t[3];

typedef struct MD2Skin
{
    char name[64];              /* texture file name */
} TMD2Skin;

typedef struct MD2TexCoord
{
    short u;
    short v;
} TMD2TexCoord;

typedef struct MD2Triangle
{
    unsigned short vertex[3];   /* vertex indices of the triangle */
    unsigned short st[3];       /* tex. coord. indices */
} TMD2Triangle;

typedef struct MD2Vertex
{
    unsigned char v[3];         /* position */
    unsigned char normalIndex;  /* normal vector index */
} TMD2Vertex;

typedef struct MD2Frame
{
    vec3_t scale;               /* scale factor */
    vec3_t translate;           /* translation vector */
    char name[16];              /* frame name */
    TMD2Vertex *verts; /* list of frame's vertices */
} TMD2Frame;

typedef struct Q2Model {
    TMD2Skin* skins;
    float* tex;
    TMD2Triangle* tris;
    TMD2Frame* frames;

    int skinCount;
    int texCount;
    int vertCount;
    int trisCount;
    int frameCount;
} TQ2Model;


typedef struct {
    float x, y, z, w;
    float u, v;
} ModelVertex;

#pragma pack(pop)

typedef struct {
    uint32_t* triangle;
    ModelVertex* vertexes;
    int vertCount;
    int trisCount;

    float* rawBuf;
} TQ2ModelFrame;

TQ2Model* openFile(FILE* f, long offset);
TQ2ModelFrame* allocateFrame(TQ2Model* model);
void getTModel(Q2Model& src, TQ2ModelFrame & out, int srcFrame, int dstFrame, float progress);

#endif //ENGINE_Q2MODEL_H
