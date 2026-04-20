////
//// Created by Artur Duch on 01/09/2023.
////
//
//#include <cstdio>
//#include <SDL_log.h>
//#include "Q2Model.h"
//
//
//TQ2Model* openFile(const char* path) {
//    FILE *f = fopen(path, "rb");
//
//    TQ2Model *out = new TQ2Model();
//
//    TQ2Header header;
//
//    fread(&header, sizeof(TQ2Header), 1, f);
//
//    SDL_Log("Tris %d, verts %d, cords %d", header.num_tris, header.num_vertices, header.num_st);
//    SDL_Log("Frame size %d", header.framesize);
//    SDL_Log("Frame count %d", header.num_frames);
//    SDL_Log("Skin count %d", header.num_skins);
//
//    auto fileTex = new TMD2TexCoord [header.num_st];
//
//    out->skins = new TMD2Skin [header.num_skins];
//    out->tex = new float [header.num_st * 2];
//    out->tris = new TMD2Triangle [header.num_tris];
//    out->frames = new TMD2Frame [header.num_frames];
//
////    auto glcmds = (int *)malloc (sizeof (int) * header.num_glcmds);
//
//    /* Read model data */
//    fseek (f, header.offset_skins, SEEK_SET);
//    fread (out->skins, sizeof(TMD2Skin),header.num_skins, f);
//
//    fseek (f, header.offset_st, SEEK_SET);
//    fread (fileTex, sizeof(TMD2TexCoord),header.num_st, f);
//
//    for(int i = 0; i<header.num_st; i++) {
//        out->tex[i * 2 + 0] = ((float)fileTex[i].u) / header.skinwidth;
//        out->tex[i * 2 + 1] = ((float)fileTex[i].v) / header.skinheight;
//    }
//
//    fseek (f, header.offset_tris, SEEK_SET);
//    fread (out->tris, sizeof (TMD2Triangle),header.num_tris, f);
//
////    fseek (f, header.offset_glcmds, SEEK_SET);
////    fread (glcmds, sizeof (int), header.num_glcmds, f);
//
//    /* Read frames */
//    fseek (f, header.offset_frames, SEEK_SET);
//    for (int i = 0; i < header.num_frames; ++i)
//    {
//        out->frames[i].verts = new TMD2Vertex [header.num_vertices];
//        /* Memory allocation for vertices of this frame */
//
//        /* Read frame data */
//        fread (out->frames[i].scale, sizeof (vec3_t), 1, f);
//        fread (out->frames[i].translate, sizeof (vec3_t), 1, f);
//        fread (out->frames[i].name, sizeof (char), 16, f);
//        fread (out->frames[i].verts, sizeof (TMD2Vertex),header.num_vertices, f);
////        SDL_Log("Frame name %d: %s", i, out->frames[i].name);
//    }
//    fclose(f);
//
//    out->skinCount = header.num_skins;
//    out->texCount = header.num_st;
//    out->vertCount = header.num_vertices;
//    out->trisCount = header.num_tris;
//    out->frameCount = header.num_frames;
//
//    delete[] fileTex;
//
//    return out;
//}
//
//void getTModel(Q2Model& src, TModel& out, int srcFrame, int dstFrame, float progress, TDrawMemPool& tmpPool, TDrawMemPool& pool) {
//    TMD2Frame& sFrame = src.frames[srcFrame];
//    TMD2Frame& dFrame = src.frames[dstFrame];
//
//    TDrawMemBuf tmpBuf;
//    peekMemBuf(tmpPool, tmpBuf);
//
//    if (tmpBuf.vertexSize < src.vertCount * 4) {
//        resize(tmpBuf, src.vertCount * 4);
//    }
//
//    for (int i = 0; i<src.vertCount; i++) {
//        float x = (((float) sFrame.verts[i].v[0]) * sFrame.scale[0] + sFrame.translate[0]) * (1.0f - progress) +
//                  (((float) dFrame.verts[i].v[0]) * dFrame.scale[0] + dFrame.translate[0]) * progress;
//        float y = (((float) sFrame.verts[i].v[1]) * sFrame.scale[1] + sFrame.translate[1]) * (1.0f - progress) +
//                  (((float) dFrame.verts[i].v[1]) * dFrame.scale[1] + dFrame.translate[1]) * progress;
//        float z = (((float) sFrame.verts[i].v[2]) * sFrame.scale[2] + sFrame.translate[2]) * (1.0f - progress) +
//                  (((float) dFrame.verts[i].v[2]) * dFrame.scale[2] + dFrame.translate[2]) * progress;
//
//        tmpBuf.vertexMem[i * 4 + 0] = y;
//        tmpBuf.vertexMem[i * 4 + 1] = -z;
//        tmpBuf.vertexMem[i * 4 + 2] = x;
//        tmpBuf.vertexMem[i * 4 + 3] = 1.0f;
//    }
//
//    TDrawMemBuf outBuf;
//    peekMemBuf(pool, outBuf);
//
//    if (outBuf.trisSize < src.trisCount * 3) {
//        resize(outBuf, src.trisCount * 3);
//    }
//
//    if (outBuf.vertexSize < src.trisCount * 3 * VERTEX_SIZE) {
//        resize(outBuf, src.trisCount * 3 * VERTEX_SIZE);
//    }
//
//    claimBuf(pool, src.trisCount * 3 * VERTEX_SIZE, src.trisCount * 3);
//
//    out.trisCount = src.trisCount;
//    out.vertCount = src.trisCount * 3;
//    out.tris = outBuf.trisMem;
//    out.vertexes = outBuf.vertexMem;
//
//    for(int i = 0; i<src.trisCount; i++) {
//        outBuf.trisMem[i * 3 + 0] = i * 3 + 0;
//        outBuf.trisMem[i * 3 + 1] = i * 3 + 1;
//        outBuf.trisMem[i * 3 + 2] = i * 3 + 2;
//        int v0  = src.tris[i].vertex[0];
//        int uv0 = src.tris[i].st[0];
//
//        int v1  = src.tris[i].vertex[1];
//        int uv1 = src.tris[i].st[1];
//
//        int v2  = src.tris[i].vertex[2];
//        int uv2 = src.tris[i].st[2];
//
//        memcpy(out.vertexes + (i * 3 + 0) * VERTEX_SIZE, tmpBuf.vertexMem + v0 * 4, sizeof(float) * 4);
//        memcpy(out.vertexes + (i * 3 + 0) * VERTEX_SIZE + 4,src.tex + uv0 * 2, sizeof(float) * 2);
//
//        memcpy(out.vertexes + (i * 3 + 1) * VERTEX_SIZE, tmpBuf.vertexMem + v1 * 4, sizeof(float) * 4);
//        memcpy(out.vertexes + (i * 3 + 1) * VERTEX_SIZE + 4,src.tex + uv1 * 2, sizeof(float) * 2);
//
//        memcpy(out.vertexes + (i * 3 + 2) * VERTEX_SIZE, tmpBuf.vertexMem + v2 * 4, sizeof(float) * 4);
//        memcpy(out.vertexes + (i * 3 + 2) * VERTEX_SIZE + 4,src.tex + uv2 * 2, sizeof(float) * 2);
//
//    }
//
//
//}