

#include <spine/spine.h>
#include "spine/Attachment.h"
#include "spine/BlendMode.h"
#include "spine/Bone.h"
#include "spine/Color.h"
#include "spine/MeshAttachment.h"
#include "spine/ClippingAttachment.h"
#include "spine/RegionAttachment.h"
#include "spine/Skeleton.h"
#include "spine/Slot.h"
#include "spine/SlotData.h"
#include <cmath>
#include <cstddef>
#include <cstring>
#include <utility>

#include "MeshGenerator.h"
#include "SkeletonRendererInstruction.h"



using namespace spine;


static const int RINDEX = 0;
static const int GINDEX = 1;
static const int BINDEX = 2;
static const int AINDEX = 3;


// template specialization for Color
template<>
void Vector<Vector2>::setSizeWithoutConstruct(size_t newSize) {
	assert(newSize >= 0);
	size_t oldSize = _size;
	_size = newSize;
	if (_capacity < newSize) {
		_capacity = (int) (_size * 1.75f);
		if (_capacity < 8) _capacity = 8;
		_buffer = spine::SpineExtension::realloc<Vector2>(_buffer, _capacity, __FILE__, __LINE__);
        // use simple memset to clear newly allocated space
        // size_t bytesToInit = (_size - oldSize) * sizeof(Vector2);
        // memset(_buffer + oldSize, 0, bytesToInit);
	}     
}

template<>
void Vector<Vector3>::setSizeWithoutConstruct(size_t newSize) {
	assert(newSize >= 0);
	size_t oldSize = _size;
	_size = newSize;
	if (_capacity < newSize) {
		_capacity = (int) (_size * 1.75f);
		if (_capacity < 8) _capacity = 8;
		_buffer = spine::SpineExtension::realloc<Vector3>(_buffer, _capacity, __FILE__, __LINE__);
        // use simple memset to clear newly allocated space
        // size_t bytesToInit = (_size - oldSize) * sizeof(Vector3);
        // memset(_buffer + oldSize, 0, bytesToInit);
	}     
}

template<>
void Vector<Vector4>::setSizeWithoutConstruct(size_t newSize) {
	assert(newSize >= 0);
	size_t oldSize = _size;
	_size = newSize;
	if (_capacity < newSize) {
		_capacity = (int) (_size * 1.75f);
		if (_capacity < 8) _capacity = 8;
		_buffer = spine::SpineExtension::realloc<Vector4>(_buffer, _capacity, __FILE__, __LINE__);
        // use simple memset to clear newly allocated space
        // size_t bytesToInit = (_size - oldSize) * sizeof(Vector4);
        // memset(_buffer + oldSize, 0, bytesToInit);
	}     
}

template<>
void Vector<float>::setSizeWithoutConstruct(size_t newSize) {
	assert(newSize >= 0);
	size_t oldSize = _size;
	_size = newSize;
	if (_capacity < newSize) {
		_capacity = (int) (_size * 1.75f);
		if (_capacity < 8) _capacity = 8;
		_buffer = spine::SpineExtension::realloc<float>(_buffer, _capacity, __FILE__, __LINE__);
        // use simple memset to clear newly allocated space
        // size_t bytesToInit = (_size - oldSize) * sizeof(float);
        // memset(_buffer + oldSize, 0, bytesToInit);
	}     
}

template<>
void Vector<int>::setSizeWithoutConstruct(size_t newSize) {
	assert(newSize >= 0);
	size_t oldSize = _size;
	_size = newSize;
	if (_capacity < newSize) {
		_capacity = (int) (_size * 1.75f);
		if (_capacity < 8) _capacity = 8;
		_buffer = spine::SpineExtension::realloc<int>(_buffer, _capacity, __FILE__, __LINE__);
        // use simple memset to clear newly allocated space
        // size_t bytesToInit = (_size - oldSize) * sizeof(int);
        // memset(_buffer + oldSize, 0, bytesToInit);
	}     
}

void spine::MeshGenerator::BuildMeshWithArrays(SkeletonRendererInstruction& instruction, bool updateTriangles)
{
    bool canvasGroupTintBlack = settings.tintBlack && settings.canvasGroupTintBlack;
    int totalVertexCount = instruction.rawVertexCount;

    {
        if (totalVertexCount > vertexBuffer.size())
        {

            // vertexBuffer.setSize(totalVertexCount, Vector3());
            // uvBuffer.setSize(totalVertexCount, Vector2());
            // colorBuffer.setSize(totalVertexCount, Color());
            vertexBuffer.setSizeWithoutConstruct(totalVertexCount);
            uvBuffer.setSizeWithoutConstruct(totalVertexCount);
            colorBuffer.setSizeWithoutConstruct(totalVertexCount);
        }
    }

    // Color color;
    Vector4 color;

    int vertexIndex = 0;
    auto &tempVerts = this->tempVerts;
    int lastSlotIndex = 0;

    auto &bmin = meshBoundsMin;
    auto &bmax = meshBoundsMax;

    for (int si = 0, n = instruction.submeshInstructions.size(); si < n; si++)
    {
        SubmeshInstruction* submesh = instruction.submeshInstructions[si];
        Skeleton *skeleton = submesh->skeleton;
        Vector<Slot *> &drawOrderItems = (skeleton->getDrawOrder());
        const Color& skeletonColor = skeleton->getColor();
        const float& a = skeletonColor.a, &b = skeletonColor.b, &g = skeletonColor.g, &r = skeletonColor.r;

        const int& endSlot = submesh->endSlot;
        const int& startSlot = submesh->startSlot;
        lastSlotIndex = endSlot;

        if (settings.tintBlack)
        {
            Vector2 rg;
            Vector2 b2;
            int vi = vertexIndex;
            b2.y = 1.0;

            {
                if (uv2 == nullptr)
                {
                    uv2 = new Vector<Vector2>();
                    uv3 = new Vector<Vector2>();
                }
                if (totalVertexCount > uv2->size())
                {
                    uv2->setSizeWithoutConstruct(totalVertexCount);
                    uv3->setSizeWithoutConstruct(totalVertexCount);
                }
            }

            for (int slotIndex = startSlot; slotIndex < endSlot; slotIndex++)
            {
                Slot *slot = drawOrderItems[slotIndex];
                if (!slot->getBone().isActive())
                    continue;

                Attachment *attachment = slot->getAttachment();

                const Color& darkColor = slot->getDarkColor();            
                rg.x = darkColor.r;
                rg.y = darkColor.g;
                b2.x = darkColor.b;
                b2.y = 1.0;

                if (attachment->getRTTI().instanceOf(RegionAttachment::rtti))
                {
                    RegionAttachment *regionAttachment = static_cast<RegionAttachment *>(attachment);
                    if (settings.pmaVertexColors)
                    {
                        rg.x *= a;
                        rg.y *= a;
                        b2.x *= a;
                        b2.y = slot->getData().getBlendMode() == BlendMode_Additive ? 0 : (a * slot->getColor().a * regionAttachment->getColor().a);
                    }
                    (*uv2)[vi] = rg;
                    (*uv2)[vi + 1] = rg;
                    (*uv2)[vi + 2] = rg;
                    (*uv2)[vi + 3] = rg;
                    (*uv3)[vi] = b2;
                    (*uv3)[vi + 1] = b2;
                    (*uv3)[vi + 2] = b2;
                    (*uv3)[vi + 3] = b2;
                    vi += 4;
                }
                else
                {

                    if (attachment->getRTTI().instanceOf(MeshAttachment::rtti))
                    {
                        MeshAttachment *meshAttachment = static_cast<MeshAttachment *>(attachment);
                        if (settings.pmaVertexColors)
                        {
                            float alpha = a * slot->getColor().a * meshAttachment->getColor().a;
                            rg.x *= alpha;
                            rg.y *= alpha;
                            b2.x *= alpha;
                            b2.y = slot->getData().getBlendMode() == BlendMode_Additive ? 0 : alpha;
                        }
                        const size_t& verticesArrayLength = meshAttachment->getWorldVerticesLength();
                        for (int iii = 0; iii < verticesArrayLength; iii += 2)
                        {
                            (*uv2)[vi] = rg;
                            (*uv3)[vi] = b2;
                            vi++;
                        }
                    }
                }
            }
        }

        for (int slotIndex = startSlot; slotIndex < endSlot; slotIndex++)
        {
            Slot *slot = drawOrderItems[slotIndex];
            if (!slot->getBone().isActive())
                continue;

            Attachment *attachment = slot->getAttachment();
            float z = slotIndex * settings.zSpacing;

            if (attachment == nullptr)
            {
                continue;
            }

            if (attachment->getRTTI().instanceOf(RegionAttachment::rtti))
            {
                RegionAttachment *regionAttachment = static_cast<RegionAttachment *>(attachment);
                regionAttachment->computeWorldVertices(*slot, tempVerts, 0);

                const float &x1 = tempVerts[RegionAttachment::BLX], &y1 = tempVerts[RegionAttachment::BLY];
                const float &x2 = tempVerts[RegionAttachment::ULX], &y2 = tempVerts[RegionAttachment::ULY];
                const float &x3 = tempVerts[RegionAttachment::URX], &y3 = tempVerts[RegionAttachment::URY];
                const float &x4 = tempVerts[RegionAttachment::BRX], &y4 = tempVerts[RegionAttachment::BRY];

                vertexBuffer[vertexIndex].x = x1;
                vertexBuffer[vertexIndex].y = y1;
                vertexBuffer[vertexIndex].z = z;
                vertexBuffer[vertexIndex + 1].x = x4;
                vertexBuffer[vertexIndex + 1].y = y4;
                vertexBuffer[vertexIndex + 1].z = z;
                vertexBuffer[vertexIndex + 2].x = x2;
                vertexBuffer[vertexIndex + 2].y = y2;
                vertexBuffer[vertexIndex + 2].z = z;
                vertexBuffer[vertexIndex + 3].x = x3;
                vertexBuffer[vertexIndex + 3].y = y3;
                vertexBuffer[vertexIndex + 3].z = z;

                if (settings.pmaVertexColors)
                {
                    color.w = (a * slot->getColor().a * regionAttachment->getColor().a );
                    color.x = (r * slot->getColor().r * regionAttachment->getColor().r * color.w);
                    color.y = (g * slot->getColor().g * regionAttachment->getColor().g * color.w);
                    color.z = (b * slot->getColor().b * regionAttachment->getColor().b * color.w);
                    if (slot->getData().getBlendMode() == BlendMode_Additive && !canvasGroupTintBlack)
                        color.w = 0;
                }
                else
                {
                    color.w = (a * slot->getColor().a * regionAttachment->getColor().a );
                    color.x = (r * slot->getColor().r * regionAttachment->getColor().r );
                    color.y = (g * slot->getColor().g * regionAttachment->getColor().g );
                    color.z = (b * slot->getColor().b * regionAttachment->getColor().b );
                }

                colorBuffer[vertexIndex] = color;
                colorBuffer[vertexIndex + 1] = color;
                colorBuffer[vertexIndex + 2] = color;
                colorBuffer[vertexIndex + 3] = color;

                Vector<float>& regionUVs = regionAttachment->getUVs();
                uvBuffer[vertexIndex].x = regionUVs[RegionAttachment::BLX]; uvBuffer[vertexIndex].y = regionUVs[RegionAttachment::BLY];
                uvBuffer[vertexIndex + 1].x = regionUVs[RegionAttachment::BRX]; uvBuffer[vertexIndex + 1].y = regionUVs[RegionAttachment::BRY];
                uvBuffer[vertexIndex + 2].x = regionUVs[RegionAttachment::ULX]; uvBuffer[vertexIndex + 2].y = regionUVs[RegionAttachment::ULY];
                uvBuffer[vertexIndex + 3].x = regionUVs[RegionAttachment::URX]; uvBuffer[vertexIndex + 3].y = regionUVs[RegionAttachment::URY];

                if (x1 < bmin.x) bmin.x = x1; // Potential first attachment bounds initialization. Initial min should not block initial max. Same for Y below.
                if (x1 > bmax.x) bmax.x = x1;
                if (x2 < bmin.x) bmin.x = x2;
                else if (x2 > bmax.x) bmax.x = x2;
                if (x3 < bmin.x) bmin.x = x3;
                else if (x3 > bmax.x) bmax.x = x3;
                if (x4 < bmin.x) bmin.x = x4;
                else if (x4 > bmax.x) bmax.x = x4;


                if (y1 < bmin.y) bmin.y = y1;
                if (y1 > bmax.y) bmax.y = y1;
                if (y2 < bmin.y) bmin.y = y2;
                else if (y2 > bmax.y) bmax.y = y2;
                if (y3 < bmin.y) bmin.y = y3;
                else if (y3 > bmax.y) bmax.y = y3;
                if (y4 < bmin.y) bmin.y = y4;
                else if (y4 > bmax.y) bmax.y = y4;

                vertexIndex += 4;
            }
            else
            {
                MeshAttachment* meshAttachment = static_cast<MeshAttachment*>(attachment);
                if (attachment->getRTTI().instanceOf(MeshAttachment::rtti))
                {
                    int vertexArrayLength = meshAttachment->getWorldVerticesLength();
                    if (tempVerts.size() < vertexArrayLength)
                        tempVerts.setSizeWithoutConstruct(vertexArrayLength);
                    meshAttachment->computeWorldVertices(*slot, tempVerts);

                    if (settings.pmaVertexColors)
                    {
                        color.w = (a * slot->getColor().a * meshAttachment->getColor().a );
                        color.x = (r * slot->getColor().r * meshAttachment->getColor().r * color.w);
                        color.y = (g * slot->getColor().g * meshAttachment->getColor().g * color.w);
                        color.z = (b * slot->getColor().b * meshAttachment->getColor().b * color.w);
                        if (slot->getData().getBlendMode() == BlendMode_Additive && !canvasGroupTintBlack)
                            color.w = 0;
                    }
                    else
                    {
                        color.w = (a * slot->getColor().a * meshAttachment->getColor().a );
                        color.z = (b * slot->getColor().b * meshAttachment->getColor().b );
                        color.x = (r * slot->getColor().r * meshAttachment->getColor().r );
                        color.y = (g * slot->getColor().g * meshAttachment->getColor().g );
                    }

                    Vector<float> &attachmentUVs = meshAttachment->getUVs();

                    // Potential first attachment bounds initialization. See conditions in RegionAttachment logic.
                    if (vertexIndex == 0)
                    {
                        // Initial min should not block initial max.
                        // vi == vertexIndex does not always mean the bounds are fresh. It could be a submesh. Do not nuke old values by omitting the check.
                        // Should know that this is the first attachment in the submesh. slotIndex == startSlot could be an empty slot.
                        float &fx = tempVerts[0], &fy = tempVerts[1];
                        if (fx < bmin.x) bmin.x = fx;
                        if (fx > bmax.x) bmax.x = fx;
                        if (fy < bmin.y) bmin.y = fy;
                        if (fy > bmax.y) bmax.y = fy;
                    }

                    for (int iii = 0; iii < vertexArrayLength; iii += 2)
                    {
                        float &x = tempVerts[iii], &y = tempVerts[iii + 1];
                        vertexBuffer[vertexIndex].x = x; vertexBuffer[vertexIndex].y = y; vertexBuffer[vertexIndex].z = z;
                        colorBuffer[vertexIndex] = color; uvBuffer[vertexIndex].x = attachmentUVs[iii]; uvBuffer[vertexIndex].y = attachmentUVs[iii + 1];

                        if (x < bmin.x) bmin.x = x;
                        else if (x > bmax.x) bmax.x = x;

                        if (y < bmin.y) bmin.y = y;
                        else if (y > bmax.y) bmax.y = y;

                        vertexIndex++;
                    }
                }
            }
        }
    }

    //meshBoundsMin = bmin;
    //meshBoundsMax = bmax;
    meshBoundsThickness = lastSlotIndex * settings.zSpacing;

    int submeshInstructionCount = instruction.submeshInstructions.size();
    subMeshes.setSize(submeshInstructionCount, Vector<int>());

    if (updateTriangles)
    {
        if (subMeshes.size() < submeshInstructionCount)
        {
            subMeshes.setSize(submeshInstructionCount, Vector<int>());
            //for (int i = 0, n = submeshInstructionCount; i < n; i++)
            //{
                //Vector<int>& subMeshBuffer = subMeshes[i];
                //subMeshBuffer.clear();
            //}
        }

        int attachmentFirstVertex = 0;
        for (int smbi = 0; smbi < submeshInstructionCount; smbi++)
        {
            SubmeshInstruction *submeshInstruction = instruction.submeshInstructions[smbi];
            Vector<int> &currentSubmeshBuffer = subMeshes[smbi];
            { // submesh.Resize(submesh.rawTriangleCount);
                int &newTriangleCount = submeshInstruction->rawTriangleCount;
                if (newTriangleCount > currentSubmeshBuffer.size())
                    currentSubmeshBuffer.setSizeWithoutConstruct(newTriangleCount);
                else if (newTriangleCount < currentSubmeshBuffer.size())
                {
                    // Zero the extra.
                    // Vector<int> &sbi = currentSubmeshBuffer;
                    // for (int ei = newTriangleCount, nn = sbi.size(); ei < nn; ei++)
                    //     sbi[ei] = 0;
                    int* sbmBuffer = currentSubmeshBuffer.buffer();
                    memset(&sbmBuffer[newTriangleCount], 0, sizeof(int) * (currentSubmeshBuffer.size() - newTriangleCount));
                }
                // currentSubmeshBuffer.Count = newTriangleCount;
            }

            Vector<int> &tris = currentSubmeshBuffer;
            int triangleIndex = 0;
            Skeleton *skeleton = submeshInstruction->skeleton;
            auto &drawOrderItems = skeleton->getDrawOrder();
            for (int slotIndex = submeshInstruction->startSlot, endSlot = submeshInstruction->endSlot; slotIndex < endSlot; slotIndex++)
            {
                Slot *slot = drawOrderItems[slotIndex];
                if (!slot->getBone().isActive())
                    continue;

                Attachment *attachment = drawOrderItems[slotIndex]->getAttachment();
                if (attachment == nullptr)
                {
                    continue;
                }
                if (attachment->getRTTI().instanceOf(RegionAttachment::rtti))
                {
                    tris[triangleIndex] = attachmentFirstVertex;
                    tris[triangleIndex + 1] = attachmentFirstVertex + 2;
                    tris[triangleIndex + 2] = attachmentFirstVertex + 1;
                    tris[triangleIndex + 3] = attachmentFirstVertex + 2;
                    tris[triangleIndex + 4] = attachmentFirstVertex + 3;
                    tris[triangleIndex + 5] = attachmentFirstVertex + 1;
                    triangleIndex += 6;
                    attachmentFirstVertex += 4;
                    continue;
                }

                if (attachment->getRTTI().instanceOf(MeshAttachment::rtti))
                {
                    MeshAttachment * meshAttachment = static_cast<MeshAttachment *>(attachment);
                    Vector<unsigned short>& attachmentTriangles = meshAttachment->getTriangles();
                    for (int ii = 0, nn = attachmentTriangles.size(); ii < nn; ii++, triangleIndex++)
                        tris[triangleIndex] = attachmentFirstVertex + attachmentTriangles[ii];
                    attachmentFirstVertex += meshAttachment->getWorldVerticesLength() >> 1; // length/2;
                }
            }
        }
    }
}


bool MeshGenerator::IsInstructionNotEqual()
{
    SkeletonRendererInstruction& a = oldInstruction[instructionIndex];
    SkeletonRendererInstruction& b = newInstruction;

    if (a.hasActiveClipping || b.hasActiveClipping) return true;

    if (a.rawVertexCount != b.rawVertexCount) return true;

    if (a.immutableTriangles != b.immutableTriangles) return true;

    int attachmentCountB = b.attachments.size();
    if (a.attachments.size() != attachmentCountB) return true;

    int submeshCountA = a.submeshInstructions.size();
    int submeshCountB = b.submeshInstructions.size();
    if (submeshCountA != submeshCountB) return true;

    for (int i = 0; i < attachmentCountB; ++i)
    {
        if ((a.attachments)[i] != (b.attachments)[i]) return true;
    }

    for (int i = 0; i < submeshCountB; ++i)
    {
        SubmeshInstruction* submeshA = a.submeshInstructions[i];
        SubmeshInstruction* submeshB = b.submeshInstructions[i];

        if (!(
            submeshA->rawVertexCount == submeshB->rawVertexCount &&
            submeshA->startSlot == submeshB->startSlot &&
            submeshA->endSlot == submeshB->endSlot &&
            submeshA->rawTriangleCount == submeshB->rawTriangleCount &&
            submeshA->rawFirstVertexIndex == submeshB->rawFirstVertexIndex
            ))
        {
            return true;
        }
    }

    return false;
}



void MeshGenerator::BuildMeshWithArraysWithBuffersColor32(SkeletonRendererInstruction& instruction, bool updateTriangles, Vector3* vertexBuffer, Vector2* uvBuffer, uint32_t* colorBuffer)
{
    bool canvasGroupTintBlack = settings.tintBlack && settings.canvasGroupTintBlack;
    int totalVertexCount = instruction.rawVertexCount;

    // Color color;
    //Vector4 color;
    uint32_t color;

    int vertexIndex = 0;
    auto& tempVerts = this->tempVerts;
    int lastSlotIndex = 0;

    auto& bmin = meshBoundsMin;
    auto& bmax = meshBoundsMax;

    for (int si = 0, n = instruction.submeshInstructions.size(); si < n; si++)
    {
        SubmeshInstruction* submesh = instruction.submeshInstructions[si];
        Skeleton* skeleton = submesh->skeleton;
        Vector<Slot*>& drawOrderItems = (skeleton->getDrawOrder());
        const Color& skeletonColor = skeleton->getColor();
        const float& a = skeletonColor.a, & b = skeletonColor.b, & g = skeletonColor.g, & r = skeletonColor.r;

        const int& endSlot = submesh->endSlot;
        const int& startSlot = submesh->startSlot;
        lastSlotIndex = endSlot;

        uint8_t tempColorArray[4];
        uint8_t& tempA = tempColorArray[AINDEX];
        uint8_t& tempR = tempColorArray[RINDEX];
        uint8_t& tempG = tempColorArray[GINDEX];
        uint8_t& tempB = tempColorArray[BINDEX];
        uint32_t& tempColor = *(uint32_t*)tempColorArray;

        if (settings.tintBlack)
        {
            Vector2 rg;
            Vector2 b2;
            int vi = vertexIndex;
            b2.y = 1.0;

            {
                if (uv2 == nullptr)
                {
                    uv2 = new Vector<Vector2>();
                    uv3 = new Vector<Vector2>();
                }
                if (totalVertexCount > uv2->size())
                {
                    uv2->setSizeWithoutConstruct(totalVertexCount);
                    uv3->setSizeWithoutConstruct(totalVertexCount);
                }
            }

            for (int slotIndex = startSlot; slotIndex < endSlot; slotIndex++)
            {
                Slot* slot = drawOrderItems[slotIndex];
                if (!slot->getBone().isActive())
                    continue;

                Attachment* attachment = slot->getAttachment();

                const Color& darkColor = slot->getDarkColor();
                rg.x = darkColor.r;
                rg.y = darkColor.g;
                b2.x = darkColor.b;
                b2.y = 1.0;

                if (attachment->getRTTI().instanceOf(RegionAttachment::rtti))
                {
                    RegionAttachment* regionAttachment = static_cast<RegionAttachment*>(attachment);
                    if (settings.pmaVertexColors)
                    {
                        rg.x *= a;
                        rg.y *= a;
                        b2.x *= a;
                        b2.y = slot->getData().getBlendMode() == BlendMode_Additive ? 0 : (a * slot->getColor().a * regionAttachment->getColor().a);
                    }
                    (*uv2)[vi] = rg;
                    (*uv2)[vi + 1] = rg;
                    (*uv2)[vi + 2] = rg;
                    (*uv2)[vi + 3] = rg;
                    (*uv3)[vi] = b2;
                    (*uv3)[vi + 1] = b2;
                    (*uv3)[vi + 2] = b2;
                    (*uv3)[vi + 3] = b2;
                    vi += 4;
                }
                else
                {

                    if (attachment->getRTTI().instanceOf(MeshAttachment::rtti))
                    {
                        MeshAttachment* meshAttachment = static_cast<MeshAttachment*>(attachment);
                        if (settings.pmaVertexColors)
                        {
                            float alpha = a * slot->getColor().a * meshAttachment->getColor().a;
                            rg.x *= alpha;
                            rg.y *= alpha;
                            b2.x *= alpha;
                            b2.y = slot->getData().getBlendMode() == BlendMode_Additive ? 0 : alpha;
                        }
                        const size_t& verticesArrayLength = meshAttachment->getWorldVerticesLength();
                        for (int iii = 0; iii < verticesArrayLength; iii += 2)
                        {
                            (*uv2)[vi] = rg;
                            (*uv3)[vi] = b2;
                            vi++;
                        }
                    }
                }
            }
        }

        for (int slotIndex = startSlot; slotIndex < endSlot; slotIndex++)
        {
            Slot* slot = drawOrderItems[slotIndex];
            if (!slot->getBone().isActive())
                continue;

            Attachment* attachment = slot->getAttachment();
            float z = slotIndex * settings.zSpacing;

            if (attachment == nullptr)
            {
                continue;
            }

            if (attachment->getRTTI().instanceOf(RegionAttachment::rtti))
            {
                RegionAttachment* regionAttachment = static_cast<RegionAttachment*>(attachment);
                regionAttachment->computeWorldVertices(*slot, tempVerts, 0);

                const float& x1 = tempVerts[RegionAttachment::BLX], & y1 = tempVerts[RegionAttachment::BLY];
                const float& x2 = tempVerts[RegionAttachment::ULX], & y2 = tempVerts[RegionAttachment::ULY];
                const float& x3 = tempVerts[RegionAttachment::URX], & y3 = tempVerts[RegionAttachment::URY];
                const float& x4 = tempVerts[RegionAttachment::BRX], & y4 = tempVerts[RegionAttachment::BRY];

                vertexBuffer[vertexIndex].x = x1;
                vertexBuffer[vertexIndex].y = y1;
                vertexBuffer[vertexIndex].z = z;
                vertexBuffer[vertexIndex + 1].x = x4;
                vertexBuffer[vertexIndex + 1].y = y4;
                vertexBuffer[vertexIndex + 1].z = z;
                vertexBuffer[vertexIndex + 2].x = x2;
                vertexBuffer[vertexIndex + 2].y = y2;
                vertexBuffer[vertexIndex + 2].z = z;
                vertexBuffer[vertexIndex + 3].x = x3;
                vertexBuffer[vertexIndex + 3].y = y3;
                vertexBuffer[vertexIndex + 3].z = z;

                if (settings.pmaVertexColors)
                {
                    //color.w = (a * slot->getColor().a * regionAttachment->getColor().a);
                    //color.x = (r * slot->getColor().r * regionAttachment->getColor().r * color.w);
                    //color.y = (g * slot->getColor().g * regionAttachment->getColor().g * color.w);
                    //color.z = (b * slot->getColor().b * regionAttachment->getColor().b * color.w);
                    tempA = (uint8_t)(a * slot->getColor().a * regionAttachment->getColor().a * 255);
                    tempR = (uint8_t)(r * slot->getColor().r * regionAttachment->getColor().r * tempA);
                    tempG = (uint8_t)(g * slot->getColor().g * regionAttachment->getColor().g * tempA);
                    tempB = (uint8_t)(b * slot->getColor().b * regionAttachment->getColor().b * tempA);
                    if (slot->getData().getBlendMode() == BlendMode_Additive && !canvasGroupTintBlack)
                        tempA = 0;
                }
                else
                {
                    tempA = (uint8_t)(a * slot->getColor().a * regionAttachment->getColor().a * 255);
                    tempR = (uint8_t)(r * slot->getColor().r * regionAttachment->getColor().r * 255);
                    tempG = (uint8_t)(g * slot->getColor().g * regionAttachment->getColor().g * 255);
                    tempB = (uint8_t)(b * slot->getColor().b * regionAttachment->getColor().b * 255);
                }

                //color = (tempA << 24) | (tempR << 16) | (tempG << 8) | (tempB);
                color = tempColor;
                colorBuffer[vertexIndex] = color;
                colorBuffer[vertexIndex + 1] = color;
                colorBuffer[vertexIndex + 2] = color;
                colorBuffer[vertexIndex + 3] = color;

                Vector<float>& regionUVs = regionAttachment->getUVs();
                uvBuffer[vertexIndex].x = regionUVs[RegionAttachment::BLX]; uvBuffer[vertexIndex].y = regionUVs[RegionAttachment::BLY];
                uvBuffer[vertexIndex + 1].x = regionUVs[RegionAttachment::BRX]; uvBuffer[vertexIndex + 1].y = regionUVs[RegionAttachment::BRY];
                uvBuffer[vertexIndex + 2].x = regionUVs[RegionAttachment::ULX]; uvBuffer[vertexIndex + 2].y = regionUVs[RegionAttachment::ULY];
                uvBuffer[vertexIndex + 3].x = regionUVs[RegionAttachment::URX]; uvBuffer[vertexIndex + 3].y = regionUVs[RegionAttachment::URY];

                if (x1 < bmin.x) bmin.x = x1; // Potential first attachment bounds initialization. Initial min should not block initial max. Same for Y below.
                if (x1 > bmax.x) bmax.x = x1;
                if (x2 < bmin.x) bmin.x = x2;
                else if (x2 > bmax.x) bmax.x = x2;
                if (x3 < bmin.x) bmin.x = x3;
                else if (x3 > bmax.x) bmax.x = x3;
                if (x4 < bmin.x) bmin.x = x4;
                else if (x4 > bmax.x) bmax.x = x4;


                if (y1 < bmin.y) bmin.y = y1;
                if (y1 > bmax.y) bmax.y = y1;
                if (y2 < bmin.y) bmin.y = y2;
                else if (y2 > bmax.y) bmax.y = y2;
                if (y3 < bmin.y) bmin.y = y3;
                else if (y3 > bmax.y) bmax.y = y3;
                if (y4 < bmin.y) bmin.y = y4;
                else if (y4 > bmax.y) bmax.y = y4;

                vertexIndex += 4;
            }
            else
            {
                MeshAttachment* meshAttachment = static_cast<MeshAttachment*>(attachment);
                if (attachment->getRTTI().instanceOf(MeshAttachment::rtti))
                {
                    int vertexArrayLength = meshAttachment->getWorldVerticesLength();
                    if (tempVerts.size() < vertexArrayLength)
                        tempVerts.setSizeWithoutConstruct(vertexArrayLength);
                    meshAttachment->computeWorldVertices(*slot, tempVerts);

                    if (settings.pmaVertexColors)
                    {
                        //color.w = (a * slot->getColor().a * meshAttachment->getColor().a);
                        //color.x = (r * slot->getColor().r * meshAttachment->getColor().r * color.w);
                        //color.y = (g * slot->getColor().g * meshAttachment->getColor().g * color.w);
                        //color.z = (b * slot->getColor().b * meshAttachment->getColor().b * color.w);
                        tempA = (uint8_t)(a * slot->getColor().a * meshAttachment->getColor().a * 255);
                        tempR = (uint8_t)(r * slot->getColor().r * meshAttachment->getColor().r * tempA);
                        tempG = (uint8_t)(g * slot->getColor().g * meshAttachment->getColor().g * tempA);
                        tempB = (uint8_t)(b * slot->getColor().b * meshAttachment->getColor().b * tempA);
                        if (slot->getData().getBlendMode() == BlendMode_Additive && !canvasGroupTintBlack)
                            tempA = 0;
                    }
                    else
                    {
                        //color.w = (a * slot->getColor().a * meshAttachment->getColor().a);
                        //color.z = (b * slot->getColor().b * meshAttachment->getColor().b);
                        //color.x = (r * slot->getColor().r * meshAttachment->getColor().r);
                        //color.y = (g * slot->getColor().g * meshAttachment->getColor().g);
                        tempA = (uint8_t)(a * slot->getColor().a * meshAttachment->getColor().a * 255);
                        tempR = (uint8_t)(r * slot->getColor().r * meshAttachment->getColor().r * 255);
                        tempG = (uint8_t)(g * slot->getColor().g * meshAttachment->getColor().g * 255);
                        tempB = (uint8_t)(b * slot->getColor().b * meshAttachment->getColor().b * 255);
                    }

                    //color = (tempA << 24) | (tempR << 16) | (tempG << 8) | (tempB);
                    color = tempColor;

                    Vector<float>& attachmentUVs = meshAttachment->getUVs();

                    // Potential first attachment bounds initialization. See conditions in RegionAttachment logic.
                    if (vertexIndex == 0)
                    {
                        // Initial min should not block initial max.
                        // vi == vertexIndex does not always mean the bounds are fresh. It could be a submesh. Do not nuke old values by omitting the check.
                        // Should know that this is the first attachment in the submesh. slotIndex == startSlot could be an empty slot.
                        float& fx = tempVerts[0], & fy = tempVerts[1];
                        if (fx < bmin.x) bmin.x = fx;
                        if (fx > bmax.x) bmax.x = fx;
                        if (fy < bmin.y) bmin.y = fy;
                        if (fy > bmax.y) bmax.y = fy;
                    }

                    for (int iii = 0; iii < vertexArrayLength; iii += 2)
                    {
                        float& x = tempVerts[iii], & y = tempVerts[iii + 1];
                        vertexBuffer[vertexIndex].x = x; vertexBuffer[vertexIndex].y = y; vertexBuffer[vertexIndex].z = z;
                        colorBuffer[vertexIndex] = color; uvBuffer[vertexIndex].x = attachmentUVs[iii]; uvBuffer[vertexIndex].y = attachmentUVs[iii + 1];

                        if (x < bmin.x) bmin.x = x;
                        else if (x > bmax.x) bmax.x = x;

                        if (y < bmin.y) bmin.y = y;
                        else if (y > bmax.y) bmax.y = y;

                        vertexIndex++;
                    }
                }
            }
        }
    }

    //meshBoundsMin = bmin;
    //meshBoundsMax = bmax;
    meshBoundsThickness = lastSlotIndex * settings.zSpacing;

    int submeshInstructionCount = instruction.submeshInstructions.size();
    subMeshes.setSize(submeshInstructionCount, Vector<int>());

    if (updateTriangles)
    {
        if (subMeshes.size() < submeshInstructionCount)
        {
            subMeshes.setSize(submeshInstructionCount, Vector<int>());
            //for (int i = 0, n = submeshInstructionCount; i < n; i++)
            //{
                //Vector<int>& subMeshBuffer = subMeshes[i];
                //subMeshBuffer.clear();
            //}
        }

        int attachmentFirstVertex = 0;
        for (int smbi = 0; smbi < submeshInstructionCount; smbi++)
        {
            SubmeshInstruction* submeshInstruction = instruction.submeshInstructions[smbi];
            Vector<int>& currentSubmeshBuffer = subMeshes[smbi];
            { // submesh.Resize(submesh.rawTriangleCount);
                int& newTriangleCount = submeshInstruction->rawTriangleCount;
                if (newTriangleCount > currentSubmeshBuffer.size())
                    currentSubmeshBuffer.setSizeWithoutConstruct(newTriangleCount);
                else if (newTriangleCount < currentSubmeshBuffer.size())
                {
                    // Zero the extra.
                    // Vector<int> &sbi = currentSubmeshBuffer;
                    // for (int ei = newTriangleCount, nn = sbi.size(); ei < nn; ei++)
                    //     sbi[ei] = 0;
                    int* sbmBuffer = currentSubmeshBuffer.buffer();
                    memset(&sbmBuffer[newTriangleCount], 0, sizeof(int) * (currentSubmeshBuffer.size() - newTriangleCount));
                }
                // currentSubmeshBuffer.Count = newTriangleCount;
            }

            Vector<int>& tris = currentSubmeshBuffer;
            int triangleIndex = 0;
            Skeleton* skeleton = submeshInstruction->skeleton;
            auto& drawOrderItems = skeleton->getDrawOrder();
            for (int slotIndex = submeshInstruction->startSlot, endSlot = submeshInstruction->endSlot; slotIndex < endSlot; slotIndex++)
            {
                Slot* slot = drawOrderItems[slotIndex];
                if (!slot->getBone().isActive())
                    continue;

                Attachment* attachment = drawOrderItems[slotIndex]->getAttachment();
                if (attachment == nullptr)
                {
                    continue;
                }
                if (attachment->getRTTI().instanceOf(RegionAttachment::rtti))
                {
                    tris[triangleIndex] = attachmentFirstVertex;
                    tris[triangleIndex + 1] = attachmentFirstVertex + 2;
                    tris[triangleIndex + 2] = attachmentFirstVertex + 1;
                    tris[triangleIndex + 3] = attachmentFirstVertex + 2;
                    tris[triangleIndex + 4] = attachmentFirstVertex + 3;
                    tris[triangleIndex + 5] = attachmentFirstVertex + 1;
                    triangleIndex += 6;
                    attachmentFirstVertex += 4;
                    continue;
                }

                if (attachment->getRTTI().instanceOf(MeshAttachment::rtti))
                {
                    MeshAttachment* meshAttachment = static_cast<MeshAttachment*>(attachment);
                    Vector<unsigned short>& attachmentTriangles = meshAttachment->getTriangles();
                    for (int ii = 0, nn = attachmentTriangles.size(); ii < nn; ii++, triangleIndex++)
                        tris[triangleIndex] = attachmentFirstVertex + attachmentTriangles[ii];
                    attachmentFirstVertex += meshAttachment->getWorldVerticesLength() >> 1; // length/2;
                }
            }
        }
    }
}


void spine::MeshGenerator::ScaleVertexDataWithBuffers(SkeletonRendererInstruction& instruction, float scale, Vector3* vertexBuffer)
{
    //Vector<Vector3>& vbi = vertexBuffer;

    for (int i = 0, n = instruction.rawVertexCount; i < n; ++i)
    {
        vertexBuffer[i] *= scale;
    }

    meshBoundsMin.x *= scale;
    meshBoundsMin.y *= scale;
    meshBoundsMax.x *= scale;
    meshBoundsMax.y *= scale;

    meshBoundsThickness *= scale;
}

void MeshGenerator::FillSubmeshLateVertexDataWithBuffers(int submeshIndex, Vector4* tangents, int vertexCount)
{
    if (settings.calculateTangents)
    {
        //int vertexCount = vertexBuffer.size();
        auto& sbi = subMeshes;
        int submeshCount = sbi.size();
        auto& vbi = vertexBuffer;
        auto& ubi = uvBuffer;
        //if (tangents->size() != vertexBuffer.size())
        //{
        //    if (tangents == nullptr)
        //    {
        //        tangents = new Vector<Vector4>();
        //    }
        //    tangents->setSize(vertexCount, Vector4());
        //    tempTanBuffer.setSize(vertexCount * 2, Vector2());
        //}
        if (tempTanBuffer.size() > vertexCount * 2)
        {
            tempTanBuffer.setSizeWithoutConstruct(vertexCount * 2);
        }

        //for (int i = 0; i < submeshCount; i++)
        int i = submeshIndex;
        {
            auto& submesh = sbi[i];
            int triangleCount = submesh.size();
            MeshGenerator::SolveTangents2DTriangles(tempTanBuffer, submesh, triangleCount, vbi, ubi, vertexCount);
        }
        MeshGenerator::SolveTangents2DBufferWithBuffer(tangents, tempTanBuffer, vertexCount);
    }
}

void MeshGenerator::BuildMeshSubmeshWithArraysWithBuffersColor32(SkeletonRendererInstruction& instruction, int submeshIndex, bool updateTriangles, Vector3* vertexBuffer, Vector2* uvBuffer, uint32_t* colorBuffer)
{
    bool canvasGroupTintBlack = settings.tintBlack && settings.canvasGroupTintBlack;
    int totalVertexCount = instruction.rawVertexCount;

    // Color color;
    //Vector4 color;
    uint32_t color;

    int vertexIndex = 0;
    auto& tempVerts = this->tempVerts;
    int lastSlotIndex = 0;

    auto& bmin = meshBoundsMin;
    auto& bmax = meshBoundsMax;

    //for (int si = 0, n = instruction.submeshInstructions.size(); si < n; si++)
    int si = submeshIndex;
    {
        SubmeshInstruction* submesh = instruction.submeshInstructions[si];
        Skeleton* skeleton = submesh->skeleton;
        Vector<Slot*>& drawOrderItems = (skeleton->getDrawOrder());
        const Color& skeletonColor = skeleton->getColor();
        const float& a = skeletonColor.a, & b = skeletonColor.b, & g = skeletonColor.g, & r = skeletonColor.r;

        const int& endSlot = submesh->endSlot;
        const int& startSlot = submesh->startSlot;
        lastSlotIndex = endSlot;

        uint8_t tempColorArray[4];
        uint8_t& tempA = tempColorArray[AINDEX];
        uint8_t& tempR = tempColorArray[RINDEX];
        uint8_t& tempG = tempColorArray[GINDEX];
        uint8_t& tempB = tempColorArray[BINDEX];
        uint32_t& tempColor = *(uint32_t*)tempColorArray;

        if (settings.tintBlack)
        {
            Vector2 rg;
            Vector2 b2;
            int vi = vertexIndex;
            b2.y = 1.0;

            {
                if (uv2 == nullptr)
                {
                    uv2 = new Vector<Vector2>();
                    uv3 = new Vector<Vector2>();
                }
                if (totalVertexCount > uv2->size())
                {
                    uv2->setSizeWithoutConstruct(totalVertexCount);
                    uv3->setSizeWithoutConstruct(totalVertexCount);
                }
            }

            for (int slotIndex = startSlot; slotIndex < endSlot; slotIndex++)
            {
                Slot* slot = drawOrderItems[slotIndex];
                if (!slot->getBone().isActive())
                    continue;

                Attachment* attachment = slot->getAttachment();

                const Color& darkColor = slot->getDarkColor();
                rg.x = darkColor.r;
                rg.y = darkColor.g;
                b2.x = darkColor.b;
                b2.y = 1.0;

                if (attachment->getRTTI().instanceOf(RegionAttachment::rtti))
                {
                    RegionAttachment* regionAttachment = static_cast<RegionAttachment*>(attachment);
                    if (settings.pmaVertexColors)
                    {
                        rg.x *= a;
                        rg.y *= a;
                        b2.x *= a;
                        b2.y = slot->getData().getBlendMode() == BlendMode_Additive ? 0 : (a * slot->getColor().a * regionAttachment->getColor().a);
                    }
                    (*uv2)[vi] = rg;
                    (*uv2)[vi + 1] = rg;
                    (*uv2)[vi + 2] = rg;
                    (*uv2)[vi + 3] = rg;
                    (*uv3)[vi] = b2;
                    (*uv3)[vi + 1] = b2;
                    (*uv3)[vi + 2] = b2;
                    (*uv3)[vi + 3] = b2;
                    vi += 4;
                }
                else
                {

                    if (attachment->getRTTI().instanceOf(MeshAttachment::rtti))
                    {
                        MeshAttachment* meshAttachment = static_cast<MeshAttachment*>(attachment);
                        if (settings.pmaVertexColors)
                        {
                            float alpha = a * slot->getColor().a * meshAttachment->getColor().a;
                            rg.x *= alpha;
                            rg.y *= alpha;
                            b2.x *= alpha;
                            b2.y = slot->getData().getBlendMode() == BlendMode_Additive ? 0 : alpha;
                        }
                        const size_t& verticesArrayLength = meshAttachment->getWorldVerticesLength();
                        for (int iii = 0; iii < verticesArrayLength; iii += 2)
                        {
                            (*uv2)[vi] = rg;
                            (*uv3)[vi] = b2;
                            vi++;
                        }
                    }
                }
            }
        }

        for (int slotIndex = startSlot; slotIndex < endSlot; slotIndex++)
        {
            Slot* slot = drawOrderItems[slotIndex];
            if (!slot->getBone().isActive())
                continue;

            Attachment* attachment = slot->getAttachment();
            float z = slotIndex * settings.zSpacing;

            if (attachment == nullptr)
            {
                continue;
            }

            if (attachment->getRTTI().instanceOf(RegionAttachment::rtti))
            {
                RegionAttachment* regionAttachment = static_cast<RegionAttachment*>(attachment);
                regionAttachment->computeWorldVertices(*slot, tempVerts, 0);

                const float& x1 = tempVerts[RegionAttachment::BLX], & y1 = tempVerts[RegionAttachment::BLY];
                const float& x2 = tempVerts[RegionAttachment::ULX], & y2 = tempVerts[RegionAttachment::ULY];
                const float& x3 = tempVerts[RegionAttachment::URX], & y3 = tempVerts[RegionAttachment::URY];
                const float& x4 = tempVerts[RegionAttachment::BRX], & y4 = tempVerts[RegionAttachment::BRY];

                vertexBuffer[vertexIndex].x = x1;
                vertexBuffer[vertexIndex].y = y1;
                vertexBuffer[vertexIndex].z = z;
                vertexBuffer[vertexIndex + 1].x = x4;
                vertexBuffer[vertexIndex + 1].y = y4;
                vertexBuffer[vertexIndex + 1].z = z;
                vertexBuffer[vertexIndex + 2].x = x2;
                vertexBuffer[vertexIndex + 2].y = y2;
                vertexBuffer[vertexIndex + 2].z = z;
                vertexBuffer[vertexIndex + 3].x = x3;
                vertexBuffer[vertexIndex + 3].y = y3;
                vertexBuffer[vertexIndex + 3].z = z;

                if (settings.pmaVertexColors)
                {
                    //color.w = (a * slot->getColor().a * regionAttachment->getColor().a);
                    //color.x = (r * slot->getColor().r * regionAttachment->getColor().r * color.w);
                    //color.y = (g * slot->getColor().g * regionAttachment->getColor().g * color.w);
                    //color.z = (b * slot->getColor().b * regionAttachment->getColor().b * color.w);
                    tempA = (uint8_t)(a * slot->getColor().a * regionAttachment->getColor().a * 255);
                    tempR = (uint8_t)(r * slot->getColor().r * regionAttachment->getColor().r * tempA);
                    tempG = (uint8_t)(g * slot->getColor().g * regionAttachment->getColor().g * tempA);
                    tempB = (uint8_t)(b * slot->getColor().b * regionAttachment->getColor().b * tempA);
                    if (slot->getData().getBlendMode() == BlendMode_Additive && !canvasGroupTintBlack)
                        tempA = 0;
                }
                else
                {
                    tempA = (uint8_t)(a * slot->getColor().a * regionAttachment->getColor().a * 255);
                    tempR = (uint8_t)(r * slot->getColor().r * regionAttachment->getColor().r * 255);
                    tempG = (uint8_t)(g * slot->getColor().g * regionAttachment->getColor().g * 255);
                    tempB = (uint8_t)(b * slot->getColor().b * regionAttachment->getColor().b * 255);
                }

                //color = (tempA << 24) | (tempR << 16) | (tempG << 8) | (tempB);
                color = tempColor;
                colorBuffer[vertexIndex] = color;
                colorBuffer[vertexIndex + 1] = color;
                colorBuffer[vertexIndex + 2] = color;
                colorBuffer[vertexIndex + 3] = color;

                Vector<float>& regionUVs = regionAttachment->getUVs();
                uvBuffer[vertexIndex].x = regionUVs[RegionAttachment::BLX]; uvBuffer[vertexIndex].y = regionUVs[RegionAttachment::BLY];
                uvBuffer[vertexIndex + 1].x = regionUVs[RegionAttachment::BRX]; uvBuffer[vertexIndex + 1].y = regionUVs[RegionAttachment::BRY];
                uvBuffer[vertexIndex + 2].x = regionUVs[RegionAttachment::ULX]; uvBuffer[vertexIndex + 2].y = regionUVs[RegionAttachment::ULY];
                uvBuffer[vertexIndex + 3].x = regionUVs[RegionAttachment::URX]; uvBuffer[vertexIndex + 3].y = regionUVs[RegionAttachment::URY];

                if (x1 < bmin.x) bmin.x = x1; // Potential first attachment bounds initialization. Initial min should not block initial max. Same for Y below.
                if (x1 > bmax.x) bmax.x = x1;
                if (x2 < bmin.x) bmin.x = x2;
                else if (x2 > bmax.x) bmax.x = x2;
                if (x3 < bmin.x) bmin.x = x3;
                else if (x3 > bmax.x) bmax.x = x3;
                if (x4 < bmin.x) bmin.x = x4;
                else if (x4 > bmax.x) bmax.x = x4;


                if (y1 < bmin.y) bmin.y = y1;
                if (y1 > bmax.y) bmax.y = y1;
                if (y2 < bmin.y) bmin.y = y2;
                else if (y2 > bmax.y) bmax.y = y2;
                if (y3 < bmin.y) bmin.y = y3;
                else if (y3 > bmax.y) bmax.y = y3;
                if (y4 < bmin.y) bmin.y = y4;
                else if (y4 > bmax.y) bmax.y = y4;

                vertexIndex += 4;
            }
            else
            {
                MeshAttachment* meshAttachment = static_cast<MeshAttachment*>(attachment);
                if (attachment->getRTTI().instanceOf(MeshAttachment::rtti))
                {
                    int vertexArrayLength = meshAttachment->getWorldVerticesLength();
                    if (tempVerts.size() < vertexArrayLength)
                        tempVerts.setSizeWithoutConstruct(vertexArrayLength);
                    meshAttachment->computeWorldVertices(*slot, tempVerts);

                    if (settings.pmaVertexColors)
                    {
                        //color.w = (a * slot->getColor().a * meshAttachment->getColor().a);
                        //color.x = (r * slot->getColor().r * meshAttachment->getColor().r * color.w);
                        //color.y = (g * slot->getColor().g * meshAttachment->getColor().g * color.w);
                        //color.z = (b * slot->getColor().b * meshAttachment->getColor().b * color.w);
                        tempA = (uint8_t)(a * slot->getColor().a * meshAttachment->getColor().a * 255);
                        tempR = (uint8_t)(r * slot->getColor().r * meshAttachment->getColor().r * tempA);
                        tempG = (uint8_t)(g * slot->getColor().g * meshAttachment->getColor().g * tempA);
                        tempB = (uint8_t)(b * slot->getColor().b * meshAttachment->getColor().b * tempA);
                        if (slot->getData().getBlendMode() == BlendMode_Additive && !canvasGroupTintBlack)
                            tempA = 0;
                    }
                    else
                    {
                        //color.w = (a * slot->getColor().a * meshAttachment->getColor().a);
                        //color.z = (b * slot->getColor().b * meshAttachment->getColor().b);
                        //color.x = (r * slot->getColor().r * meshAttachment->getColor().r);
                        //color.y = (g * slot->getColor().g * meshAttachment->getColor().g);
                        tempA = (uint8_t)(a * slot->getColor().a * meshAttachment->getColor().a * 255);
                        tempR = (uint8_t)(r * slot->getColor().r * meshAttachment->getColor().r * 255);
                        tempG = (uint8_t)(g * slot->getColor().g * meshAttachment->getColor().g * 255);
                        tempB = (uint8_t)(b * slot->getColor().b * meshAttachment->getColor().b * 255);
                    }

                    //color = (tempA << 24) | (tempR << 16) | (tempG << 8) | (tempB);
                    color = tempColor;

                    Vector<float>& attachmentUVs = meshAttachment->getUVs();

                    // Potential first attachment bounds initialization. See conditions in RegionAttachment logic.
                    if (vertexIndex == 0)
                    {
                        // Initial min should not block initial max.
                        // vi == vertexIndex does not always mean the bounds are fresh. It could be a submesh. Do not nuke old values by omitting the check.
                        // Should know that this is the first attachment in the submesh. slotIndex == startSlot could be an empty slot.
                        float& fx = tempVerts[0], & fy = tempVerts[1];
                        if (fx < bmin.x) bmin.x = fx;
                        if (fx > bmax.x) bmax.x = fx;
                        if (fy < bmin.y) bmin.y = fy;
                        if (fy > bmax.y) bmax.y = fy;
                    }

                    for (int iii = 0; iii < vertexArrayLength; iii += 2)
                    {
                        float& x = tempVerts[iii], & y = tempVerts[iii + 1];
                        vertexBuffer[vertexIndex].x = x; vertexBuffer[vertexIndex].y = y; vertexBuffer[vertexIndex].z = z;
                        //colorBuffer[vertexIndex] = color;
                        memcpy(colorBuffer + vertexIndex, &color, sizeof(uint32_t));
                        uvBuffer[vertexIndex].x = attachmentUVs[iii]; uvBuffer[vertexIndex].y = attachmentUVs[iii + 1];

                        if (x < bmin.x) bmin.x = x;
                        else if (x > bmax.x) bmax.x = x;

                        if (y < bmin.y) bmin.y = y;
                        else if (y > bmax.y) bmax.y = y;

                        vertexIndex++;
                    }
                }
            }
        }
    }

    //meshBoundsMin = bmin;
    //meshBoundsMax = bmax;
    meshBoundsThickness = lastSlotIndex * settings.zSpacing;

    //int submeshInstructionCount = instruction.submeshInstructions.size();
    //subMeshes.setSize(submeshInstructionCount, Vector<int>());
    subMeshes.setSizeWithoutConstruct(1);

    if (updateTriangles)
    {
        //if (subMeshes.size() < submeshInstructionCount)
        {
            //subMeshes.setSize(submeshInstructionCount, Vector<int>());
            //for (int i = 0, n = submeshInstructionCount; i < n; i++)
            //{
                //Vector<int>& subMeshBuffer = subMeshes[i];
                //subMeshBuffer.clear();
            //}
        }

        int attachmentFirstVertex = 0;
        //for (int smbi = 0; smbi < submeshInstructionCount; smbi++)
        int smbi = submeshIndex;
        {
            SubmeshInstruction* submeshInstruction = instruction.submeshInstructions[smbi];
            Vector<int>& currentSubmeshBuffer = subMeshes[0];
            { // submesh.Resize(submesh.rawTriangleCount);
                int& newTriangleCount = submeshInstruction->rawTriangleCount;
                //if (newTriangleCount > currentSubmeshBuffer.size())
                    currentSubmeshBuffer.setSizeWithoutConstruct(newTriangleCount);
                //else if (newTriangleCount < currentSubmeshBuffer.size())
                //{
                    // Zero the extra.
                    // Vector<int> &sbi = currentSubmeshBuffer;
                    // for (int ei = newTriangleCount, nn = sbi.size(); ei < nn; ei++)
                    //     sbi[ei] = 0;
                    //int* sbmBuffer = currentSubmeshBuffer.buffer();
                    //memset(&sbmBuffer[newTriangleCount], 0, sizeof(int) * (currentSubmeshBuffer.size() - newTriangleCount));
                //}
                // currentSubmeshBuffer.Count = newTriangleCount;
            }

            Vector<int>& tris = currentSubmeshBuffer;
            int triangleIndex = 0;
            Skeleton* skeleton = submeshInstruction->skeleton;
            auto& drawOrderItems = skeleton->getDrawOrder();
            for (int slotIndex = submeshInstruction->startSlot, endSlot = submeshInstruction->endSlot; slotIndex < endSlot; slotIndex++)
            {
                Slot* slot = drawOrderItems[slotIndex];
                if (!slot->getBone().isActive())
                    continue;

                Attachment* attachment = drawOrderItems[slotIndex]->getAttachment();
                if (attachment == nullptr)
                {
                    continue;
                }
                if (attachment->getRTTI().instanceOf(RegionAttachment::rtti))
                {
                    tris[triangleIndex] = attachmentFirstVertex;
                    tris[triangleIndex + 1] = attachmentFirstVertex + 2;
                    tris[triangleIndex + 2] = attachmentFirstVertex + 1;
                    tris[triangleIndex + 3] = attachmentFirstVertex + 2;
                    tris[triangleIndex + 4] = attachmentFirstVertex + 3;
                    tris[triangleIndex + 5] = attachmentFirstVertex + 1;
                    triangleIndex += 6;
                    attachmentFirstVertex += 4;
                    continue;
                }

                if (attachment->getRTTI().instanceOf(MeshAttachment::rtti))
                {
                    MeshAttachment* meshAttachment = static_cast<MeshAttachment*>(attachment);
                    Vector<unsigned short>& attachmentTriangles = meshAttachment->getTriangles();
                    for (int ii = 0, nn = attachmentTriangles.size(); ii < nn; ii++, triangleIndex++)
                        tris[triangleIndex] = attachmentFirstVertex + attachmentTriangles[ii];
                    attachmentFirstVertex += meshAttachment->getWorldVerticesLength() >> 1; // length/2;
                }
            }
        }
    }
}

void MeshGenerator::ScaleSubmeshVertexDataWithBuffers(SkeletonRendererInstruction& instruction, int submeshIndex, float scale, Vector3* vertexBuffer)
{
    //Vector<Vector3>& vbi = vertexBuffer;

    for (int i = 0, n = instruction.submeshInstructions[submeshIndex]->rawVertexCount; i < n; ++i)
    {
        vertexBuffer[i] *= scale;
    }

    meshBoundsMin.x *= scale;
    meshBoundsMin.y *= scale;
    meshBoundsMax.x *= scale;
    meshBoundsMax.y *= scale;

    meshBoundsThickness *= scale;
}

void spine::MeshGenerator::getSubMeshes(int submeshIndex, int* dst)
{
    //int total = subMeshes.size(), index = 0;
    //for (int i = 0; i < total; i++)
    int index = 0;
    int i = 0;
    {
        size_t sz = subMeshes[i].size();
        memcpy(dst + index, subMeshes[i].buffer(), sizeof(int) * sz);
        index += sz;
    }
}

int spine::MeshGenerator::getColorBuffer(float *dst)
{
    int sz = colorBuffer.size();
    for (int i = 0; i < sz; i++)
    {
        // Color cur = colorBuffer[i];
        Vector4 cur = colorBuffer[i];
        dst[i * 4] = cur.x;
        dst[i * 4 + 1] = cur.y;
        dst[i * 4 + 2] = cur.z;
        dst[i * 4 + 3] = cur.w;
    }

    return sz;
}


int spine::MeshGenerator::getColorBufferVec4(Vector4* dst)
{
    int sz = colorBuffer.size();
    // for (int i = 0; i < sz; i++)
    // {
    //     // Color cur = colorBuffer[i];
    //     dst[i].x = colorBuffer[i].r;
    //     dst[i].y = colorBuffer[i].g;
    //     dst[i].z = colorBuffer[i].b;
    //     dst[i].w = colorBuffer[i].a;
    // }
    memcpy(dst, colorBuffer.buffer(), sz * sizeof(Vector4));
    return sz;
}

int spine::MeshGenerator::getVertexBuffer(Vector3 *dst)
{
    int sz = vertexBuffer.size();
    memcpy(dst, vertexBuffer.buffer(), sz * sizeof(Vector3));

    return sz;
}

int spine::MeshGenerator::getUVBuffer(Vector2 *dst)
{
    int sz = uvBuffer.size();
    memcpy(dst, uvBuffer.buffer(), sz * sizeof(Vector2));

    return sz;
}

int spine::MeshGenerator::getTangentBuffer(Vector4* dst)
{
    int sz = tangents->size();
    memcpy(dst, tangents->buffer(), sz * sizeof(Vector4));
    
    return sz;
}

size_t spine::MeshGenerator::getBufferSize()
{
    return vertexBuffer.size();
}

void spine::MeshGenerator::Begin()
{
    vertexBuffer.clear();
    colorBuffer.clear();
    uvBuffer.clear();
    clipper.clipEnd();

    {
        meshBoundsMin.x =   std::numeric_limits<float>::infinity();
        meshBoundsMin.y =   std::numeric_limits<float>::infinity();
        meshBoundsMax.x = -std::numeric_limits<float>::infinity();
        meshBoundsMax.y = -std::numeric_limits<float>::infinity();
        meshBoundsThickness = 0.0f;
    }

    submeshIndex = 0;
    subMeshes.ensureCapacity(6);
    subMeshes.setSize(1, Vector<int>());
    tempVerts.setSize(8, 0);

    instructionIndex = !instructionIndex;
}

void spine::MeshGenerator::AddAttachmentTintBlack (float r2, float g2, float b2, float a, int vertexCount) 
{
    Vector2 rg = Vector2(r2, g2);
    Vector2 bo = Vector2(b2, a);

    int ovc = vertexBuffer.size();
    int newVertexCount = ovc + vertexCount;
    {
        if (uv2 == nullptr) {
            uv2 = new Vector<Vector2>();
            uv3 = new Vector<Vector2>();
        }

        uv2->setSize(newVertexCount, Vector2());
        uv3->setSize(newVertexCount, Vector2());
    }

    Vector<Vector2>* uv2i = uv2;
    Vector<Vector2>* uv3i = uv3;
    for (int i = 0; i < vertexCount; i++) {
        (*uv2i)[ovc + i] = rg;
        (*uv3i)[ovc + i] = bo;
    }
}

void spine::MeshGenerator::AddSubmesh(SubmeshInstruction instruction, bool updateTriangles)
{
    Settings settings = this->settings;
    int newSubmeshCount = submeshIndex + 1;
    if (subMeshes.size() < newSubmeshCount)
        subMeshes.setSize(newSubmeshCount, Vector<int>());
    Vector<int> submesh = subMeshes[submeshIndex];
    if (submesh.size() == 0)
        subMeshes[submeshIndex] = submesh = Vector<int>();
    submesh.clear();

    Skeleton *skeleton = instruction.skeleton;
    Vector<Slot *> drawOrderItems = skeleton->getDrawOrder();

    // Color color;
    Vector4 color;
    float skeletonA = skeleton->getColor().a;
    float skeletonR = skeleton->getColor().r;
    float skeletonG = skeleton->getColor().g;
    float skeletonB = skeleton->getColor().b;
    Vector2 meshBoundsMin = this->meshBoundsMin, meshBoundsMax = this->meshBoundsMax;

    // Settings
    float zSpacing = settings.zSpacing;
    bool pmaVertexColors = settings.pmaVertexColors;
    bool tintBlack = settings.tintBlack;
    // #if SPINE_TRIANGLECHECK
    bool useClipping = settings.useClipping && instruction.hasClipping;
    // #else
    // 			bool useClipping = settings.useClipping;
    // #endif
    bool canvasGroupTintBlack = settings.tintBlack && settings.canvasGroupTintBlack;

    if (useClipping)
    {
        if (instruction.preActiveClippingSlotSource >= 0)
        {
            Slot* slot = drawOrderItems[instruction.preActiveClippingSlotSource];
            clipper.clipStart(*slot, (ClippingAttachment*) slot->getAttachment());
        }
    }

    for (int slotIndex = instruction.startSlot; slotIndex < instruction.endSlot; slotIndex++)
    {
        Slot* slot = drawOrderItems[slotIndex];
        if (!slot->getBone().isActive())
        {
            clipper.clipEnd(*slot);
            continue;
        }
        Attachment* attachment = slot->getAttachment();
        float z = zSpacing * slotIndex;

        Vector<float> workingVerts = this->tempVerts;
        Vector<float> uvs;
        int attachmentVertexCount;
        int attachmentIndexCount;
        int attachmentTriangleIndices[6] = {0};

        Color c;

        // Identify and prepare values.
        if (attachment->getRTTI().instanceOf(RegionAttachment::rtti))
        {
            RegionAttachment* region = static_cast<RegionAttachment*>(attachment);
            region->computeWorldVertices(*slot, workingVerts, 0);
            uvs = region->getUVs();
            
            memcpy(attachmentTriangleIndices, regionTriagles, sizeof(int) * 6);
            c.r = region->getColor().r;
            c.g = region->getColor().g;
            c.b = region->getColor().b;
            c.a = region->getColor().a;
            attachmentVertexCount = 4;
            attachmentIndexCount = 6;
        }
        else
        {
            if (attachment->getRTTI().instanceOf(MeshAttachment::rtti))
            {
                MeshAttachment* mesh = static_cast<MeshAttachment*>(attachment);
                int meshVerticesLength = mesh->getWorldVerticesLength();
                if (workingVerts.size() < meshVerticesLength)
                {
                    workingVerts.ensureCapacity(meshVerticesLength);
                    memset(workingVerts.buffer(), 0, sizeof(workingVerts[0])*vertexBuffer.size());
                    tempVerts = workingVerts;
                }
                mesh->computeWorldVertices(*slot, 0, meshVerticesLength, workingVerts, 0); // meshAttachment.ComputeWorldVertices(slot, tempVerts);
                uvs = mesh->getUVs();
                Vector<unsigned short> meshTriangles = mesh->getTriangles();
                for (int i = 0; i < 6; ++i)
                    attachmentTriangleIndices[i] = static_cast<int>(meshTriangles[i]);
                c.r = mesh->getColor().r;
                c.g = mesh->getColor().g;
                c.b = mesh->getColor().b;
                c.a = mesh->getColor().a;
                attachmentVertexCount = meshVerticesLength >> 1; // meshVertexCount / 2;
                attachmentIndexCount = mesh->getTriangles().size();
            }
            else
            {
                if (useClipping)
                {
                    if (attachment->getRTTI().instanceOf(ClippingAttachment::rtti))
                    {
                        ClippingAttachment* clippingAttachment = static_cast<ClippingAttachment*>(attachment);
                        clipper.clipStart(*slot, clippingAttachment);
                        continue;
                    }
                }

                // If not any renderable attachment.
                clipper.clipEnd(*slot);
                continue;
            }
        }

        float tintBlackAlpha = 1.0f;
        if (pmaVertexColors)
        {
            float colorA = skeletonA * slot->getColor().a * c.a;
            color.w = colorA;
            color.x = skeletonR * slot->getColor().r * c.r * color.w;
            color.y = skeletonG * slot->getColor().g * c.g * color.w;
            color.z = skeletonB * slot->getColor().b * c.b * color.w;
            if (slot->getData().getBlendMode() == BlendMode::BlendMode_Additive)
            {
                if (canvasGroupTintBlack)
                    tintBlackAlpha = 0;
                else
                    color.w = 0;
            }
            else if (canvasGroupTintBlack)
            { // other blend modes
                tintBlackAlpha = colorA;
            }
        }
        else
        {
            color.w = skeletonA * slot->getColor().a * c.a;
            color.x = skeletonR * slot->getColor().r * c.r;
            color.y = skeletonG * slot->getColor().g * c.g;
            color.z = skeletonB * slot->getColor().b * c.b;
        }

        if (useClipping && clipper.isClipping())
        {
            Vector<unsigned short> triangles;
            for (int i = 0; i < 6; ++ i)
            {
                triangles.add(static_cast<unsigned short>(attachmentTriangleIndices[i]));
            }
            clipper.clipTriangles(workingVerts.buffer(), triangles.buffer(), 6, uvs.buffer(), 2);
            workingVerts = clipper.getClippedVertices();
            attachmentVertexCount = clipper.getClippedVertices().size() >> 1;
            triangles = clipper.getClippedTriangles();
            for (int i = 0; i < 6; ++ i)
            {
                attachmentTriangleIndices[i] = static_cast<int>(triangles.buffer()[i]);
            }
            attachmentIndexCount = clipper.getClippedTriangles().size();
            uvs = clipper.getClippedUVs();
        }

        // Actually add slot/attachment data into buffers.
        if (attachmentVertexCount != 0 && attachmentIndexCount != 0)
        {
            if (tintBlack)
            {
                float r2 = slot->getDarkColor().r;
                float g2 = slot->getDarkColor().g;
                float b2 = slot->getDarkColor().b;
                if (pmaVertexColors)
                {
                    float alpha = skeletonA * slot->getColor().a * c.a;
                    r2 *= alpha;
                    g2 *= alpha;
                    b2 *= alpha;
                }
                AddAttachmentTintBlack(r2, g2, b2, tintBlackAlpha, attachmentVertexCount);
            }

            // AddAttachment(workingVerts, uvs, color, attachmentTriangleIndices, attachmentVertexCount, attachmentIndexCount, ref meshBoundsMin, ref meshBoundsMax, z);
            int ovc = vertexBuffer.size();
            // Add data to vertex buffers
            {
                int newVertexCount = ovc + attachmentVertexCount;
                int oldArraySize = vertexBuffer.size();
                if (newVertexCount > oldArraySize)
                {
                    int newArraySize = (int)(oldArraySize * 1.3f);
                    if (newArraySize < newVertexCount)
                        newArraySize = newVertexCount;
                    vertexBuffer.ensureCapacity(newArraySize);
                    uvBuffer.ensureCapacity(newArraySize);
                    colorBuffer.ensureCapacity(newArraySize);
                }
                vertexBuffer.setSize(newVertexCount, Vector3());
                uvBuffer.setSize(newVertexCount, Vector2());
                // colorBuffer.setSize(newVertexCount, Color());
                colorBuffer.setSize(newVertexCount, Vector4());
            }

            Vector<Vector3>& vbi = vertexBuffer;
            Vector<Vector2>& ubi = uvBuffer;
            // Vector<Color>& cbi = colorBuffer;
            Vector<Vector4>& cbi = colorBuffer;
            if (ovc == 0)
            {
                for (int i = 0; i < attachmentVertexCount; i++)
                {
                    int vi = ovc + i;
                    int i2 = i << 1; // i * 2
                    float x = workingVerts[i2];
                    float y = workingVerts[i2 + 1];

                    vbi[vi].x = x;
                    vbi[vi].y = y;
                    vbi[vi].z = z;
                    ubi[vi].x = uvs[i2];
                    ubi[vi].y = uvs[i2 + 1];
                    cbi[vi] = color;

                    // Calculate bounds.
                    if (x < meshBoundsMin.x)
                        meshBoundsMin.x = x;
                    if (x > meshBoundsMax.x)
                        meshBoundsMax.x = x;
                    if (y < meshBoundsMin.y)
                        meshBoundsMin.y = y;
                    if (y > meshBoundsMax.y)
                        meshBoundsMax.y = y;
                }
            }
            else
            {
                for (int i = 0; i < attachmentVertexCount; i++)
                {
                    int vi = ovc + i;
                    int i2 = i << 1; // i * 2
                    float x = workingVerts[i2];
                    float y = workingVerts[i2 + 1];

                    vbi[vi].x = x;
                    vbi[vi].y = y;
                    vbi[vi].z = z;
                    ubi[vi].x = uvs[i2];
                    ubi[vi].y = uvs[i2 + 1];
                    cbi[vi] = color;

                    // Calculate bounds.
                    if (x < meshBoundsMin.x)
                        meshBoundsMin.x = x;
                    else if (x > meshBoundsMax.x)
                        meshBoundsMax.x = x;
                    if (y < meshBoundsMin.y)
                        meshBoundsMin.y = y;
                    else if (y > meshBoundsMax.y)
                        meshBoundsMax.y = y;
                }
            }

            // Add data to triangle buffer
            if (updateTriangles)
            {
                int oldTriangleCount = submesh.size();
                { // submesh.Resize(oldTriangleCount + attachmentIndexCount);
                    int newTriangleCount = oldTriangleCount + attachmentIndexCount;
                    submesh.setSize(newTriangleCount, 0);
                }
                Vector<int>& submeshItems = submesh;
                for (int i = 0; i < attachmentIndexCount; i++)
                    submeshItems[oldTriangleCount + i] = attachmentTriangleIndices[i] + ovc;
            }
        }

        clipper.clipEnd(*slot);
    }
    clipper.clipEnd();

    this->meshBoundsMin = meshBoundsMin;
    this->meshBoundsMax = meshBoundsMax;
    meshBoundsThickness = instruction.endSlot * zSpacing;

    // Trim or zero submesh triangles.
    Vector<int>& currentSubmeshItems = submesh;
    for (int i = submesh.size(), n = currentSubmeshItems.size(); i < n; i++)
        currentSubmeshItems[i] = 0;

    submeshIndex++; // Next AddSubmesh will use a new submeshIndex value.
}

void spine::MeshGenerator::ScaleVertexData(float scale)
{
    Vector<Vector3>& vbi = vertexBuffer;
    for (int i = 0; i < vertexBuffer.size(); ++i)
    {
        vbi[i] *= scale;
    }

    meshBoundsMin.x *= scale;
    meshBoundsMin.y *= scale;
    meshBoundsMax.x *= scale;
    meshBoundsMax.y *= scale;

    meshBoundsThickness *= scale;
}

void spine::MeshGenerator::EnsureVertexCapacity(int minimumVertexCount, bool includeTintBlack = false, bool includeTangents = false, bool includeNormals = false)
{
    if (minimumVertexCount > vertexBuffer.size())
    {
        vertexBuffer.setSize(minimumVertexCount, Vector3());
        uvBuffer.setSize(minimumVertexCount, Vector2());
        colorBuffer.setSize(minimumVertexCount, Vector4());

        if (includeTintBlack)
        {
            if (uv2 == nullptr)
            {
                uv2 = new Vector<Vector2>();
                uv3 = new Vector<Vector2>();
            }
            uv2->setSize(minimumVertexCount, Vector2());
            uv3->setSize(minimumVertexCount, Vector2());
        }

        if (includeNormals)
        {
            if (normals == nullptr)
            {
                normals = new Vector<Vector3>();
            }
            normals->setSize(minimumVertexCount, Vector3());
        }

        if (includeTangents)
        {
            if (tangents == nullptr)
            {
                tangents = new Vector<Vector4>();
            }
            tangents->setSize(minimumVertexCount, Vector4());
        }
    }
}

void spine::MeshGenerator::getSubMeshes(SkeletonRendererInstruction& instruction, int* dst)
{
    int total = instruction.submeshInstructions.size(), index = 0;
    for (int i = 0; i < total; i++)
    {
        size_t sz = instruction.submeshInstructions[i]->rawTriangleCount;
        memcpy(dst + index, subMeshes[i].buffer(), sizeof(int) * sz);
        index += sz;
    }
}

void spine::MeshGenerator::getSubMeshesSize(SkeletonRendererInstruction& instruction, int* dst)
{
    int total = instruction.submeshInstructions.size();
    for (int i = 0; i < total; i++)
    {
        dst[i] = instruction.submeshInstructions[i]->rawTriangleCount;
    }
}

void spine::MeshGenerator::getMeshBounds(float* min, float* max)
{
    min[0] = meshBoundsMin.x;
    min[1] = meshBoundsMin.y;

    max[0] = meshBoundsMax.x;
    max[1] = meshBoundsMax.y;
}



void MeshGenerator::generateMeshRenderers(Skeleton* skeleton)
{

    auto& drawOrder = skeleton->getDrawOrder();


    Vector<SubmeshInstruction*> instructions;
    int totalRawVertexCount = 0;
    int totalRawTriangleCount = 0;

    for (int i = 0; i < drawOrder.size(); ++i)
    {
        Slot& slot = *drawOrder[i];
        Attachment* attachment = slot.getAttachment();
        if (!attachment)
        {
            clipper.clipEnd(slot);
            continue;
        }

        if (slot.getColor().a == 0 || !slot.getBone().isActive())
        {
            clipper.clipEnd(slot);
            continue;
        }


        Vector<float> worldVertices;
        Vector<unsigned short> quadIndices;
        quadIndices.add(0);
        quadIndices.add(1);
        quadIndices.add(2);
        quadIndices.add(2);
        quadIndices.add(3);
        quadIndices.add(0);
        Vector<float> *vertices = &worldVertices;
        int32_t verticesCount;
        Vector<float>* uvs;
        Vector<unsigned short>* indices;
        int32_t indicesCount;
        Color* attachmentColor;
        int32_t pageIndex;
        String texturePath;

        if (attachment->getRTTI().isExactly(RegionAttachment::rtti))
        {
            RegionAttachment* regionAttachment = (RegionAttachment*)attachment;
            attachmentColor = &regionAttachment->getColor();

            if (attachmentColor->a == 0)
            {
                clipper.clipEnd(slot);
                continue;
            }

            worldVertices.setSize(8, 0);
            regionAttachment->computeWorldVertices(slot, worldVertices, 0, 2);
            verticesCount = 4;
            uvs = &regionAttachment->getUVs();
            indices = &quadIndices;
            indicesCount = 6;
            pageIndex = ((AtlasRegion*)regionAttachment->getRegion())->page->index;
            texturePath = ((AtlasRegion*)regionAttachment->getRegion())->page->texturePath;
        }
        else if (attachment->getRTTI().isExactly(MeshAttachment::rtti))
        {
            MeshAttachment* mesh = (MeshAttachment*)attachment;
            attachmentColor = &mesh->getColor();

            if (attachmentColor->a == 0)
            {
                clipper.clipEnd(slot);
                continue;
            }

            worldVertices.setSize(mesh->getWorldVerticesLength(), 0);
            mesh->computeWorldVertices(slot, 0, mesh->getWorldVerticesLength(), worldVertices.buffer(), 0, 2);
            verticesCount = (int32_t)(mesh->getWorldVerticesLength() >> 1);
            uvs = &mesh->getUVs();
            indices = &mesh->getTriangles();
            indicesCount = (int32_t)indices->size();
            pageIndex = ((AtlasRegion*)mesh->getRegion())->page->index;
            texturePath = ((AtlasRegion*)mesh->getRegion())->page->texturePath;

        }
        else if (attachment->getRTTI().isExactly(ClippingAttachment::rtti))
        {
            ClippingAttachment* clip = (ClippingAttachment*)slot.getAttachment();
            clipper.clipStart(slot, clip);
            continue;
        } 
        else
        {
            continue;
        }

        uint8_t colorArray[4];
        colorArray[0] = static_cast<uint8_t>(skeleton->getColor().r * slot.getColor().r * attachmentColor->r * 255);
        colorArray[1] = static_cast<uint8_t>(skeleton->getColor().g * slot.getColor().g * attachmentColor->g * 255);
        colorArray[2] = static_cast<uint8_t>(skeleton->getColor().b * slot.getColor().b * attachmentColor->b * 255);
        colorArray[3] = static_cast<uint8_t>(skeleton->getColor().a * slot.getColor().a * attachmentColor->a * 255);
        //uint32_t color = (a << 24) | (r << 16) | (g << 8) | b;
        uint32_t& color = *(uint32_t*)colorArray;
        

        if (clipper.isClipping()) {
            clipper.clipTriangles(worldVertices, *indices, *uvs, 2);
            vertices = &clipper.getClippedVertices();
            verticesCount = (int32_t)(clipper.getClippedVertices().size() >> 1);
            uvs = &clipper.getClippedUVs();
            indices = &clipper.getClippedTriangles();
            indicesCount = (int32_t)(clipper.getClippedTriangles().size());
        }

        SubmeshInstruction* current = new SubmeshInstruction();
        current->skeleton = skeleton;
        current->preActiveClippingSlotSource = -1;
        instructions.add(current);

        current->positions.setSize(verticesCount << 1, 0.0f);
        current->uvs.setSize(verticesCount << 1, 0.0f);
        current->colors.setSize(verticesCount, 0);
        current->indices.setSize(indicesCount, 0);
        current->blendMode = slot.getData().getBlendMode();
        current->atlasPage = pageIndex;
        current->texturePath = texturePath;
        memcpy(current->positions.buffer(), vertices->buffer(), (verticesCount << 1) * sizeof(float));
        memcpy(current->uvs.buffer(), uvs->buffer(), (verticesCount << 1) * sizeof(float));
        for (int ii = 0; ii < verticesCount; ii++) current->colors[ii] = color;
        memcpy(current->indices.buffer(), indices->buffer(), indices->size() * sizeof(uint16_t));
        clipper.clipEnd(slot);

    }

    clipper.clipEnd();

    if (instructions.size() == 0)
    {
        // ?
        return;
    }

    Vector<SubmeshInstruction*> &batchedInstructions = multipleInstruction.submeshInstructions;

    SubmeshInstruction* first = instructions[0];
    int startIndex = 0;
    int numVertices = first->positions.size() >> 1;
    int numIndices = first->indices.size();
    int batchedIndex = 0;
    // merge submesh instruction
    for (int i = 1; i <= instructions.size(); ++i)
    {
        SubmeshInstruction* instruction = i < instructions.size() ? instructions[i] : nullptr;
        if (instruction != nullptr && instruction->atlasPage == first->atlasPage &&
            instruction->blendMode == first->blendMode &&
            instruction->colors[0] == first->colors[0] &&
            numIndices + (instruction->positions.size() >> 1) < 0xffff &&
            instruction->texturePath == first->texturePath
            )
        {
            numVertices += (instruction->positions.size() >> 1);
            numIndices += instruction->indices.size();
        }
        else
        {
            SubmeshInstruction* batchInstruction; // = new SubmeshInstruction;
            if (batchedInstructions.size() > batchedIndex)
            {
                batchInstruction = batchedInstructions[batchedIndex];
            }
            else if (batchedInstructions.size() <= batchedIndex)
            {
                batchInstruction = new SubmeshInstruction;
                batchedInstructions.add(batchInstruction);
            }

            batchInstruction->positions.setSize(numVertices << 1, 0.0f);
            batchInstruction->uvs.setSize(numVertices << 1, 0.0f);
            batchInstruction->colors.setSize(numVertices, 0);
            batchInstruction->indices.setSize(numIndices, 0);
            batchInstruction->blendMode = first->blendMode;
            batchInstruction->atlasPage = first->atlasPage;
            int indicesOffset = 0;
            float* positions = batchInstruction->positions.buffer();
            float* uvs = batchInstruction->uvs.buffer();
            uint32_t* colors = batchInstruction->colors.buffer();
            uint16_t* indices = batchInstruction->indices.buffer();
            for (int j = startIndex; j < i; ++j)
            {
                SubmeshInstruction* instruction = instructions[j];
                memcpy(positions, instruction->positions.buffer(), instruction->positions.size() * sizeof(float));
                memcpy(uvs, instruction->uvs.buffer(), instruction->uvs.size() * sizeof(float));
                memcpy(colors, instruction->colors.buffer(), instruction->colors.size() * sizeof(int));
                for (int ii = 0; ii < instruction->indices.size(); ii++)
                {
                    indices[ii] = instruction->indices[ii] + indicesOffset;
                }
                indicesOffset += (instruction->positions.size() >> 1);
                positions += instruction->positions.size();
                uvs += instruction->uvs.size();
                colors += instruction->colors.size();
                indices += instruction->indices.size();
            }

            batchedIndex++;
            //batchedInstructions.add(batchInstruction);

            totalRawVertexCount += numVertices;
            totalRawTriangleCount += numIndices;

            if (i == instructions.size())
            {
                break;
            }
            first = instructions[i];
            startIndex = i;




            numVertices = (first->positions.size() >> 1);
            numIndices = first->indices.size();
        }
    }

    multipleInstruction.rawVertexCount = totalRawVertexCount;
    multipleInstruction.rawTriangleCount = totalRawTriangleCount;

    //Vector<SubmeshInstruction*> oldSubmeshInstructions = instructions->submeshInstructions;

    //instructions->submeshInstructions = std::move(batchedInstructions);
    for (int i = 0; i < instructions.size(); ++i)
    {
        delete instructions[i];
    }


}





void MeshGenerator::SolveTangents2DTriangles(Vector<Vector2>& tempTanBuffer, Vector<int>& triangles, int triangleCount, Vector<Vector3>& vertices, Vector<Vector2>& uvs, int vertexCount)
{
    Vector2 sdir;
    Vector2 tdir;

    for (int t = 0; t < triangleCount; t += 3)
    {
        int i1 = triangles[t + 0];
        int i2 = triangles[t + 1];
        int i3 = triangles[t + 2];

        Vector3 v1 = vertices[i1];
        Vector3 v2 = vertices[i2];
        Vector3 v3 = vertices[i3];

        Vector2 w1 = uvs[i1];
        Vector2 w2 = uvs[i2];
        Vector2 w3 = uvs[i3];

        float x1 = v2.x - v1.x;
        float x2 = v3.x - v1.x;
        float y1 = v2.y - v1.y;
        float y2 = v3.y - v1.y;

        float s1 = w2.x - w1.x;
        float s2 = w3.x - w1.x;
        float t1 = w2.y - w1.y;
        float t2 = w3.y - w1.y;

        float div = s1 * t2 - s2 * t1;
        float r = (div == 0) ? 0 : 1 / div;

        sdir.x = (t2 * x1 - t1 * x2) * r;
        sdir.y = (t2 * y1 - t1 * y2) * r;
        tempTanBuffer[i1] = tempTanBuffer[i2] = tempTanBuffer[i3] = sdir;

        tdir.x = (s1 * x2 - s2 * x1) * r;
        tdir.y = (s1 * y2 - s2 * y1) * r;
        tempTanBuffer[vertexCount + i1] = tempTanBuffer[vertexCount + i2] = tempTanBuffer[vertexCount + i3] = tdir;
    }
}

void MeshGenerator::SolveTangents2DBuffer(Vector<Vector4> &tangents, Vector<Vector2> &tempTanBuffer, int vertexCount)
{
    Vector4 tangent;
    tangent.z = 0;
    for (int i = 0; i < vertexCount; ++i) 
    {
        Vector2 t = tempTanBuffer[i];

        // t.Normalize() (aggressively inlined). Even better if offloaded to GPU via vertex shader.
        float magnitude = sqrt(t.x * t.x + t.y * t.y);
        if (magnitude > 1e-5) 
        {
            float reciprocalMagnitude = 1.0f / magnitude;
            t.x *= reciprocalMagnitude;
            t.y *= reciprocalMagnitude;
        }

        Vector2 t2 = tempTanBuffer[vertexCount + i];
        tangent.x = t.x;
        tangent.y = t.y;
        //tangent.z = 0;
        tangent.w = (t.y * t2.x > t.x * t2.y) ? 1 : -1; // 2D direction calculation. Used for binormals.
        tangents[i] = tangent;
    }
}

void MeshGenerator::FillLateVertexData()
{
    if (settings.calculateTangents)
    {
        int vertexCount = vertexBuffer.size();
        auto& sbi = subMeshes;
        int submeshCount = sbi.size();
        auto& vbi = vertexBuffer;
        auto& ubi = uvBuffer;
        if (tangents->size() != vertexBuffer.size())
        {
            if (tangents == nullptr)
            {
                tangents = new Vector<Vector4>();
            }
            tangents->setSize(vertexCount, Vector4());
            tempTanBuffer.setSize(vertexCount * 2, Vector2());
        }
        
        for (int i = 0; i < submeshCount; i++)
        {
            auto& submesh = sbi[i];
            int triangleCount = submesh.size();
            MeshGenerator::SolveTangents2DTriangles(tempTanBuffer, submesh, triangleCount, vbi, ubi, vertexCount);
        }
        MeshGenerator::SolveTangents2DBuffer(*tangents, tempTanBuffer, vertexCount);
    }
}

void MeshGenerator::SolveTangents2DBufferWithBuffer(Vector4* tangents, Vector<Vector2>& tempTanBuffer, int vertexCount)
{
    Vector4 tangent;
    tangent.z = 0;
    for (int i = 0; i < vertexCount; ++i)
    {
        Vector2 t = tempTanBuffer[i];

        // t.Normalize() (aggressively inlined). Even better if offloaded to GPU via vertex shader.
        float magnitude = sqrt(t.x * t.x + t.y * t.y);
        if (magnitude > 1e-5)
        {
            float reciprocalMagnitude = 1.0f / magnitude;
            t.x *= reciprocalMagnitude;
            t.y *= reciprocalMagnitude;
        }

        Vector2 t2 = tempTanBuffer[vertexCount + i];
        tangent.x = t.x;
        tangent.y = t.y;
        //tangent.z = 0;
        tangent.w = (t.y * t2.x > t.x * t2.y) ? 1 : -1; // 2D direction calculation. Used for binormals.
        tangents[i] = tangent;
    }
}

void MeshGenerator::FillLateVertexDataWithBuffers(Vector4* tangents, int vertexCount)
{
    if (settings.calculateTangents)
    {
        //int vertexCount = vertexBuffer.size();
        auto& sbi = subMeshes;
        int submeshCount = sbi.size();
        auto& vbi = vertexBuffer;
        auto& ubi = uvBuffer;
        if (tempTanBuffer.size() > vertexCount * 2)
        {
            tempTanBuffer.setSizeWithoutConstruct(vertexCount * 2);
        }

        for (int i = 0; i < submeshCount; i++)
        {
            auto& submesh = sbi[i];
            int triangleCount = submesh.size();
            MeshGenerator::SolveTangents2DTriangles(tempTanBuffer, submesh, triangleCount, vbi, ubi, vertexCount);
        }
        MeshGenerator::SolveTangents2DBufferWithBuffer(tangents, tempTanBuffer, vertexCount);
    }
}