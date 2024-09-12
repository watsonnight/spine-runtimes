#ifndef Spine_MeshGenerator_h
#define Spine_MeshGenerator_h


#include "spine/Bone.h"
#include "spine/Color.h"
#include "spine/MeshAttachment.h"
#include "spine/Skeleton.h"
#include "spine/Slot.h"
#include "spine/Vector.h"
#include <spine/SkeletonClipping.h>
#include <utility>
#include <limits>
#include "spine/SkeletonClipping.h"


#include "Utils.h"
#include "SkeletonRendererInstruction.h"

namespace spine 
{
    class MeshGenerator : public SpineObject
    {
        friend class Skeleton;
        friend class SkeletonRendererInstruction;



public:
        MeshGenerator();
        ~MeshGenerator();

        struct Settings
        {
            public:

            bool useClipping = true;
            float zSpacing = 0.0;
            bool pmaVertexColors = true;
            bool tintBlack = false;
                

            bool canvasGroupTintBlack;
            bool calculateTangents = false;
            bool addNormals = false;
            bool immutableTriangles = false;
        };

        Settings settings;
        int getVertexBuffer(Vector3* dst);
        int getUVBuffer(Vector2* dst);
        int getColorBuffer(float* dst); 
        int getColorBufferVec4(Vector4* dst);

        void getSubMeshes(SkeletonRendererInstruction& instruction, int* dst);
        void getSubMeshesSize(SkeletonRendererInstruction& instruction, int * dst);
        void getMeshBounds(float* min, float* max);
        int getTangentBuffer(Vector4* dst);


        void generateMeshRenderers(Skeleton* skeleton);

        SkeletonRendererInstruction multipleInstruction;


protected:
        Vector<Vector3> vertexBuffer;
        Vector<Vector2> uvBuffer;
        // Vector<Color> colorBuffer;
        Vector<Vector4> colorBuffer;

        Vector<Vector2>* uv2;
        Vector<Vector2>* uv3;
        Vector<Vector3>* normals;
        Vector<Vector4>* tangents;

        Vector<Vector<int>> subMeshes;

        Vector2 meshBoundsMin, meshBoundsMax;
        float meshBoundsThickness;
        
        SkeletonClipping clipper;

        int submeshIndex;

        //float* tempVerts = new float[8];
        Vector<float> tempVerts;
        int regionTriagles[6] = {0, 1, 2, 2, 3, 0};

        Vector<Vector2> tempTanBuffer;

        
public:
        SkeletonRendererInstruction* currentInstruction;

        SkeletonRendererInstruction oldInstruction[2], newInstruction;
        int instructionIndex = 0;

        void Begin();
        void BuildMeshWithArrays (SkeletonRendererInstruction& instruction, bool updateTriangles);
        size_t getBufferSize();

        bool IsInstructionNotEqual();
        void ScaleVertexDataWithBuffers(SkeletonRendererInstruction& instruction, float scale, Vector3* vertexBuffer);

        void BuildMeshWithArraysWithBuffersColor32(SkeletonRendererInstruction& instruction, bool updateTriangles, Vector3* vertexBuffer, Vector2* uvBuffer, uint32_t* colorBuffer);
        void FillLateVertexDataWithBuffers(Vector4* tangents, int vertexCount);
        static void SolveTangents2DBufferWithBuffer(Vector4* tangents, Vector<Vector2>& tempTanBuffer, int vertexCount);


        void ScaleSubmeshVertexDataWithBuffers(SkeletonRendererInstruction& instruction, int submeshIndex, float scale, Vector3* vertexBuffer);
        void getSubMeshes(int submeshIndex, int* dst);

        void BuildMeshSubmeshWithArraysWithBuffersColor32(SkeletonRendererInstruction& instruction, int submeshIndex, bool updateTriangles, Vector3* vertexBuffer, Vector2* uvBuffer, uint32_t* colorBuffer);
        void FillSubmeshLateVertexDataWithBuffers(int submeshIndex, Vector4* tangents, int vertexCount);


        static void BuildInstruction(SubmeshInstructionTransfer* instructions, int size, Skeleton* skel[], SkeletonRendererInstruction* output)
        {
            if (size <= 0) return;
            if (output == nullptr) output = new SkeletonRendererInstruction();
            output->submeshInstructions.clear();

            Skeleton* cur = skel[0];
            auto drawOrder = cur->getDrawOrder();
            int cnt = drawOrder.size();

            output->rawVertexCount = 0;
            // for (int i = 0; i < cnt; i++)
            // {
            //     auto slot = drawOrder[i];
            //     if (!slot->getBone().isActive())
            //     {
            //         continue;
            //     }
            //     MeshAttachment* attachment = static_cast<MeshAttachment*>(slot->getAttachment());
	
            //     output->rawVertexCount += attachment->getWorldVerticesLength();
            // }

            for (int i = 0; i < size; i++)
            {
                SubmeshInstruction* curr = new SubmeshInstruction(instructions[i], skel[i]);
                output->submeshInstructions.add(curr);
                output->rawVertexCount += curr->rawVertexCount;
                if (curr->hasClipping) output->hasActiveClipping = true;
            }
        }

        void AddSubmesh (SubmeshInstruction instruction, bool updateTriangles = true);
        void AddAttachmentTintBlack (float r2, float g2, float b2, float a, int vertexCount);
        void ScaleVertexData (float scale);
        void EnsureVertexCapacity(int minimumVertexCount, bool includeTintBlack, bool includeTangents, bool includeNormals);

        void FillLateVertexData();

        static void SolveTangents2DTriangles(Vector<Vector2>&, Vector<int>&, int triangleCount, Vector<Vector3>& vertices, Vector<Vector2>& uvs, int vertexCount);
        static void SolveTangents2DBuffer(Vector<Vector4>& tangents, Vector<Vector2>& tempTanBuffer, int vertexCount);
    };
}



#endif