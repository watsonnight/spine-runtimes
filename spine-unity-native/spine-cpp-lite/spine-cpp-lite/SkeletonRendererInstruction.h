#ifndef Spine_SkeletonRendererInstruction_h
#define Spine_SkeletonRendererInstruction_h

#include "spine/Attachment.h"
#include "spine/Skeleton.h"
#include "spine/Vector.h"
#include <cstdint>


namespace spine 
{
    class Attachment;
    struct SubmeshInstructionTransfer;
    struct SubmeshInstruction;


    struct SubmeshInstructionTransfer
    {
        public:
        intptr_t skeletonHandle;
        int startSlot;
        int endSlot;
        bool forceSeparate;
        int preActiveClippingSlotSource;

        int rawTriangleCount;
        int rawVertexCount;
        int rawFirstVertexIndex;
        bool hasClipping;
        bool hasPMAAdditiveSlot;

    };
    struct SubmeshInstruction
    {
    public:
        Skeleton* skeleton;
        int startSlot = 0;
        int endSlot;
        bool forceSeparate = false;
        int preActiveClippingSlotSource = -1;

        int rawTriangleCount = 0;
        int rawVertexCount = 0;
        int rawFirstVertexIndex = 0;
        bool hasClipping = false;
        bool hasPMAAdditiveSlot = false;

        int regionHashCode = 0;
        int regionSlotIndex = -1;

        SubmeshInstruction(const SubmeshInstructionTransfer& trans, Skeleton* skel): skeleton(skel), startSlot(trans.startSlot),
        endSlot(trans.endSlot), forceSeparate(trans.forceSeparate), preActiveClippingSlotSource(trans.preActiveClippingSlotSource),
        rawTriangleCount(trans.rawTriangleCount), rawVertexCount(trans.rawVertexCount), rawFirstVertexIndex(trans.rawFirstVertexIndex),
        hasClipping(trans.hasClipping), hasPMAAdditiveSlot(trans.hasPMAAdditiveSlot)
        {}

        SubmeshInstruction() {}

        Vector<float> positions;
        Vector<float> uvs;
        Vector<uint32_t> colors;
        Vector<uint16_t> indices;
        int blendMode = 0;
        int32_t atlasPage = 0;
        String texturePath;
    };



    class SkeletonRendererInstruction
    {
    public:
        Vector<SubmeshInstruction*> submeshInstructions;
        bool immutableTriangles = false;
        bool hasActiveClipping = false;
        int rawVertexCount = 0;
        int rawTriangleCount = 0;
            
        Vector<Attachment*> attachments; // not used

        void moveTo(SkeletonRendererInstruction& other);

        ~SkeletonRendererInstruction();

        void DeleteAll();
        void Clear();
        //static SkeletonRendererInstruction* BuildInstruction(spine::Skeleton*, SubmeshInstructionTransfer* instructions[], int size);
    };


}





#endif