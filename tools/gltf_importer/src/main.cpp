/**
* @copyright 2023 - Max Bebök
* @license MIT
*/
#include <stdio.h>
#include <string>
#include <filesystem>
#include <algorithm>

#include "structs.h"
#include "parser.h"
#include "hash.h"

#include "binaryFile.h"
#include "converter.h"

namespace fs = std::filesystem;

namespace {
  uint32_t insertString(std::string &stringTable, const std::string &newString) {
    auto strPos = stringTable.find(newString);
    if(strPos == std::string::npos) {
      strPos = stringTable.size();
      stringTable += newString;
      stringTable.push_back('\0');
    }
    return strPos;
  }

  int writeBone(BinaryFile &file, const Bone &bone, std::string &stringTable, int level) {
    //printf("Bone[%d]: %s -> %d\n", bone.index, bone.name.c_str(), bone.parentIndex);

    file.write(insertString(stringTable, bone.name));
    file.write<uint16_t>(bone.parentIndex);
    file.write<uint16_t>(level); // level

    auto matScaled = bone.parentMatrix;
    matScaled[3][0] *= MODEL_SCALE;
    matScaled[3][1] *= MODEL_SCALE;
    matScaled[3][2] *= MODEL_SCALE;

    file.writeArray(matScaled.ptr(), 4*4);

    int boneCount = 1;
    for(const auto& child : bone.children) {
      boneCount += writeBone(file, *child, stringTable, level+1);
    }
    return boneCount;
  };
}

int main(int argc, char* argv[])
{
  const char* gltfPath = argv[1];
  const char* t3dmPath = argv[2];

  auto t3dm = parseGLTF(gltfPath, MODEL_SCALE);
  fs::path gltfBasePath{gltfPath};

  // sort models by transparency mode (opaque -> cutout -> transparent)
  // within the same transparency mode, sort by material
  std::sort(t3dm.models.begin(), t3dm.models.end(), [](const Model &a, const Model &b) {
    if(a.materialA.alphaMode == b.materialA.alphaMode) {
      return a.materialA.uuid < b.materialA.uuid;
    }
    return a.materialA.alphaMode < b.materialA.alphaMode;
  });

  uint32_t chunkIndex = 0;
  uint32_t chunkCount = 2; // vertices + indices
  std::vector<ModelChunked> modelChunks{};
  modelChunks.reserve(t3dm.models.size());
  for(const auto & model : t3dm.models) {
    modelChunks.push_back(chunkUpModel(model));
    chunkCount += 1 + 2; // object + material A/B (@TODO: optimize material count)
  }
  chunkCount += t3dm.skeletons.empty() ? 0 : 1;

  // Main file
  BinaryFile file{};
  file.writeChars("T3DM", 4);
  file.write(chunkCount); // chunk count

  file.write<uint16_t>(0); // total vertex count (set later)
  file.write<uint16_t>(0); // total index count (set later)

  uint32_t offsetChunkTypeTable = file.getPos();
  file.skip(3 * sizeof(uint32_t)); // chunk type indices (filled later)

  uint32_t offsetStringTablePtr = file.getPos();
  file.skip(sizeof(uint32_t)); // string table offset (filled later)

  uint32_t offsetChunkTable = file.getPos();
  file.skip(chunkCount * sizeof(uint32_t)); // chunk-table

  auto addToChunkTable = [&](char type) {
    uint32_t offset = file.posPush();
      file.setPos(offsetChunkTable);
      file.writeChunkPointer(type, offset);
      offsetChunkTable = file.getPos();
    file.posPop();
    ++chunkIndex;
  };

  auto addChunkTypeIndex = [&]() {
    file.posPush();
      file.setPos(offsetChunkTypeTable);
      file.write(chunkIndex);
      offsetChunkTypeTable = file.getPos();
    file.posPop();
  };

  // Chunks
  BinaryFile chunkVerts{};
  BinaryFile chunkIndices{};
  std::vector<std::shared_ptr<BinaryFile>> chunkMaterials{};
  std::vector<BinaryFile> chunkSkeletons{};

  std::string stringTable = "S";

  // now write out each model (aka. collection of mesh-parts + materials)
  int m=0;
  uint32_t materialIndex = 0;
  uint16_t totalVertCount = 0;
  uint16_t totalIndexCount = 0;

  if(!t3dm.skeletons.empty())
  {
    auto &chunkBone = chunkSkeletons.emplace_back();
    chunkBone.skip(4); // size, filed later

    int boneCount = 0;
    for(auto &skel : t3dm.skeletons) {
      boneCount += writeBone(chunkBone, skel, stringTable, 0);
    }

    chunkBone.setPos(0);
    chunkBone.write<uint16_t>(boneCount);
  }

  file.align(8);
  for(auto &model : t3dm.models)
  {
    addToChunkTable('O');

    // write material chunk(s)
    auto writeMaterial = [&chunkMaterials, &stringTable, &gltfBasePath](const Material &material) {
      auto f = std::make_shared<BinaryFile>();
      f->write(material.colorCombiner);
      f->write(material.drawFlags);

      f->write(material.alphaMode);
      f->write(material.fogMode);
      f->skip(2); // reserved/padding

      f->write(material.texReference);

      std::string texPath = "";
      if(!material.texPath.empty()) {
        texPath = fs::relative(material.texPath, std::filesystem::current_path()).string();

        if(texPath.find("assets/") == 0) {
          texPath.replace(0, 7, "rom:/");
        }
        if(texPath.find(".png") != std::string::npos) {
          texPath.replace(texPath.find(".png"), 4, ".sprite");
        }
      }

      if(!texPath.empty()) {
        // check if string already exits
        auto strPos = insertString(stringTable, texPath);

        uint32_t hash = stringHash(texPath);
        printf("Texture: %s (%d)\n", texPath.c_str(), hash);
        f->write((uint32_t)strPos);
        f->write(hash);

      } else {
        f->write(0);
        // if no texture is set, use the reference as hash
        // this is needed to force a reevaluation of the texture state
        f->write(material.texReference);
      }

      f->write((uint32_t)0); // runtime pointer
      f->write((uint16_t)material.texWidth);
      f->write((uint16_t)material.texHeight);

      auto writeTile = [&](const TileParam &tile) {
        f->write(tile.low);
        f->write(tile.high);
        f->write(tile.mask);
        f->write(tile.shift);
        f->write(tile.mirror);
        f->write(tile.clamp);
      };

      writeTile(material.s);
      writeTile(material.t);
      chunkMaterials.push_back(f);
    };
    writeMaterial(model.materialA);
    writeMaterial(model.materialB);

    // write object chunk
    const auto &chunks = modelChunks[m];
    file.write((uint32_t)chunks.chunks.size());
    file.write(materialIndex++);
    file.write(materialIndex++);

    printf("Object %d: %d vert offset\n", m, chunkVerts.getPos());

    // Write parts, these are a collection of indices after a vertex-slice load
    for(const auto& chunk : chunks.chunks)
    {
      //printf("  t3d_vert_load(vertices, %d, %d);\n", chunk.vertexOffset, chunk.vertexCount);
      uint32_t partVertOffset = (chunk.vertexOffset * VertexT3D::byteSize());
      partVertOffset += chunkVerts.getPos();

      file.write(partVertOffset);
      file.write<uint16_t>(chunk.vertexCount);
      file.write<uint16_t>(chunk.vertexDestOffset);
      file.write(chunkIndices.getPos());
      file.write((uint16_t)chunk.indices.size());
      file.write<uint16_t>(chunk.boneIndex); // Matrix/Bone index

      for(uint8_t index : chunk.indices) {
        chunkIndices.write(index);
      }
      totalIndexCount += chunk.indices.size();
    }

    // vertex buffer
    printf("  Verts: %d\n", chunks.vertices.size());
    for(auto v=0; v<chunks.vertices.size(); v+=2)
    {
      const auto &vertA = chunks.vertices[v];
      const auto &vertB = chunks.vertices[v+1];

      //printf("Pos: %d %d %d | %d %d %d\n", vertA.pos[0], vertA.pos[1], vertA.pos[2], vertB.pos[0], vertB.pos[1], vertB.pos[2]);
      chunkVerts.write(vertA.pos[0]);
      chunkVerts.write(vertA.pos[1]);
      chunkVerts.write(vertA.pos[2]);
      chunkVerts.write(vertA.norm);

      chunkVerts.write(vertB.pos[0]);
      chunkVerts.write(vertB.pos[1]);
      chunkVerts.write(vertB.pos[2]);
      chunkVerts.write(vertB.norm);

      chunkVerts.write(vertA.rgba);
      chunkVerts.write(vertB.rgba);

      chunkVerts.write(vertA.s);
      chunkVerts.write(vertA.t);
      chunkVerts.write(vertB.s);
      chunkVerts.write(vertB.t);
    }
    totalVertCount += chunks.vertices.size();

    ++m;
  }

  // Now patch all chunks together and write out the chunk-table

  file.align(16);
  addChunkTypeIndex();
  addToChunkTable('V');
  file.writeMemFile(chunkVerts);

  file.align(4);
  addChunkTypeIndex();
  addToChunkTable('I');
  file.writeMemFile(chunkIndices);

  addChunkTypeIndex();
  for(auto &f : chunkMaterials) {
    file.align(8);
    addToChunkTable('M');
    file.writeMemFile(*f);
  }

  for(const auto &chunkSkel : chunkSkeletons) {
    file.align(8);
    addToChunkTable('S');
    file.writeMemFile(chunkSkel);
  }

  // String table
  file.align(4);
  uint32_t stringTableOffset = file.getPos();
  file.write(stringTable);

  file.setPos(offsetStringTablePtr);
  file.write(stringTableOffset);

  // patch vertex/index count
  file.setPos(0x08);
  file.write(totalVertCount);
  file.write(totalIndexCount);

  // write to actual file
  file.writeToFile(t3dmPath);
}