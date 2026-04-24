#include <cstdio>
#include <SDL_log.h>
#include "Q2Model.h"

TQ2Model* openFile(FILE* f, long offset) {
    TQ2Model *out = new TQ2Model();

    TQ2Header header;

    fseek(f, offset, SEEK_SET);
    fread(&header, sizeof(TQ2Header), 1, f);

    SDL_Log("Tris %d, verts %d, cords %d", header.num_tris, header.num_vertices, header.num_st);
    SDL_Log("Frame size %d", header.framesize);
    SDL_Log("Frame count %d", header.num_frames);
    SDL_Log("Skin count %d", header.num_skins);

    auto fileTex = new TMD2TexCoord [header.num_st];

    out->skins = new TMD2Skin [header.num_skins];
    out->tex = new float [header.num_st * 2];
    out->tris = new TMD2Triangle [header.num_tris];
    out->frames = new TMD2Frame [header.num_frames];

//    auto glcmds = (int *)malloc (sizeof (int) * header.num_glcmds);

    /* Read model data */
    fseek (f, offset + header.offset_skins, SEEK_SET);
    fread (out->skins, sizeof(TMD2Skin),header.num_skins, f);

    fseek (f, offset + header.offset_st, SEEK_SET);
    fread (fileTex, sizeof(TMD2TexCoord),header.num_st, f);

    for(int i = 0; i<header.num_st; i++) {
        out->tex[i * 2 + 0] = ((float)fileTex[i].u) / header.skinwidth;
        out->tex[i * 2 + 1] = ((float)fileTex[i].v) / header.skinheight;
    }

    fseek (f, offset + header.offset_tris, SEEK_SET);
    fread (out->tris, sizeof (TMD2Triangle),header.num_tris, f);

//    fseek (f, header.offset_glcmds, SEEK_SET);
//    fread (glcmds, sizeof (int), header.num_glcmds, f);

    /* Read frames */
    fseek (f, offset + header.offset_frames, SEEK_SET);
    for (int i = 0; i < header.num_frames; ++i)
    {
        out->frames[i].verts = new TMD2Vertex [header.num_vertices];
        /* Memory allocation for vertices of this frame */

        /* Read frame data */
        fread (out->frames[i].scale, sizeof (vec3_t), 1, f);
        fread (out->frames[i].translate, sizeof (vec3_t), 1, f);
        fread (out->frames[i].name, sizeof (char), 16, f);
        fread (out->frames[i].verts, sizeof (TMD2Vertex),header.num_vertices, f);
//        SDL_Log("Frame name %d: %s", i, out->frames[i].name);
    }
    fclose(f);

    out->skinCount = header.num_skins;
    out->texCount = header.num_st;
    out->vertCount = header.num_vertices;
    out->trisCount = header.num_tris;
    out->frameCount = header.num_frames;

    delete[] fileTex;

    return out;
}

TQ2ModelFrame* allocateFrame(TQ2Model* model) {
    TQ2ModelFrame *frame = (TQ2ModelFrame*)malloc(sizeof(TQ2ModelFrame));
    frame->trisCount = model->trisCount;
    frame->vertCount = frame->trisCount * 3;
    frame->triangle = new uint32_t[frame->trisCount * 3];
    frame->vertexes = new ModelVertex [frame->trisCount * 3];
    frame->rawBuf = new float[4 * model->vertCount];
    return frame;
}

void getTModel(Q2Model& src, TQ2ModelFrame & out, int srcFrame, int dstFrame, float progress) {
//    progress = 0.0;
    TMD2Frame& sFrame = src.frames[srcFrame];
    TMD2Frame& dFrame = src.frames[dstFrame];

    static int lastFrame = -1;
    if (lastFrame != srcFrame) {
//        printf("frame id %d %s\n", srcFrame, sFrame.name);
        lastFrame = srcFrame;
    }

    for (int i = 0; i<src.vertCount; i++) {
        float x = (((float) sFrame.verts[i].v[0]) * sFrame.scale[0] + sFrame.translate[0]) * (1.0f - progress) +
                  (((float) dFrame.verts[i].v[0]) * dFrame.scale[0] + dFrame.translate[0]) * progress;
        float y = (((float) sFrame.verts[i].v[1]) * sFrame.scale[1] + sFrame.translate[1]) * (1.0f - progress) +
                  (((float) dFrame.verts[i].v[1]) * dFrame.scale[1] + dFrame.translate[1]) * progress;
        float z = (((float) sFrame.verts[i].v[2]) * sFrame.scale[2] + sFrame.translate[2]) * (1.0f - progress) +
                  (((float) dFrame.verts[i].v[2]) * dFrame.scale[2] + dFrame.translate[2]) * progress;

        out.rawBuf[i * 4 + 0] = y;
        out.rawBuf[i * 4 + 1] = z;
        out.rawBuf[i * 4 + 2] = x;
        out.rawBuf[i * 4 + 3] = 1.0f;
    }

    auto vertexes = (float*)out.vertexes;
    int VERTEX_SIZE = 6;

    for(int i = 0; i<src.trisCount; i++) {
        out.triangle[i * 3 + 0] = i * 3 + 0;
        out.triangle[i * 3 + 1] = i * 3 + 1;
        out.triangle[i * 3 + 2] = i * 3 + 2;

        int v0  = src.tris[i].vertex[0];
        int uv0 = src.tris[i].st[0];

        int v1  = src.tris[i].vertex[1];
        int uv1 = src.tris[i].st[1];

        int v2  = src.tris[i].vertex[2];
        int uv2 = src.tris[i].st[2];


        memcpy(vertexes + (i * 3 + 0) * VERTEX_SIZE, out.rawBuf + v0 * 4, sizeof(float) * 4);
        memcpy(vertexes + (i * 3 + 0) * VERTEX_SIZE + 4,src.tex + uv0 * 2, sizeof(float) * 2);

        memcpy(vertexes + (i * 3 + 1) * VERTEX_SIZE, out.rawBuf + v1 * 4, sizeof(float) * 4);
        memcpy(vertexes + (i * 3 + 1) * VERTEX_SIZE + 4,src.tex + uv1 * 2, sizeof(float) * 2);

        memcpy(vertexes + (i * 3 + 2) * VERTEX_SIZE, out.rawBuf + v2 * 4, sizeof(float) * 4);
        memcpy(vertexes + (i * 3 + 2) * VERTEX_SIZE + 4,src.tex + uv2 * 2, sizeof(float) * 2);

    }


}