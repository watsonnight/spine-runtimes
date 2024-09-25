
#ifndef SPINE_CPP_LITE_UNITY
#define SPINE_CPP_LITE_UNITY

#include <cstddef>
#include <cstdint>
#include <stdint.h>
#include <string>

#include "SkeletonRendererInstruction.h"
#include "MeshGenerator.h"

#ifdef __cplusplus
#if _WIN32
#define SPINE_CPP_LITE_EXPORT extern "C" __declspec(dllexport)
#else
#ifdef __EMSCRIPTEN__
#define SPINE_CPP_LITE_EXPORT extern "C" __attribute__((used))
#else
#define SPINE_CPP_LITE_EXPORT extern "C"
#endif
#endif
#else
#if _WIN32
#define SPINE_CPP_LITE_EXPORT __declspec(dllexport)
#else
#ifdef __EMSCRIPTEN__
#define SPINE_CPP_LITE_EXPORT __attribute__((used))
#else
#define SPINE_CPP_LITE_EXPORT
#endif
#endif
#endif


//typedef char utf8;

SPINE_CPP_LITE_EXPORT intptr_t spine_atlas_load_unity(char* atlasData);
SPINE_CPP_LITE_EXPORT void spine_atlas_destroy_unity(intptr_t atlasHandle);
SPINE_CPP_LITE_EXPORT void spine_atlas_flipV_unity(intptr_t atlasHandle);
SPINE_CPP_LITE_EXPORT intptr_t spine_atlas_get_region_unity(intptr_t atlasHandle, int index);
SPINE_CPP_LITE_EXPORT int spine_atlas_get_region_count_unity(intptr_t atlasHandle);
SPINE_CPP_LITE_EXPORT const char* spine_atlas_get_region_name_unity(intptr_t atlasHandle, int index);
// SPINE_CPP_LITE_EXPORT intptr_t spine_atlasregion_get_pages_unity(intptr_t atlasHandle, intptr_t regionHandle);


//bone
// SPINE_CPP_LITE_EXPORT float spine_bone_get_x_unity(intptr_t boneHandle);
// SPINE_CPP_LITE_EXPORT float spine_bone_get_y_unity(intptr_t boneHandle);
// SPINE_CPP_LITE_EXPORT float spine_bone_get_rotation_unity(intptr_t boneHandle);
SPINE_CPP_LITE_EXPORT float spine_bone_get_scale_x_unity(intptr_t boneHandle);
SPINE_CPP_LITE_EXPORT float spine_bone_get_scale_y_unity(intptr_t boneHandle);
// SPINE_CPP_LITE_EXPORT float spine_bone_get_shear_x_unity(intptr_t boneHandle);
// SPINE_CPP_LITE_EXPORT float spine_bone_get_shear_y_unity(intptr_t boneHandle);
SPINE_CPP_LITE_EXPORT float spine_bone_get_world_x_unity(intptr_t boneHandle);
SPINE_CPP_LITE_EXPORT float spine_bone_get_world_y_unity(intptr_t boneHandle);
SPINE_CPP_LITE_EXPORT float spine_bone_get_a_unity(intptr_t boneHandle);
SPINE_CPP_LITE_EXPORT float spine_bone_get_c_unity(intptr_t boneHandle);
SPINE_CPP_LITE_EXPORT float spine_bone_get_world_rotation_x_unity(intptr_t boneHandle);
SPINE_CPP_LITE_EXPORT float spine_bone_get_world_rotation_y_unity(intptr_t boneHandle);
SPINE_CPP_LITE_EXPORT float spine_bone_get_world_scale_x_unity(intptr_t boneHandle);
SPINE_CPP_LITE_EXPORT float spine_bone_get_world_scale_y_unity(intptr_t boneHandle);
SPINE_CPP_LITE_EXPORT float spine_bone_get_data_length_unity(intptr_t boneHandle);
SPINE_CPP_LITE_EXPORT const char* spine_bone_get_data_name_unity(intptr_t boneHandle);
// SPINE_CPP_LITE_EXPORT void spine_bone_set_local_position_unity(intptr_t boneHandle, float* position);
SPINE_CPP_LITE_EXPORT intptr_t spine_bone_get_parent_unity(intptr_t boneHandle);
SPINE_CPP_LITE_EXPORT intptr_t spine_bone_get_skeleton_unity(intptr_t boneHandle);
SPINE_CPP_LITE_EXPORT void spine_bone_dispose_local_unity(intptr_t boneHandle);





SPINE_CPP_LITE_EXPORT intptr_t spine_skeleton_data_load_binary_scale_unity(intptr_t atlasHandle, const uint8_t* skeletonData, int32_t length, float scale);
SPINE_CPP_LITE_EXPORT intptr_t spine_skeleton_data_load_json_scale_unity(intptr_t atlasHandle, const uint8_t* skeletonData, int32_t length, float scale);

SPINE_CPP_LITE_EXPORT void spine_skeleton_data_destroy_unity(intptr_t skeletonDataHandle);
SPINE_CPP_LITE_EXPORT intptr_t spine_skeleton_data_find_animation_unity(intptr_t skeletonDataHandle, const char* startingAnimationName);
SPINE_CPP_LITE_EXPORT void spine_skeleton_data_remove_animation_unity(intptr_t animationHandle);
SPINE_CPP_LITE_EXPORT intptr_t spine_skeleton_data_find_event_data_unity(intptr_t skeletonDataHandle, const char* eventName);


SPINE_CPP_LITE_EXPORT intptr_t spine_skeleton_create_unity(intptr_t skeletonDataHandle);
SPINE_CPP_LITE_EXPORT void spine_skeleton_destroy_unity(intptr_t skeletonHandle);
SPINE_CPP_LITE_EXPORT void spine_skeleton_update_world_transform_unity(intptr_t skeletonHandle);
// SPINE_CPP_LITE_EXPORT void spine_skeleton_set_scale_x_unity(intptr_t skeletonHandle, float scaleX);
SPINE_CPP_LITE_EXPORT float spine_skeleton_get_scale_x_unity(intptr_t skeletonHandle);
// SPINE_CPP_LITE_EXPORT void spine_skeleton_set_scale_y_unity(intptr_t skeletonHandle, float scaleY);
SPINE_CPP_LITE_EXPORT float spine_skeleton_get_scale_y_unity(intptr_t skeletonHandle);
SPINE_CPP_LITE_EXPORT void spine_skeleton_set_skin_unity(intptr_t skeletonHandle, const char* skinName);

SPINE_CPP_LITE_EXPORT void spine_skeleton_set_color_unity(intptr_t skeletonHandle, const float* color);
SPINE_CPP_LITE_EXPORT void spine_skeleton_set_color_a_unity(intptr_t skeletonHandle, float alpha);
SPINE_CPP_LITE_EXPORT void spine_skeleton_set_color_r_unity(intptr_t skeletonHandle, float red);
SPINE_CPP_LITE_EXPORT void spine_skeleton_set_color_g_unity(intptr_t skeletonHandle, float green);
SPINE_CPP_LITE_EXPORT void spine_skeleton_set_color_b_unity(intptr_t skeletonHandle, float blue);

SPINE_CPP_LITE_EXPORT intptr_t spine_skeleton_find_bone_unity(intptr_t skeletonHandle, const char* name);
SPINE_CPP_LITE_EXPORT intptr_t spine_skeleton_find_slot_unity(intptr_t skeletonHandle, const char* name);
SPINE_CPP_LITE_EXPORT void spine_skeleton_sync_attachments_unity(intptr_t skeletonHandle, int* slotIndexes, const char** names, int* hashCode, int size);


SPINE_CPP_LITE_EXPORT intptr_t spine_animation_state_data_create_unity(intptr_t skeletonDataHandle);
SPINE_CPP_LITE_EXPORT void spine_animation_state_data_destroy_unity(intptr_t animationStateDataHandle);
SPINE_CPP_LITE_EXPORT intptr_t spine_animation_state_create_unity(intptr_t animationStateDataHandle);
SPINE_CPP_LITE_EXPORT void spine_animation_state_destroy_unity(intptr_t animationStateHandle);


SPINE_CPP_LITE_EXPORT int spine_animation_state_set_and_remove_animation_unity(intptr_t animationStateHandle, int trackIndex, intptr_t animationHandle, bool loop);
SPINE_CPP_LITE_EXPORT int spine_animation_state_add_and_remove_animation_unity(intptr_t animationStateHandle, int trackIndex, intptr_t animationHandle, bool loop, float delay);

SPINE_CPP_LITE_EXPORT bool spine_animation_state_apply_unity(intptr_t animationStateHandle, intptr_t skeletonHandle);
SPINE_CPP_LITE_EXPORT void spine_animation_state_update_unity(intptr_t animationStateHandle, float delta);

typedef void (*eventCallbackDelegate)(intptr_t stateHandle, int trackIndex, int eventType, intptr_t eventHandle);
SPINE_CPP_LITE_EXPORT void spine_animation_state_add_callback_unity(intptr_t animationStateHandle, eventCallbackDelegate cb);
SPINE_CPP_LITE_EXPORT void spine_animation_state_remove_callback_unity(intptr_t animationStateHandle, eventCallbackDelegate cb);

// SPINE_CPP_LITE_EXPORT void spine_animation_state_add_empty_animation_unity(intptr_t animationStateHandle, int trackIndex, float mixDuration, float delay);

// SPINE_CPP_LITE_EXPORT void spine_animation_state_track_set_attachment_threshold_unity(intptr_t animationStateHandle, int trackIndex, float attachmentThreshold);
// SPINE_CPP_LITE_EXPORT void spine_animation_state_track_set_mix_duration_unity(intptr_t animationStateHandle, int trackIndex, float mixDuration);
SPINE_CPP_LITE_EXPORT void spine_animation_state_track_set_time_scale_unity(intptr_t animationStateHandle, int trackIndex, float timeScale);
// SPINE_CPP_LITE_EXPORT float spine_animation_state_track_get_time_scale_unity(intptr_t animationStateHandle, int trackIndex);
SPINE_CPP_LITE_EXPORT void spine_animation_state_track_get_track_time_end_duration_unity(intptr_t animationStateHandle, int trackIndex, float* outTrackTime);

SPINE_CPP_LITE_EXPORT void spine_animation_apply_unity(intptr_t animationHandle, intptr_t skeletonHandle, float lastTime, float time, bool loop, float alpha, int mixBlend, int direction);


//Slot
SPINE_CPP_LITE_EXPORT void spine_slot_dispose_local_unity(intptr_t slotHandle);
SPINE_CPP_LITE_EXPORT void spine_slot_set_attachment_unity(intptr_t slotHandle, intptr_t attachmentHandle);

// SPINE_CPP_LITE_EXPORT intptr_t spine_event_get_event_data_handle_unity(intptr_t eventHandle);


//Attachment

SPINE_CPP_LITE_EXPORT intptr_t spine_region_attachment_create_from_atlas_region_unity(intptr_t atlasHandle, const char* regionName, const char* attachmentName, float scale, float rotation);
SPINE_CPP_LITE_EXPORT void spine_region_attachment_dispose_unity(intptr_t regionAttachmentHandle, intptr_t slotHandle);

SPINE_CPP_LITE_EXPORT intptr_t spine_mesh_generator_create_unity();
SPINE_CPP_LITE_EXPORT void spine_mesh_generator_destroy_unity(intptr_t meshGeneratorHandle);


SPINE_CPP_LITE_EXPORT void spine_mesh_generator_set_settings_unity(intptr_t handle, spine::MeshGenerator::Settings* settings);
SPINE_CPP_LITE_EXPORT void spine_mesh_generator_build_mesh_unity(intptr_t handle, bool updateTriangles);

SPINE_CPP_LITE_EXPORT void spine_mesh_generator_generate_single_instruction_unity(intptr_t skeletonHandle, intptr_t meshGeneratorHandle, int* outParams);
SPINE_CPP_LITE_EXPORT void spine_mesh_generator_generate_multiple_instruction_unity(intptr_t skeletonHandle, intptr_t meshGeneratorHandle, 
    bool generateMeshOverride, bool immutableTriangles, int* separatorSlotIndex, int separatorSlotCount, int* outParams);
SPINE_CPP_LITE_EXPORT void spine_mesh_generator_generate_multiple_instruction_results_unity(intptr_t skeletonHandle, intptr_t meshGeneratorHandle,
    int* outRegionHashCode, int* outEndSlotIndex, int* outSubmeshTriangleCount, int* outSubmeshVertexCount, int* outRegionIndex);

SPINE_CPP_LITE_EXPORT bool spine_mesh_generator_generate_instruction_is_not_equal_unity(intptr_t meshGeneratorHandle);


SPINE_CPP_LITE_EXPORT void spine_mesh_generator_build_mesh_get_buffer_color32_unity(intptr_t generatorHandle, bool updateTriangles, float meshScale,
    float* outVertex, float* outUv, uint32_t* outColor, float* outBounds, int* outTriangles, bool calcTangents, float* outTangent);


SPINE_CPP_LITE_EXPORT void spine_mesh_generator_build_mesh_index_get_buffer_color32_unity(intptr_t generatorHandle, int submeshIndex, bool updateTriangles, float meshScale,
    float* outVertex, float* outUv, uint32_t* outColor, float* outBounds, int* outTriangles, bool calcTangents, float* outTangent);

SPINE_CPP_LITE_EXPORT void spine_mesh_generator_generate_mesh_renderers_unity(intptr_t meshGeneratorHandle, intptr_t skeletonHandle, int* outParams);
SPINE_CPP_LITE_EXPORT int spine_mesh_generator_get_mesh_renderers_count_unity(intptr_t meshGeneratorHandle);
SPINE_CPP_LITE_EXPORT void spine_mesh_generator_get_results_unity(intptr_t meshGeneratorHandle, float meshScale, int* outVertexCount, int* outIndicesCount,
    float* outVertex, float* outUv, uint32_t* outColor, float* outBounds, uint16_t* outIndices, bool calcTangents, float* outTangent);

SPINE_CPP_LITE_EXPORT void spine_mesh_generator_get_page_indexes_unity(intptr_t meshGeneratorHandle, int32_t* pageIndex);
SPINE_CPP_LITE_EXPORT void spine_mesh_generator_get_size_unity(intptr_t meshGeneratorHandle, int meshIndex, int* outVertexCount, int* outIndicesCount);
SPINE_CPP_LITE_EXPORT void spine_mesh_generator_update_multiple_data_unity(intptr_t meshGeneratorHandle, int meshIndex, Vector3* outVertexes, Vector2* outUvs, Vector4* outColors, uint16_t* outIndices, float meshScale);



// Timeline
SPINE_CPP_LITE_EXPORT intptr_t spine_get_timelinehandle_by_index(intptr_t skeletonHandle, int animationindex, int timelineindex);
SPINE_CPP_LITE_EXPORT void spine_timeline_destroy_unity(intptr_t timelineHandle);
SPINE_CPP_LITE_EXPORT long long* spine_timeline_get_propertyIds(intptr_t timelineHandle,int* length);
SPINE_CPP_LITE_EXPORT float* spine_timeline_get_frames(intptr_t timelineHandle, int* length);
SPINE_CPP_LITE_EXPORT void spine_timeline_set_frame(intptr_t timelineHandle, int index, float value);

//CurveTimeline Curve[]
SPINE_CPP_LITE_EXPORT float* spine_timeline_get_curves(intptr_t timelineHandle, int* length);
SPINE_CPP_LITE_EXPORT  void set_liner(intptr_t timelineHandle, int frame);
SPINE_CPP_LITE_EXPORT  void set_stepped(intptr_t timelineHandle, int frame);
SPINE_CPP_LITE_EXPORT  float get_curve_type(intptr_t timelineHandle, int frame);
SPINE_CPP_LITE_EXPORT void set_bezier(intptr_t timelineHandle, int bezier, int frame, int value, float time1, float value1, float cx1, float cy1, float cx2,
    float cy2, float time2, float value2);
SPINE_CPP_LITE_EXPORT float get_bezier_value(intptr_t timelineHandle, float time, int frameIndex, int valueOffset, int i);
SPINE_CPP_LITE_EXPORT void  shrink(intptr_t timelineHandle, int size);

//SPINE_CPP_LITE_EXPORT intptr_t spine_deform_timeline_create_unity(int frameCount, int bezierCount, int slotIndex, int attachmentpid);

// AttachmentTimeline
SPINE_CPP_LITE_EXPORT int spine_timeline_get_slotIndex(intptr_t timelineHandle);
SPINE_CPP_LITE_EXPORT const char* spine_attachment_timeline_get_attachmentName(intptr_t timelineHandle, int frame);
SPINE_CPP_LITE_EXPORT void spine_attachment_timeline_set_attachmentName(intptr_t timelineHandle, int frame, const char* value);
SPINE_CPP_LITE_EXPORT int spine_timeline_get_boneIndex(intptr_t timelineHandle);
//SPINE_CPP_LITE_EXPORT int spine_timeline_get_boneIndex(intptr_t timelineHandle);


// Create Timeline
//SPINE_CPP_LITE_EXPORT intptr_t spine_timeline_create_unity(size_t frameCount, size_t frameEntries, long long* propertyIds, size_t propertyIdsCount);
//SPINE_CPP_LITE_EXPORT intptr_t spine_curve_timeline_create_unity(size_t frameCount, size_t bezierCount, int intParam, const char* className);
//SPINE_CPP_LITE_EXPORT intptr_t spine_attachment_timeline_create_unity(int frameCount, int slotIndex);
#endif