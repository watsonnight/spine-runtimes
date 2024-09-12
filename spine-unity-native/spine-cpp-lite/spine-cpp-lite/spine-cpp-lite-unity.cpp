
#include "spine-cpp-lite-unity.h"
#include "MeshGenerator.h"
#include "spine/Skeleton.h"
#include "SkeletonRendererInstruction.h"

#include <cstdint>
#include <vector>
#include <spine/spine.h>
#include <spine/Debug.h>
using namespace spine;


static SpineExtension* defaultExtension = nullptr;
static DebugExtension* debugExtension = nullptr;

static void initExtensions() {
	if (defaultExtension == nullptr) {
		defaultExtension = new DefaultSpineExtension();
		debugExtension = new DebugExtension(defaultExtension);
	}
}

spine::SpineExtension* spine::getDefaultExtension() {
	initExtensions();
	return defaultExtension;
}

void spine_enable_debug_extension(uint32_t enable) {
	initExtensions();
	SpineExtension::setInstance(enable ? debugExtension : defaultExtension);
}

static HashMap<intptr_t, SpineObject*> s_global_spine_object_map;

static HashMap<intptr_t, Atlas*> s_global_spine_atlas_map;

static HashMap<intptr_t, Skeleton*> s_global_spine_skeleton_map;

static HashMap<intptr_t, Bone*> s_local_spine_bone_map;

static HashMap<intptr_t, Slot*> s_local_spine_slot_map;

static HashMap<intptr_t, AnimationState*> s_global_spine_animation_state_map;

static HashMap<intptr_t, AnimationStateData*> s_global_spine_animation_state_data_map;

static HashMap<intptr_t, Animation*> s_global_spine_animation_map;

static HashMap<intptr_t, Timeline*> s_global_spine_timeline_map;

static HashMap<intptr_t, SkeletonData*> s_global_spine_skeleton_data_map;

static HashMap<intptr_t, MeshGenerator*> s_global_mesh_generator_map;


const intptr_t k_invalid = 0;


#define CHECK_GET_ATLAS_RETURN(atlasHandle, val) \
	if (!s_global_spine_atlas_map.containsKey(atlasHandle)) \
	{														\
		return val;											\
	}														\
	Atlas* atlas = s_global_spine_atlas_map[atlasHandle];


intptr_t spine_atlas_load_unity(char* atlasData)
{
	if (!atlasData) return k_invalid;
	int32_t length = (int32_t)strlen(atlasData);
	auto atlas = new (__FILE__, __LINE__) Atlas(atlasData, length, "", (TextureLoader*)nullptr, false);
	intptr_t handle = (intptr_t)atlas;
	s_global_spine_atlas_map.put(handle, atlas);
	return handle;
}

void spine_atlas_destroy_unity(intptr_t atlasHandle)
{
	CHECK_GET_ATLAS_RETURN(atlasHandle, );
	
	s_global_spine_atlas_map.remove(atlasHandle);
	delete atlas;
}

void spine_atlas_flipV_unity(intptr_t atlasHandle)
{
	CHECK_GET_ATLAS_RETURN(atlasHandle,)

	atlas->flipV();
}

intptr_t spine_atlas_find_region_unity(intptr_t atlasHandle, const char* name)
{
	CHECK_GET_ATLAS_RETURN(atlasHandle, k_invalid);

	AtlasRegion* region =  atlas->findRegion(name);
	return (intptr_t)region;
}

intptr_t spine_atlas_get_region_unity(intptr_t atlasHandle, int index)
{
	CHECK_GET_ATLAS_RETURN(atlasHandle, k_invalid);

	if (index < 0 || index >= atlas->getRegions().size())
	{
		return k_invalid;
	}
	return (intptr_t)atlas->getRegions()[index];
}

int spine_atlas_get_region_count_unity(intptr_t atlasHandle)
{
	CHECK_GET_ATLAS_RETURN(atlasHandle, k_invalid);

	return atlas->getRegions().size();
}

const char* spine_atlas_get_region_name_unity(intptr_t atlasHandle, int index)
{
	CHECK_GET_ATLAS_RETURN(atlasHandle, "");

	if (index < 0 || index >= atlas->getRegions().size())
	{
		return "";
	}
	return atlas->getRegions()[index]->name.buffer();
}

// intptr_t spine_atlasregion_get_pages_unity(intptr_t atlasHandle, intptr_t regionHandle)
// {
// 	CHECK_GET_ATLAS_RETURN(atlasHandle, k_invalid);
// 	auto& regions = atlas->getRegions();
// 	for (int i = 0; i < regions.size(); i++)
// 	{
// 		if ((intptr_t)regions[i] == regionHandle)
// 		{
// 			return (intptr_t)regions[i]->page;
// 		}
// 	}
// 	return k_invalid;
// }



#define CHECK_BONE_RETURN(boneHandle, val) \
	if (!s_local_spine_bone_map.containsKey(boneHandle)) \
	{														\
		return val;											\
	}		

// float spine_bone_get_x_unity(intptr_t boneHandle)
// {
// 	CHECK_BONE_RETURN(boneHandle, 0.0f);
//
// 	Bone* bone = s_local_spine_bone_map[boneHandle];
//
// 	return bone->getX();
// }

// float spine_bone_get_y_unity(intptr_t boneHandle)
// {
// 	CHECK_BONE_RETURN(boneHandle, 0.0f);
//
// 	Bone* bone = s_local_spine_bone_map[boneHandle];
//
// 	return bone->getY();
// }
//
// float spine_bone_get_rotation_unity(intptr_t boneHandle)
// {
// 	CHECK_BONE_RETURN(boneHandle, 0.0f);
//
// 	Bone* bone = s_local_spine_bone_map[boneHandle];
//
// 	return bone->getRotation();
// }

float spine_bone_get_scale_x_unity(intptr_t boneHandle)
{
	CHECK_BONE_RETURN(boneHandle, 0.0f);

	Bone* bone = s_local_spine_bone_map[boneHandle];

	return bone->getScaleX();
}
float spine_bone_get_scale_y_unity(intptr_t boneHandle)
{
	CHECK_BONE_RETURN(boneHandle, 0.0f);

	Bone* bone = s_local_spine_bone_map[boneHandle];

	return bone->getScaleY();
}

// float spine_bone_get_shear_x_unity(intptr_t boneHandle)
// {
// 	CHECK_BONE_RETURN(boneHandle, 0.0f);
//
// 	Bone* bone = s_local_spine_bone_map[boneHandle];
//
// 	return bone->getShearX();
// }
//
// float spine_bone_get_shear_y_unity(intptr_t boneHandle)
// {
// 	CHECK_BONE_RETURN(boneHandle, 0.0f);
//
// 	Bone* bone = s_local_spine_bone_map[boneHandle];
//
// 	return bone->getShearY();
// }

// void spine_bone_set_local_position_unity(intptr_t skeletonHandle, intptr_t boneHandle, float* position)
// {
// 	CHECK_BONE_RETURN(boneHandle, );
//
// 	Bone* bone = s_local_spine_bone_map[boneHandle];
// 	if (bone == nullptr)
// 	{
// 		return;
// 	}
//
// 	bone->setX(position[0]);
// 	bone->setY(position[1]);
// }


float spine_bone_get_world_x_unity(intptr_t boneHandle)
{
	CHECK_BONE_RETURN(boneHandle, 0.0f);

	Bone* bone = s_local_spine_bone_map[boneHandle];

	return bone->getWorldX();
}

float spine_bone_get_world_y_unity(intptr_t boneHandle)
{
	CHECK_BONE_RETURN(boneHandle, 0.0f);

	Bone* bone = s_local_spine_bone_map[boneHandle];

	return bone->getWorldY();
}

float spine_bone_get_a_unity(intptr_t boneHandle)
{
	CHECK_BONE_RETURN(boneHandle, 0.0f);

	Bone* bone = s_local_spine_bone_map[boneHandle];

	return bone->getA();
}

float spine_bone_get_c_unity(intptr_t boneHandle)
{
	CHECK_BONE_RETURN(boneHandle, 0.0f);

	Bone* bone = s_local_spine_bone_map[boneHandle];

	return bone->getC();
}

float spine_bone_get_world_rotation_x_unity(intptr_t boneHandle)
{
	CHECK_BONE_RETURN(boneHandle, 0.0f);

	Bone* bone = s_local_spine_bone_map[boneHandle];

	return bone->getWorldRotationX();
}

float spine_bone_get_world_rotation_y_unity(intptr_t boneHandle)
{
	CHECK_BONE_RETURN(boneHandle, 0.0f);

	Bone* bone = s_local_spine_bone_map[boneHandle];

	return bone->getWorldRotationY();
}

float spine_bone_get_world_scale_x_unity(intptr_t boneHandle)
{
	CHECK_BONE_RETURN(boneHandle, 0.0f);

	Bone* bone = s_local_spine_bone_map[boneHandle];

	return bone->getWorldScaleX();
}

float spine_bone_get_world_scale_y_unity(intptr_t boneHandle)
{
	CHECK_BONE_RETURN(boneHandle, 0.0f);

	Bone* bone = s_local_spine_bone_map[boneHandle];

	return bone->getWorldScaleY();
}

float spine_bone_get_data_length_unity(intptr_t boneHandle)
{
	CHECK_BONE_RETURN(boneHandle, 0.0f);

	Bone* bone = s_local_spine_bone_map[boneHandle];

	return bone->getData().getLength();
}

const char* spine_bone_get_data_name_unity(intptr_t boneHandle)
{
	CHECK_BONE_RETURN(boneHandle, "");

	Bone* bone = s_local_spine_bone_map[boneHandle];

	return bone->getData().getName().buffer();
}

void spine_bone_set_local_position_unity(intptr_t boneHandle, float* position)
{
	CHECK_BONE_RETURN(boneHandle, );

	Bone* bone = s_local_spine_bone_map[boneHandle];

	bone->setX(position[0]);
	bone->setY(position[1]);
}

intptr_t spine_bone_get_parent_unity(intptr_t boneHandle)
{
	CHECK_BONE_RETURN(boneHandle, k_invalid);

	Bone* bone = s_local_spine_bone_map[boneHandle];

	Bone* parent = bone->getParent();

	if (nullptr == parent)
	{
		return k_invalid;
	}

	intptr_t handle = (intptr_t)parent;

	if (!s_local_spine_bone_map.containsKey(handle))
	{
		s_local_spine_bone_map.put(handle, parent);
	}

	return handle;
}

intptr_t spine_bone_get_skeleton_unity(intptr_t boneHandle)
{
	CHECK_BONE_RETURN(boneHandle, k_invalid);

	Bone* bone = s_local_spine_bone_map[boneHandle];

	Skeleton* skeleton = &bone->getSkeleton();
	intptr_t handle = (intptr_t)skeleton;

	if (!s_global_spine_skeleton_map.containsKey(handle))
	{
		s_global_spine_skeleton_map.put(handle, skeleton);
	}

	return handle;
}

void spine_bone_dispose_local_unity(intptr_t boneHandle)
{
	CHECK_BONE_RETURN(boneHandle, );

	s_local_spine_bone_map.remove(boneHandle);
}









intptr_t spine_skeleton_data_load_binary_scale_unity(intptr_t atlasHandle, const uint8_t* skeletonData, int32_t length, float scale)
{
	CHECK_GET_ATLAS_RETURN(atlasHandle, k_invalid);
	//Bone::setYDown(true);
	SkeletonBinary binary(atlas);
	binary.setScale(scale);
	SkeletonData* data = binary.readSkeletonData(skeletonData, length);

	intptr_t skeletonDataHandle = (intptr_t)data;
	s_global_spine_skeleton_data_map.put(skeletonDataHandle, data);

	return skeletonDataHandle;
}

intptr_t spine_skeleton_data_load_json_scale_unity(intptr_t atlasHandle, const uint8_t* skeletonData, int32_t length, float scale)
{
	CHECK_GET_ATLAS_RETURN(atlasHandle, k_invalid);
	//Bone::setYDown(true);
	SkeletonJson json(atlas);
	json.setScale(scale);
	SkeletonData* data = json.readSkeletonData((const char*)skeletonData);

	intptr_t skeletonDataHandle = (intptr_t)data;
	s_global_spine_skeleton_data_map.put(skeletonDataHandle, data);

	return skeletonDataHandle;
}


#define CHECK_SKELETON_DATA_RETURN(skeletonDataHandle, val) \
	if (!s_global_spine_skeleton_data_map.containsKey(skeletonDataHandle)) \
	{														\
		return val;											\
	}		



void spine_skeleton_data_destroy_unity(intptr_t skeletonDataHandle)
{
	CHECK_SKELETON_DATA_RETURN(skeletonDataHandle, );

	SkeletonData* data = s_global_spine_skeleton_data_map[skeletonDataHandle];
	s_global_spine_skeleton_data_map.remove(skeletonDataHandle);

	delete data;
}

intptr_t spine_skeleton_data_find_animation_unity(intptr_t skeletonDataHandle, const char* startingAnimationName)
{
	CHECK_SKELETON_DATA_RETURN(skeletonDataHandle, k_invalid);

	SkeletonData* data = s_global_spine_skeleton_data_map[skeletonDataHandle];

	Animation* animation = data->findAnimation(startingAnimationName);
	if (!animation)
	{
		return k_invalid;
	}
	intptr_t handle = (intptr_t)animation;
	s_global_spine_animation_map.put(handle, animation);

	return (intptr_t)animation;
}

#define CHECK_ANIMATION_RETURN(animationHandle, val) \
	if (!s_global_spine_animation_map.containsKey(animationHandle)) \
	{														\
		return val;											\
	}		


void spine_skeleton_data_remove_animation_unity(intptr_t animationHandle)
{
	CHECK_ANIMATION_RETURN(animationHandle, );

	s_global_spine_animation_map.remove(animationHandle);
}

intptr_t spine_skeleton_data_find_event_data_unity(intptr_t skeletonDataHandle, const char* eventName)
{
	CHECK_SKELETON_DATA_RETURN(skeletonDataHandle, k_invalid);

	SkeletonData* data = s_global_spine_skeleton_data_map[skeletonDataHandle];

	EventData* eventData = data->findEvent(eventName);
	intptr_t handle = (intptr_t)eventData;

	return handle;
}



intptr_t spine_skeleton_create_unity(intptr_t skeletonDataHandle)
{
	CHECK_SKELETON_DATA_RETURN(skeletonDataHandle, k_invalid);

	SkeletonData* data = s_global_spine_skeleton_data_map[skeletonDataHandle];

	Skeleton* skeleton = new (__FILE__, __LINE__) Skeleton(data);

	intptr_t handle = (intptr_t)skeleton;
	s_global_spine_skeleton_map.put(handle, skeleton);
	return handle;
}

#define CHECK_SKELETON_RETURN(skeletonHandle, val) \
	if (!s_global_spine_skeleton_map.containsKey(skeletonHandle)) \
	{														\
		return val;											\
	}	

void spine_skeleton_destroy_unity(intptr_t skeletonHandle)
{
	CHECK_SKELETON_RETURN(skeletonHandle, );

	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];
	s_global_spine_skeleton_map.remove(skeletonHandle);

	auto& bones = skeleton->getBones();
	for (int i = 0; i < bones.size(); ++i)
	{
		intptr_t handle = (intptr_t)bones[i];
		s_local_spine_bone_map.remove(handle);
	}

	auto& slots = skeleton->getSlots();
	for (int i = 0; i < slots.size(); ++i)
	{
		intptr_t handle = (intptr_t)slots[i];
		s_local_spine_slot_map.remove(handle);
	}

	delete skeleton;
}


intptr_t spine_animation_state_data_create_unity(intptr_t skeletonDataHandle)
{
	CHECK_SKELETON_DATA_RETURN(skeletonDataHandle, k_invalid);

	SkeletonData* data = s_global_spine_skeleton_data_map[skeletonDataHandle];

	AnimationStateData* animationStateData = new (__FILE__, __LINE__) AnimationStateData(data);

	intptr_t handle = (intptr_t)animationStateData;
	s_global_spine_animation_state_data_map.put(handle, animationStateData);

	return handle;
}

void spine_animation_state_data_destroy_unity(intptr_t animationStateDataHandle)
{
	if (!s_global_spine_animation_state_data_map.containsKey(animationStateDataHandle))
	{
		return;
	}

	AnimationStateData* animationStateData = s_global_spine_animation_state_data_map[animationStateDataHandle];
	s_global_spine_animation_state_data_map.remove(animationStateDataHandle);
	delete animationStateData;
}


intptr_t spine_animation_state_create_unity(intptr_t animationStateDataHandle)
{
	if (!s_global_spine_animation_state_data_map.containsKey(animationStateDataHandle))
	{
		return k_invalid;
	}

	AnimationStateData* stateData = s_global_spine_animation_state_data_map[animationStateDataHandle];

	AnimationState* state = new (__FILE__, __LINE__) AnimationState(stateData);

	intptr_t handle = (intptr_t)state;
	s_global_spine_animation_state_map.put(handle, state);

	return handle;
}

#define CHECK_ANIMATION_STATE_RETURN(animationStateHandle, val) \
	if (!s_global_spine_animation_state_map.containsKey(animationStateHandle)) \
	{														\
		return val;											\
	}	

void spine_animation_state_destroy_unity(intptr_t animationStateHandle)
{
	CHECK_ANIMATION_STATE_RETURN(animationStateHandle, );

	AnimationState* state = s_global_spine_animation_state_map[animationStateHandle];
	s_global_spine_animation_state_map.remove(animationStateHandle);
	delete state;
}


int spine_animation_state_set_and_remove_animation_unity(intptr_t animationStateHandle, int trackIndex, intptr_t animationHandle, bool loop)
{
	if (animationHandle == k_invalid)
	{
		return -1;
	}
	CHECK_ANIMATION_STATE_RETURN(animationStateHandle, -1);


	AnimationState* animationState = s_global_spine_animation_state_map[animationStateHandle];
	Animation* animation = s_global_spine_animation_map[animationHandle];

	TrackEntry* track = animationState->setAnimation(trackIndex, animation, loop);

	s_global_spine_animation_map.remove(animationHandle);

	return track->getTrackIndex();
}

int spine_animation_state_add_and_remove_animation_unity(intptr_t animationStateHandle, int trackIndex, intptr_t animationHandle, bool loop, float delay)
{
	if (animationHandle == k_invalid)
	{
		return -1;
	}
	CHECK_ANIMATION_STATE_RETURN(animationStateHandle, -1);


	AnimationState* animationState = s_global_spine_animation_state_map[animationStateHandle];
	Animation* animation = s_global_spine_animation_map[animationHandle];

	TrackEntry* track = animationState->addAnimation(trackIndex, animation, loop, delay);

	s_global_spine_animation_map.remove(animationHandle);

	return track->getTrackIndex();
}


bool spine_animation_state_apply_unity(intptr_t animationStateHandle, intptr_t skeletonHandle)
{
	CHECK_ANIMATION_STATE_RETURN(animationStateHandle, true);

	AnimationState* animationState = s_global_spine_animation_state_map[animationStateHandle];
	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];

	return animationState->apply(*skeleton);
}

void spine_animation_state_update_unity(intptr_t animationStateHandle, float delta)
{
	CHECK_ANIMATION_STATE_RETURN(animationStateHandle, );

	AnimationState* animationState = s_global_spine_animation_state_map[animationStateHandle];

	return animationState->update(delta);
}

static HashMap<intptr_t, eventCallbackDelegate> s_global_event_callback_map;

void animationCallback(AnimationState* state, EventType type, TrackEntry* entry, Event* event)
{
	intptr_t stateHandle = (intptr_t)state;
	if (s_global_event_callback_map.containsKey(stateHandle))
	{
		eventCallbackDelegate eventCB = s_global_event_callback_map[stateHandle];
		eventCB(stateHandle, entry->getTrackIndex(), type, (intptr_t)event);
	}
}

void spine_animation_state_add_callback_unity(intptr_t animationStateHandle, eventCallbackDelegate cb)
{
	CHECK_ANIMATION_STATE_RETURN(animationStateHandle, );

	AnimationState* animationState = s_global_spine_animation_state_map[animationStateHandle];

	s_global_event_callback_map.put(animationStateHandle, cb);
	animationState->setListener(animationCallback);
}

void spine_animation_state_remove_callback_unity(intptr_t animationStateHandle, eventCallbackDelegate cb)
{
	CHECK_ANIMATION_STATE_RETURN(animationStateHandle, );

	AnimationState* animationState = s_global_spine_animation_state_map[animationStateHandle];

	s_global_event_callback_map.remove(animationStateHandle);
	animationState->setListener((AnimationStateListenerObject*)NULL);
}


// void spine_animation_state_add_empty_animation_unity(intptr_t animationStateHandle, int trackIndex, float mixDuration, float delay)
// {
// 	CHECK_ANIMATION_STATE_RETURN(animationStateHandle, );
//
// 	AnimationState* animationState = s_global_spine_animation_state_map[animationStateHandle];
//
// 	if (trackIndex < 0)
// 	{
// 		return;
// 	}
//
// 	animationState->addEmptyAnimation(trackIndex, mixDuration, delay);
//
// }

// void spine_animation_state_track_set_attachment_threshold_unity(intptr_t animationStateHandle, int trackIndex, float attachmentThreshold)
// {
// 	CHECK_ANIMATION_STATE_RETURN(animationStateHandle, );
//
// 	AnimationState* animationState = s_global_spine_animation_state_map[animationStateHandle];
//
// 	if (trackIndex < 0 || trackIndex >= animationState->getTracks().size())
// 	{
// 		return;
// 	}
// 	TrackEntry* track = animationState->getTracks()[trackIndex];
// 	track->setAttachmentThreshold(attachmentThreshold);
// }
//
// void spine_animation_state_track_set_mix_duration_unity(intptr_t animationStateHandle, int trackIndex, float mixDuration)
// {
// 	CHECK_ANIMATION_STATE_RETURN(animationStateHandle, );
//
// 	AnimationState* animationState = s_global_spine_animation_state_map[animationStateHandle];
//
// 	if (trackIndex < 0 || trackIndex >= animationState->getTracks().size())
// 	{
// 		return;
// 	}
// 	TrackEntry* track = animationState->getTracks()[trackIndex];
// 	track->setMixDuration(mixDuration);
// }

void spine_animation_state_track_set_time_scale_unity(intptr_t animationStateHandle, int trackIndex, float timeScale)
{
	CHECK_ANIMATION_STATE_RETURN(animationStateHandle, );

	AnimationState* animationState = s_global_spine_animation_state_map[animationStateHandle];

	if (trackIndex < 0 || trackIndex >= animationState->getTracks().size())
	{
		return;
	}
	TrackEntry* track = animationState->getTracks()[trackIndex];
	track->setTimeScale(timeScale);
}
// float spine_animation_state_track_get_time_scale_unity(intptr_t animationStateHandle, int trackIndex)
// {
// 	CHECK_ANIMATION_STATE_RETURN(animationStateHandle, k_invalid);
//
// 	AnimationState* animationState = s_global_spine_animation_state_map[animationStateHandle];
//
// 	if (trackIndex < 0 || trackIndex >= animationState->getTracks().size())
// 	{
// 		return k_invalid;
// 	}
// 	TrackEntry* track = animationState->getTracks()[trackIndex];
// 	return track->getTimeScale();
// }

void spine_animation_state_track_get_track_time_end_duration_unity(intptr_t animationStateHandle, int trackIndex, float* outTrackTime)
{
	CHECK_ANIMATION_STATE_RETURN(animationStateHandle, );

	AnimationState* animationState = s_global_spine_animation_state_map[animationStateHandle];

	if (trackIndex < 0 || trackIndex >= animationState->getTracks().size())
	{
		return;
	}
	TrackEntry* track = animationState->getTracks()[trackIndex];
	
	outTrackTime[0] = track->getTrackTime();
	outTrackTime[1] = track->getTrackEnd();
	outTrackTime[2] = track->getAnimation()->getDuration();
}

void spine_animation_apply_unity(intptr_t animationHandle, intptr_t skeletonHandle, float lastTime, float time, bool loop, float alpha, int mixBlend, int direction)
{

	CHECK_ANIMATION_RETURN(animationHandle, );
	CHECK_SKELETON_RETURN(skeletonHandle, );

	Animation* animation = s_global_spine_animation_map[animationHandle];
	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];
	animation->apply(*skeleton, lastTime, time, loop, nullptr, alpha, (MixBlend)mixBlend, (MixDirection)direction);

}


void spine_skeleton_update_world_transform_unity(intptr_t skeletonHandle)
{
	CHECK_SKELETON_RETURN(skeletonHandle, );
	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];

	skeleton->updateWorldTransform();
}


// void spine_skeleton_set_scale_x_unity(intptr_t skeletonHandle, float scaleX)
// {
// 	CHECK_SKELETON_RETURN(skeletonHandle, );
//
// 	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];
//
// 	skeleton->setScaleX(scaleX);
// }

float spine_skeleton_get_scale_x_unity(intptr_t skeletonHandle)
{
	CHECK_SKELETON_RETURN(skeletonHandle, 0.0f);

	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];

	return skeleton->getScaleX();
}

// void spine_skeleton_set_scale_y_unity(intptr_t skeletonHandle, float scaleY)
// {
// 	CHECK_SKELETON_RETURN(skeletonHandle, );
//
// 	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];
//
// 	skeleton->setScaleY(scaleY);
// }

float spine_skeleton_get_scale_y_unity(intptr_t skeletonHandle)
{
	CHECK_SKELETON_RETURN(skeletonHandle, 0.0f);

	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];

	return skeleton->getScaleY();
}

void spine_skeleton_set_skin_unity(intptr_t skeletonHandle, const char* skinName)
{
	CHECK_SKELETON_RETURN(skeletonHandle, );

	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];

	skeleton->setSkin(skinName);
}

void spine_skeleton_set_color_unity(intptr_t skeletonHandle, const float* color)
{
	CHECK_SKELETON_RETURN(skeletonHandle, );

	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];

	Color col = skeleton->getColor();
	col.r = color[0];
	col.g = color[1];
	col.b = color[2];
	col.a = color[3];
}

void spine_skeleton_set_color_a_unity(intptr_t skeletonHandle, float alpha)
{
	CHECK_SKELETON_RETURN(skeletonHandle, );

	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];
	skeleton->getColor().a = alpha;
}

void spine_skeleton_set_color_r_unity(intptr_t skeletonHandle, float red)
{
	CHECK_SKELETON_RETURN(skeletonHandle, );

	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];
	skeleton->getColor().r = red;
}
void spine_skeleton_set_color_g_unity(intptr_t skeletonHandle, float green)
{
	CHECK_SKELETON_RETURN(skeletonHandle, );

	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];
	skeleton->getColor().g = green;
}

void spine_skeleton_set_color_b_unity(intptr_t skeletonHandle, float blue)
{
	CHECK_SKELETON_RETURN(skeletonHandle, );

	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];
	skeleton->getColor().b = blue;
}

intptr_t spine_skeleton_find_bone_unity(intptr_t skeletonHandle, const char* name)
{
	CHECK_SKELETON_RETURN(skeletonHandle, k_invalid);

	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];
	
	Bone* bone = skeleton->findBone(name);
	if (bone == nullptr)
	{
		return k_invalid;
	}
	
	intptr_t handle = (intptr_t)bone;
	s_local_spine_bone_map.put(handle, bone);

	return handle;
}

intptr_t spine_skeleton_find_slot_unity(intptr_t skeletonHandle, const char* name)
{
	CHECK_SKELETON_RETURN(skeletonHandle, k_invalid);

	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];

	Slot* slot = skeleton->findSlot(name);
	if (slot == nullptr)
	{
		return k_invalid;
	}

	intptr_t handle = (intptr_t)slot;
	s_local_spine_slot_map.put(handle, slot);

	return handle;
}

void spine_skeleton_sync_attachments_unity(intptr_t skeletonDataHandle, int* slotIndexes, const char** names, int* hashCode, int size)
{
	CHECK_SKELETON_DATA_RETURN(skeletonDataHandle, );

	SkeletonData* skeleton = s_global_spine_skeleton_data_map[skeletonDataHandle];

	Skin* skin = skeleton->getDefaultSkin();

	for (int i = 0; i < size; ++i)
	{
		Attachment* attachment = skin->getAttachment(slotIndexes[i], names[i]);
		if (attachment == nullptr)
		{
			continue;
		}

		if (attachment->getRTTI().instanceOf(RegionAttachment::rtti))
		{
			static_cast<RegionAttachment*>(attachment)->setHashCode(hashCode[i]);
		}
		else if (attachment->getRTTI().instanceOf(MeshAttachment::rtti))
		{
			static_cast<MeshAttachment*>(attachment)->setHashCode(hashCode[i]);
		}
	}
}



void spine_slot_dispose_local_unity(intptr_t slotHandle)
{
	if (s_local_spine_slot_map.containsKey(slotHandle))
	{
		s_local_spine_slot_map.remove(slotHandle);
	}
}

void spine_slot_set_attachment_unity(intptr_t slotHandle, intptr_t attachmentHandle)
{
	if (!s_local_spine_slot_map.containsKey(slotHandle))
	{
		return;
	}

	Slot* slot = s_local_spine_slot_map[slotHandle];
	Attachment* attachment = (Attachment*)attachmentHandle;

	slot->setAttachment(attachment);
}

// SPINE_CPP_LITE_EXPORT intptr_t spine_event_get_event_data_handle_unity(intptr_t eventHandle)
// {
// 	Event* event = (Event*)eventHandle;
// 	if (event == nullptr)
// 	{
// 		return k_invalid;
// 	}
//
// 	const EventData& data = event->getData();
//
// 	intptr_t dataHandle = (intptr_t)&data;
// 	return dataHandle;
// }


intptr_t spine_region_attachment_create_from_atlas_region_unity(intptr_t atlasHandle, const char* regionName, const char* attachmentName, float scale, float rotation)
{
	CHECK_GET_ATLAS_RETURN(atlasHandle, k_invalid);

	AtlasAttachmentLoader loader(atlas);

	Skin skin("unused");
	RegionAttachment* attachment = loader.newRegionAttachment(skin, attachmentName, regionName, nullptr);
	AtlasRegion* region = atlas->findRegion(regionName);
	attachment->setPath(region->name);
	attachment->setRotation(rotation);
	float originalWidth = region->originalWidth;
	float originalHeight = region->originalHeight;
	attachment->setWidth(originalWidth * scale);
	attachment->setHeight(originalHeight * scale);

	attachment->updateRegion();

	intptr_t handle = (intptr_t)attachment;

	return handle;
}

// be sure region attachment is not used by any other class;
void spine_region_attachment_dispose_unity(intptr_t regionAttachmentHandle, intptr_t slotHandle)
{
	RegionAttachment* regionAttachment = (RegionAttachment*)regionAttachmentHandle;

	if (slotHandle != k_invalid && s_local_spine_slot_map.containsKey(slotHandle))
	{
		Slot* slot = s_local_spine_slot_map[slotHandle];
		slot->setAttachment(nullptr);
	}

	delete regionAttachment;
	//regionAttachment->dereference();
	//if (regionAttachment->getRefCount() == 0) {
	//	delete regionAttachment;
	//}
}


intptr_t spine_mesh_generator_create_unity()
{
	MeshGenerator* meshGenerator = new (__FILE__, __LINE__) MeshGenerator();
	intptr_t handle = (intptr_t)meshGenerator;

	s_global_mesh_generator_map.put(handle, meshGenerator);

	return handle;
}


#define CHECK_MESH_GENERATOR_RETURN(meshGeneratorHandle, val) \
	if (!s_global_mesh_generator_map.containsKey(meshGeneratorHandle)) \
	{														\
		return val;											\
	}	

void spine_mesh_generator_destroy_unity(intptr_t meshGeneratorHandle)
{
	CHECK_MESH_GENERATOR_RETURN(meshGeneratorHandle, );

	MeshGenerator* meshGenerator = s_global_mesh_generator_map[meshGeneratorHandle];
	s_global_mesh_generator_map.remove(meshGeneratorHandle);
	delete meshGenerator;
}



void spine_mesh_generator_set_settings_unity(intptr_t handle, MeshGenerator::Settings* settings)
{
	CHECK_MESH_GENERATOR_RETURN(handle, );

	MeshGenerator* generator = s_global_mesh_generator_map[handle];
	if (generator)
	{
		generator->settings = *settings;
	}
}

void spine_mesh_generator_build_mesh_unity(intptr_t handle, bool updateTriangles)
{
	CHECK_MESH_GENERATOR_RETURN(handle, );

	MeshGenerator* generator = s_global_mesh_generator_map[handle];
	if (generator)
	{
		generator->Begin();
		generator->BuildMeshWithArrays(*generator->currentInstruction, updateTriangles);
	}
}


void spine_mesh_generator_generate_single_instruction_unity(intptr_t skeletonHandle, intptr_t meshGeneratorHandle, int* outParams)
{
	CHECK_MESH_GENERATOR_RETURN(meshGeneratorHandle, );
	CHECK_SKELETON_RETURN(skeletonHandle, );

	MeshGenerator* generator = s_global_mesh_generator_map[meshGeneratorHandle];
	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];


	generator->oldInstruction[!generator->instructionIndex].DeleteAll();
	
	generator->oldInstruction[!generator->instructionIndex].moveTo(generator->newInstruction);
	SkeletonRendererInstruction* instruction = &generator->newInstruction;
	instruction->Clear();
	Vector<Slot*>& drawOrder = skeleton->getDrawOrder();

	int drawOrderCount = skeleton->getDrawOrder().size();
	instruction->attachments.setSizeWithoutConstruct(drawOrderCount);

	SubmeshInstruction* current = new SubmeshInstruction();
	current->skeleton = skeleton;
	current->preActiveClippingSlotSource = -1;
	current->startSlot = 0;
	current->rawFirstVertexIndex = 0;
	current->forceSeparate = false;
	current->endSlot = drawOrderCount;

	bool skeletonHasClipping = false;


	int rawVertexCount = 0, rawIndicesCount = 0;
	int regionIndex = -1;


	for (int i = 0; i < drawOrderCount; ++i)
	{
		Slot* slot = drawOrder[i];
		if (!slot->getBone().isActive())
		{
			(instruction->attachments)[i] = nullptr;
			continue;
		}
		if (slot->getData().getBlendMode() == BlendMode_Additive) current->hasPMAAdditiveSlot = true;

		Attachment* attachment = slot->getAttachment();
		(instruction->attachments)[i] = attachment;

		if (attachment == NULL) {
			continue;
		}
		if (attachment->getRTTI().isExactly(RegionAttachment::rtti))
		{
			RegionAttachment* regionAttachment = static_cast<RegionAttachment*>(attachment);
			if (regionAttachment->getSequence() != nullptr) regionAttachment->getSequence()->apply(slot, regionAttachment);
			rawVertexCount += 4;
			rawIndicesCount += 6;
			regionIndex = static_cast<RegionAttachment*>(attachment)->getHashCode();
		}
		else if (attachment->getRTTI().isExactly(MeshAttachment::rtti))
		{
			MeshAttachment* meshAttachment = static_cast<MeshAttachment*>(attachment);
			if (meshAttachment->getSequence() != nullptr) meshAttachment->getSequence()->apply(slot, meshAttachment);
			rawVertexCount += (meshAttachment->getWorldVerticesLength() >> 1);
			rawIndicesCount += (meshAttachment->getTriangles().size());
			regionIndex = static_cast<MeshAttachment*>(attachment)->getHashCode();
		}
		else if (attachment->getRTTI().isExactly(ClippingAttachment::rtti))
		{
			current->hasClipping = true;
			skeletonHasClipping = true;
		}
	}

	current->rawVertexCount = rawVertexCount;
	current->rawTriangleCount = rawIndicesCount;

	instruction->rawVertexCount = rawVertexCount;
	instruction->hasActiveClipping = skeletonHasClipping;

	if (rawVertexCount > 0) {
		instruction->submeshInstructions.setSizeWithoutConstruct(1);
		instruction->submeshInstructions[0] = current;
	} 
	else
	{
		instruction->submeshInstructions.setSizeWithoutConstruct(0);

		delete current;
	}



	outParams[0] = rawVertexCount;
	outParams[1] = rawIndicesCount;
	outParams[2] = regionIndex;
	outParams[3] = skeletonHasClipping ? 1 : 0;

}

bool spine_mesh_generator_generate_instruction_is_not_equal_unity(intptr_t meshGeneratorHandle)
{
	CHECK_MESH_GENERATOR_RETURN(meshGeneratorHandle, true);

	MeshGenerator* generator = s_global_mesh_generator_map[meshGeneratorHandle];

	return generator->IsInstructionNotEqual();
}


void spine_mesh_generator_build_mesh_get_buffer_color32_unity(intptr_t generatorHandle, bool updateTriangles, float meshScale,
	float* outVertex, float* outUv, uint32_t* outColor, float* outBounds, int* outTriangles, bool calcTangents, float* outTangent)
{
	CHECK_MESH_GENERATOR_RETURN(generatorHandle, );

	MeshGenerator* generator = s_global_mesh_generator_map[generatorHandle];
	if (!generator) {
		return;
	}
	generator->Begin();
	generator->BuildMeshWithArraysWithBuffersColor32(generator->newInstruction, updateTriangles, (Vector3*)(outVertex), (Vector2*)(outUv), (uint32_t*)(outColor));
	generator->ScaleVertexDataWithBuffers(generator->newInstruction, meshScale, (Vector3*)outVertex);
	generator->getMeshBounds(outBounds, &outBounds[2]);
	if (updateTriangles)
	{
		generator->getSubMeshes(generator->newInstruction, outTriangles);
	}
	if (calcTangents)
	{
		generator->FillLateVertexDataWithBuffers((Vector4*)outTangent, generator->newInstruction.rawVertexCount);
	}
}

void spine_mesh_generator_build_mesh_index_get_buffer_color32_unity(intptr_t generatorHandle, int submeshIndex, bool updateTriangles, float meshScale,
	float* outVertex, float* outUv, uint32_t* outColor, float* outBounds, int* outTriangles, bool calcTangents, float* outTangent)
{
	CHECK_MESH_GENERATOR_RETURN(generatorHandle, );

	MeshGenerator* generator = s_global_mesh_generator_map[generatorHandle];
	if (!generator) {
		return;
	}
	generator->Begin();
	generator->BuildMeshSubmeshWithArraysWithBuffersColor32(generator->newInstruction, submeshIndex, updateTriangles, (Vector3*)(outVertex), (Vector2*)(outUv), (uint32_t*)(outColor));
	generator->ScaleSubmeshVertexDataWithBuffers(generator->newInstruction, submeshIndex, meshScale, (Vector3*)outVertex);
	generator->getMeshBounds(outBounds, &outBounds[2]);
	if (updateTriangles)
	{
		generator->getSubMeshes(submeshIndex, outTriangles);
	}
	if (calcTangents)
	{
		generator->FillSubmeshLateVertexDataWithBuffers(submeshIndex, (Vector4*)outTangent, generator->newInstruction.rawVertexCount);
	}
}



void spine_mesh_generator_generate_multiple_instruction_unity(intptr_t skeletonHandle, intptr_t meshGeneratorHandle,
	bool generateMeshOverride, bool immutableTriangles, int* separatorSlotIndex, int separatorCount, int* outParams)
{
	CHECK_MESH_GENERATOR_RETURN(meshGeneratorHandle, );
	CHECK_SKELETON_RETURN(skeletonHandle, );

	MeshGenerator* generator = s_global_mesh_generator_map[meshGeneratorHandle];
	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];


	generator->oldInstruction[!generator->instructionIndex].DeleteAll();

	generator->oldInstruction[!generator->instructionIndex].moveTo(generator->newInstruction);
	SkeletonRendererInstruction* instruction = &generator->newInstruction;
	instruction->Clear();
	Vector<Slot*>& drawOrder = skeleton->getDrawOrder();

	int drawOrderCount = skeleton->getDrawOrder().size();
	instruction->attachments.setSizeWithoutConstruct(drawOrderCount);

	SubmeshInstruction* current = new SubmeshInstruction();
	current->skeleton = skeleton;
	current->preActiveClippingSlotSource = -1;


	int rawVertexCount = 0, rawIndicesCount = 0;
	int regionSlotIndex = -1;
	int regionHash = -1;

	bool skeletonHasClipping = false;

	bool hasSeparators = separatorCount > 0;
	int clippingAttachmentSource = -1;
	int lastPreActiveClipping = -1;
	SlotData* clippingEndSlot = nullptr;
	int submeshIndex = 0;


	//input
	Vector<Slot*> separatorSlots;
	separatorSlots.setSizeWithoutConstruct(separatorCount);
	for (int i = 0; i < separatorCount; ++i)
	{
		separatorSlots[i] = skeleton->getSlots()[separatorSlotIndex[i]];
	}

	Color* attachmentColor;
	uint32_t curColor;
	int pageIndex = -1;
	void* texturePtr = nullptr;
	String texturePath;


	for (int i = 0; i < drawOrderCount; ++i)
	{
		Slot* slot = drawOrder[i];
		if (!slot->getBone().isActive())
		{
			(instruction->attachments)[i] = nullptr;
			continue;
		}
		if (slot->getData().getBlendMode() == BlendMode_Additive) current->hasPMAAdditiveSlot = true;

		Attachment* attachment = slot->getAttachment();
		(instruction->attachments)[i] = attachment;

		bool noRender = false;
		int attachmentVertexCount = 0, attachmentTriangleCount = 0;

		if (attachment == NULL) {
			continue;
		}
		if (attachment->getRTTI().isExactly(RegionAttachment::rtti))
		{
			RegionAttachment* regionAttachment = static_cast<RegionAttachment*>(attachment);
			if (regionAttachment->getSequence() != nullptr) regionAttachment->getSequence()->apply(slot, regionAttachment);
			//rawVertexCount += 4;
			//rawIndicesCount += 6;
			attachmentVertexCount = 4;
			attachmentTriangleCount = 6;
			regionSlotIndex = slot->getData().getIndex();
			regionHash = static_cast<RegionAttachment*>(attachment)->getHashCode();
			pageIndex = static_cast<AtlasRegion*>(regionAttachment->getRegion())->page->index;
			texturePath = static_cast<AtlasRegion*>(regionAttachment->getRegion())->page->texturePath;
			attachmentColor = &regionAttachment->getColor();
		}
		else if (attachment->getRTTI().isExactly(MeshAttachment::rtti))
		{
			MeshAttachment* meshAttachment = static_cast<MeshAttachment*>(attachment);
			if (meshAttachment->getSequence() != nullptr) meshAttachment->getSequence()->apply(slot, meshAttachment);
			//rawVertexCount += (meshAttachment->getWorldVerticesLength() >> 1);
			//rawIndicesCount += (meshAttachment->getTriangles().size());
			attachmentVertexCount = (meshAttachment->getWorldVerticesLength() >> 1);
			attachmentTriangleCount = (meshAttachment->getTriangles().size());
			regionSlotIndex = slot->getData().getIndex();
			regionHash = static_cast<MeshAttachment*>(attachment)->getHashCode();
			pageIndex = static_cast<AtlasRegion*>(meshAttachment->getRegion())->page->index;
			texturePath = static_cast<AtlasRegion*>(meshAttachment->getRegion())->page->texturePath;
			attachmentColor = &meshAttachment->getColor();

		}
		else if (attachment->getRTTI().isExactly(ClippingAttachment::rtti))
		{
			current->hasClipping = true;
			skeletonHasClipping = true;
			ClippingAttachment* clippingAttachment = static_cast<ClippingAttachment*>(attachment);
			clippingEndSlot = clippingAttachment->getEndSlot();
			clippingAttachmentSource = i;
		} 
		else
		{
			noRender = true;
		}

		if (hasSeparators)
		{
			current->forceSeparate = false;
			for (int s = 0; s < separatorCount; ++s)
			{
				if (slot == separatorSlots[s])
				{
					current->forceSeparate = true;
					break;
				}
			}
		}

		if (noRender)
		{
			if (current->forceSeparate && generateMeshOverride){
				{
					current->endSlot = i;
					current->preActiveClippingSlotSource = lastPreActiveClipping;

					instruction->submeshInstructions.add(current);
					submeshIndex++;

					current = new SubmeshInstruction();

					current->skeleton = skeleton;
					current->preActiveClippingSlotSource = lastPreActiveClipping;
				}

				current->startSlot = i;
				lastPreActiveClipping = clippingAttachmentSource;
				current->rawTriangleCount = 0;
				current->rawVertexCount = 0;
				current->rawFirstVertexIndex = rawVertexCount;
				current->hasClipping = clippingAttachmentSource >= 0;
				current->atlasPage = pageIndex;
				current->texturePath = texturePath;
			}
		}
		else
		{
			if (attachmentColor) {
				uint8_t r = static_cast<uint8_t>(skeleton->getColor().r * slot->getColor().r * attachmentColor->r * 255);
				uint8_t g = static_cast<uint8_t>(skeleton->getColor().g * slot->getColor().g * attachmentColor->g * 255);
				uint8_t b = static_cast<uint8_t>(skeleton->getColor().b * slot->getColor().b * attachmentColor->b * 255);
				uint8_t a = static_cast<uint8_t>(skeleton->getColor().a * slot->getColor().a * attachmentColor->a * 255);
				curColor = (a << 24) | (r << 16) | (g << 8) | b;
			}


			if (current->forceSeparate || 
				(current->rawVertexCount > 0 && !(current->atlasPage == pageIndex
												&& current->blendMode == slot->getData().getBlendMode() 
												//&& current->colors[0] != curColor
												&& current->texturePath == texturePath
												)))
			{
				current->endSlot = i;
				current->preActiveClippingSlotSource = lastPreActiveClipping;

				instruction->submeshInstructions.add(current);
				submeshIndex++;

				current = new SubmeshInstruction();
				current->skeleton = skeleton;
				current->preActiveClippingSlotSource = lastPreActiveClipping;

				current->startSlot = i;
				lastPreActiveClipping = clippingAttachmentSource;
				current->rawTriangleCount = 0;
				current->rawVertexCount = 0;
				current->rawFirstVertexIndex = rawVertexCount;
				current->hasClipping = clippingAttachmentSource >= 0;
				

			}

			current->regionHashCode = regionHash;
			current->regionSlotIndex = regionSlotIndex;
			current->rawTriangleCount += attachmentTriangleCount;
			current->rawVertexCount += attachmentVertexCount;
			current->rawFirstVertexIndex = rawVertexCount;
			rawVertexCount += attachmentVertexCount;
			current->atlasPage = pageIndex;
			current->texturePath = texturePath;
		}

		if (clippingEndSlot != nullptr && (&slot->getData()) == clippingEndSlot && i != clippingAttachmentSource)
		{
			clippingEndSlot = nullptr;
			clippingAttachmentSource = -1;
		}
	}

	if (current->rawVertexCount > 0)
	{
		current->endSlot = drawOrderCount;
		current->preActiveClippingSlotSource = lastPreActiveClipping;
		current->forceSeparate = false;

		instruction->submeshInstructions.add(current);
	}


	instruction->rawVertexCount = rawVertexCount;
	instruction->hasActiveClipping = skeletonHasClipping;
	instruction->immutableTriangles = immutableTriangles;




	outParams[0] = rawVertexCount;
	outParams[1] = rawIndicesCount;
	outParams[2] = instruction->submeshInstructions.size();
	outParams[3] = skeletonHasClipping ? 1 : 0;
}

void spine_mesh_generator_generate_multiple_instruction_results_unity(intptr_t skeletonHandle, intptr_t meshGeneratorHandle,
	int* outRegionHashCode, int* outEndSlotIndex, int* outSubmeshTriangleCount, int* outSubmeshVertexCount, int* outRegionIndex)
{
	CHECK_MESH_GENERATOR_RETURN(meshGeneratorHandle, );
	CHECK_SKELETON_RETURN(skeletonHandle, );

	MeshGenerator* generator = s_global_mesh_generator_map[meshGeneratorHandle];
	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];

	SkeletonRendererInstruction* instruction = &generator->newInstruction;

	for (int i = 0; i < instruction->submeshInstructions.size(); ++i)
	{
		outRegionHashCode[i] = instruction->submeshInstructions[i]->regionHashCode;
		outEndSlotIndex[i] = instruction->submeshInstructions[i]->endSlot;
		outSubmeshTriangleCount[i] = instruction->submeshInstructions[i]->rawTriangleCount;
		outSubmeshVertexCount[i] = instruction->submeshInstructions[i]->rawVertexCount;
		outRegionIndex[i] = instruction->submeshInstructions[i]->regionSlotIndex;
	}
}


void spine_mesh_generator_generate_mesh_renderers_unity(intptr_t meshGeneratorHandle, intptr_t skeletonHandle, int* outParams)
{
	CHECK_MESH_GENERATOR_RETURN(meshGeneratorHandle, );

	MeshGenerator* meshGenerator = s_global_mesh_generator_map[meshGeneratorHandle];
	Skeleton* skeleton = s_global_spine_skeleton_map[skeletonHandle];

	meshGenerator->generateMeshRenderers(skeleton);

	outParams[0] = meshGenerator->multipleInstruction.submeshInstructions.size();
	outParams[1] = meshGenerator->multipleInstruction.rawVertexCount;
	outParams[2] = meshGenerator->multipleInstruction.rawTriangleCount;

}

int spine_mesh_generator_get_mesh_renderers_count_unity(intptr_t meshGeneratorHandle)
{
	CHECK_MESH_GENERATOR_RETURN(meshGeneratorHandle, 0);

	MeshGenerator* meshGenerator = s_global_mesh_generator_map[meshGeneratorHandle];

	return meshGenerator->multipleInstruction.submeshInstructions.size();
}

void spine_mesh_generator_get_page_indexes_unity(intptr_t meshGeneratorHandle, int32_t* pageIndexes)
{
	CHECK_MESH_GENERATOR_RETURN(meshGeneratorHandle, );

	MeshGenerator* meshGenerator = s_global_mesh_generator_map[meshGeneratorHandle];
	auto& submeshes = meshGenerator->multipleInstruction.submeshInstructions;
	for (int i = 0; i < submeshes.size(); ++i)
	{
		pageIndexes[i] = submeshes[i]->atlasPage;
	}
}

void spine_mesh_generator_get_size_unity(intptr_t meshGeneratorHandle, int meshIndex, int* outVertexCount, int* outIndicesCount)
{
	CHECK_MESH_GENERATOR_RETURN(meshGeneratorHandle, );

	MeshGenerator* meshGenerator = s_global_mesh_generator_map[meshGeneratorHandle];
	auto& submeshes = meshGenerator->multipleInstruction.submeshInstructions;
	auto& instruction = submeshes[meshIndex];

	*outVertexCount = (instruction->positions.size() >> 1);
	*outIndicesCount = instruction->indices.size();
}

void spine_mesh_generator_update_data_unity(intptr_t meshGeneratorHandle, int meshIndex, float* outVertexes, float* outUvs, int32_t* outColors, uint16_t* outIndices
	, float meshScale)
{
	CHECK_MESH_GENERATOR_RETURN(meshGeneratorHandle, );

	MeshGenerator* meshGenerator = s_global_mesh_generator_map[meshGeneratorHandle];
	auto& submeshes = meshGenerator->multipleInstruction.submeshInstructions;
	auto& instruction = submeshes[meshIndex];

	for (int i = 0; i < instruction->positions.size(); ++i)
	{
		outVertexes[i] = instruction->positions[i] * meshScale;
	}
	memcpy(outUvs, instruction->uvs.buffer(), instruction->uvs.size() * sizeof(float));
	memcpy(outColors, instruction->colors.buffer(), instruction->colors.size() * sizeof(int));
	memcpy(outIndices, instruction->indices.buffer(), instruction->indices.size() * sizeof(unsigned short));
}

void spine_mesh_generator_update_multiple_data_unity(intptr_t meshGeneratorHandle, int meshIndex, Vector3* outVertexes, Vector2* outUvs, Vector4* outColors, uint16_t* outIndices, float meshScale)
{
	CHECK_MESH_GENERATOR_RETURN(meshGeneratorHandle, );

	MeshGenerator* meshGenerator = s_global_mesh_generator_map[meshGeneratorHandle];
	auto& submeshes = meshGenerator->multipleInstruction.submeshInstructions;
	auto& instruction = submeshes[meshIndex];

	for (int i = 0, vi = 0, n = instruction->positions.size() >> 1; vi < n; vi++)
	{
		outVertexes[vi].x = instruction->positions[i++] * meshScale;
		outVertexes[vi].y = instruction->positions[i++] * meshScale;
		outVertexes[vi].z = meshIndex * meshGenerator->settings.zSpacing;
		
		uint32_t& color = instruction->colors[vi];
		outColors[vi].w = (uint8_t)(color >> 24);
		outColors[vi].x = (uint8_t)(color >> 16);
		outColors[vi].y = (uint8_t)(color >> 8);
		outColors[vi].z = (uint8_t)(color);
	}
	memcpy(outUvs, instruction->uvs.buffer(), instruction->uvs.size() * sizeof(float));
	memcpy(outIndices, instruction->indices.buffer(), instruction->indices.size() * sizeof(unsigned short));
}

void spine_mesh_generator_get_results_unity(intptr_t meshGeneratorHandle, float meshScale, int* outVertexCount, int* outIndicesCount,
	float* outVertex, float* outUv, uint32_t* outColor, float* outBounds, uint16_t* outIndices, bool calcTangents, float* outTangent)
{
	CHECK_MESH_GENERATOR_RETURN(meshGeneratorHandle, );

	MeshGenerator* meshGenerator = s_global_mesh_generator_map[meshGeneratorHandle];
	auto& submeshes = meshGenerator->multipleInstruction.submeshInstructions;

	Vector3* vertexVec3 = (Vector3*)outVertex;

	for (int smbi = 0, smbn = submeshes.size(); smbi < smbn; ++smbi)
	{
		auto& instruction = submeshes[smbi];

		for (int i = 0, vi = 0, n = instruction->positions.size() >> 1; vi < n; vi++)
		{
			vertexVec3[vi].x = instruction->positions[i++] * meshScale;
			vertexVec3[vi].y = instruction->positions[i++] * meshScale;
			vertexVec3[vi].z = smbi * meshGenerator->settings.zSpacing;
		}

		memcpy(outUv, instruction->uvs.buffer(), instruction->uvs.size() * sizeof(float));
		memcpy(outColor, instruction->colors.buffer(), instruction->colors.size() * sizeof(uint32_t));
		
		memcpy(outIndices, instruction->indices.buffer(), instruction->indices.size() * sizeof(unsigned short));

		vertexVec3 += (instruction->positions.size() >> 1);
		outUv += instruction->uvs.size();
		outColor += instruction->colors.size();
		outIndices += instruction->indices.size();

		outVertexCount[smbi] = (instruction->positions.size() >> 1);
		outIndicesCount[smbi] = instruction->indices.size();
	}

	if (calcTangents)
	{

	}




}

// timeline
template <typename TimelineType, typename... Args>
intptr_t create_timeline(Args&&... args)
{
	TimelineType* timeline = new TimelineType(std::forward<Args>(args)...);
	intptr_t handle = reinterpret_cast<intptr_t>(timeline);
	s_global_spine_timeline_map.put(handle, timeline);
	return handle;
}

// Note: This class only to be used for get Timeline's propertyIds & frames
class ManagedTempTimeline : public Timeline {
public:
	ManagedTempTimeline(size_t frameCount, size_t frameEntries, PropertyId propertyIds[], size_t propertyIdsCount)
			: Timeline(frameCount, frameEntries) {
		setPropertyIds(propertyIds, propertyIdsCount);
	}
	virtual void apply(Skeleton &skeleton, float lastTime, float time, Vector<Event *> *pEvents,
		float alpha, MixBlend blend, MixDirection direction){}
};

intptr_t spine_timeline_create_unity(size_t frameCount, size_t frameEntries, long long* propertyIds, size_t propertyIdsCount)
{
	PropertyId* ids = reinterpret_cast<PropertyId*>(propertyIds);
	auto handle = create_timeline<ManagedTempTimeline>(frameCount, frameEntries, ids, propertyIdsCount);
	return handle;
}

intptr_t spine_curve_timeline_create_unity(size_t frameCount, size_t bezierCount, int intParam, const char* className)
{
    if (className == "RotateTimeline") {
        return create_timeline<RotateTimeline>(frameCount, bezierCount, intParam);
    } else if (className == "TranslateTimeline") {
	    return create_timeline<TranslateTimeline>(frameCount, bezierCount, intParam);
    } else if (className == "TranslateXTimeline") {
        return create_timeline<TranslateXTimeline>(frameCount, bezierCount, intParam);
    } else if (className == "TranslateYTimeline") {
        return create_timeline<TranslateYTimeline>(frameCount, bezierCount, intParam);
    } else if (className == "ScaleTimeline") {
        return create_timeline<ScaleTimeline>(frameCount, bezierCount, intParam);
    } else if (className == "ScaleXTimeline") {
        return create_timeline<ScaleXTimeline>(frameCount, bezierCount, intParam);
    } else if (className == "ScaleYTimeline") {
        return create_timeline<ScaleYTimeline>(frameCount, bezierCount, intParam);
    } else if (className == "ShearTimeline") {
        return create_timeline<ShearTimeline>(frameCount, bezierCount, intParam);
    } else if (className == "ShearXTimeline") {
        return create_timeline<ShearXTimeline>(frameCount, bezierCount, intParam);
    } else if (className == "ShearYTimeline") {
        return create_timeline<ShearYTimeline>(frameCount, bezierCount, intParam);
    } else if (className == "RGBATimeline") {
        return create_timeline<RGBATimeline>(frameCount, bezierCount, intParam);
    } else if (className == "RGBTimeline") {
        return create_timeline<RGBTimeline>(frameCount, bezierCount, intParam);
    } else if (className == "AlphaTimeline") {
        return create_timeline<AlphaTimeline>(frameCount, bezierCount, intParam);
    } else if (className == "RGBA2Timeline") {
        return create_timeline<RGBA2Timeline>(frameCount, bezierCount, intParam);
    } else if (className == "RGB2Timeline") {
        return create_timeline<RGB2Timeline>(frameCount, bezierCount, intParam);
    } else if (className == "IkConstraintTimeline") {
        return create_timeline<IkConstraintTimeline>(frameCount, bezierCount, intParam);
    } else if (className == "TransformConstraintTimeline") {
        return create_timeline<TransformConstraintTimeline>(frameCount, bezierCount, intParam);
    } else if (className == "PathConstraintPositionTimeline") {
        return create_timeline<PathConstraintPositionTimeline>(frameCount, bezierCount, intParam);
    } else if (className == "PathConstraintSpacingTimeline") {
        return create_timeline<PathConstraintSpacingTimeline>(frameCount, bezierCount, intParam);
    } else if (className == "PathConstraintMixTimeline") {
        return create_timeline<PathConstraintMixTimeline>(frameCount, bezierCount, intParam);
    } else {
        return 0;
    }
}

intptr_t spine_attachment_timeline_create_unity(int frameCount, int slotIndex)
{
	return create_timeline<AttachmentTimeline>(frameCount, slotIndex);
}

void spine_timeline_destroy_unity(intptr_t timelineHandle)
{
	if(timelineHandle == 0) {
		return;
	}
	auto* timeline = s_global_spine_timeline_map[timelineHandle];
	s_global_spine_timeline_map.remove(timelineHandle);
	delete timeline;
}

long long* spine_timeline_get_propertyIds(intptr_t timelineHandle) {
	if(timelineHandle == 0) {
		return 0;
	}
	auto timeline = s_global_spine_timeline_map[timelineHandle];
	if(timeline != nullptr) {
		return timeline->getPropertyIds().buffer();
	}
	return 0;
}

float* spine_timeline_get_frames(intptr_t timelineHandle) {
	if(timelineHandle == 0) {
		return nullptr;
	}
	auto timeline = s_global_spine_timeline_map[timelineHandle];
	if (timeline != nullptr) {
		return timeline->getFrames().buffer();
	}
	return nullptr;
}
void spine_timeline_set_frame(intptr_t timelineHandle, int index, float value) {
	if(timelineHandle == 0) {
		return;
	}
	auto timeline = s_global_spine_timeline_map[timelineHandle];
	if(timeline != nullptr) {
		auto frames = timeline->getFrames();
		if(frames.size() > index) {
			frames[index] = value;
		}
	}
}


int spine_attachment_timeline_get_slotIndex(intptr_t timelineHandle) {
	if(timelineHandle == 0) {
		return 0;
	}
	auto attachmentTimeline = dynamic_cast<AttachmentTimeline*>(s_global_spine_timeline_map[timelineHandle]);
	return attachmentTimeline->getSlotIndex();
}

const char* spine_attachment_timeline_get_attachmentName(intptr_t timelineHandle, int frame) {
	if(timelineHandle == 0) {
		return nullptr;
	}
	auto attachmentTimeline = dynamic_cast<AttachmentTimeline*>(s_global_spine_timeline_map[timelineHandle]);
	return attachmentTimeline->getAttachmentNames()[frame].buffer();
}

void spine_attachment_timeline_set_attachmentName(intptr_t timelineHandle, int frame, const char* value) {
	if(timelineHandle == 0) {
		return;
	}
	auto attachmentTimeline = dynamic_cast<AttachmentTimeline*>(s_global_spine_timeline_map[timelineHandle]);
	if(attachmentTimeline != nullptr) {
		auto attachmentNames = attachmentTimeline->getAttachmentNames();
		if(attachmentNames.size() > frame) {
			attachmentNames[frame] = value;
		}
	}
}







