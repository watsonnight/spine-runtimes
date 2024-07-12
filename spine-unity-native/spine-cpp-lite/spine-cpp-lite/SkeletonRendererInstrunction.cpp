#include "spine/Bone.h"
#include "spine/Skeleton.h"
#include "spine/Slot.h"
#include "SkeletonRendererInstruction.h"
using namespace spine;

void spine::SkeletonRendererInstruction::Clear()
{
	DeleteAll();
	submeshInstructions.setSizeWithoutConstruct(0);
	attachments.setSizeWithoutConstruct(0);
	immutableTriangles = false;
	hasActiveClipping = false;
	rawVertexCount = 0;
}

SkeletonRendererInstruction::~SkeletonRendererInstruction()
{
	DeleteAll();
}

void SkeletonRendererInstruction::DeleteAll()
{
	for (int i = 0; i < submeshInstructions.size(); i++)
	{
		delete submeshInstructions[i];
		submeshInstructions[i] = nullptr;
	}

	for (int i = 0; i < attachments.size(); ++i)
	{
		attachments[i] = nullptr;
	}

}

void SkeletonRendererInstruction::moveTo(SkeletonRendererInstruction& other)
{
	DeleteAll();

	if (submeshInstructions.size() != other.submeshInstructions.size())
	{
		submeshInstructions.setSizeWithoutConstruct(other.submeshInstructions.size());
	}

	for (int i = 0; i < submeshInstructions.size(); ++i)
	{
		submeshInstructions[i] = other.submeshInstructions[i];
		other.submeshInstructions[i] = nullptr;
	}

	if (attachments.size() != other.attachments.size())
	{
		attachments.setSizeWithoutConstruct(other.attachments.size());
	}

	for (int i = 0; i < attachments.size(); ++i)
	{
		attachments[i] = other.attachments[i];
		other.attachments[i] = nullptr;
	}

	immutableTriangles = other.immutableTriangles;
	hasActiveClipping = other.hasActiveClipping;
	rawVertexCount = other.rawVertexCount;
}

// SkeletonRendererInstruction* spine::SkeletonRendererInstruction::BuildInstruction(spine::Skeleton* skeleton, spine::SubmeshInstructionTransfer* instructions[], int size)
// {
//     auto slot = skeleton->getSlots();
//     SkeletonRendererInstruction* ins = this->
//     for (int i = 0; i < size; i++)
//     {
//         ins->submeshInstructions.add(instructions[i]);
//     }

//     int cnt = slot.size();
//     ins->attachments.setSize(cnt, nullptr);
//     for (int i = 0; i < size; i++)
//     {
//         if (!slot[i]->getBone().isActive())
//         {
//             ins->attachments[i] = nullptr;
//         }
//     }

// }

