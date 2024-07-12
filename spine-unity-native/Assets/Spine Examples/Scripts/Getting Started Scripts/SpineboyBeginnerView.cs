/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated July 28, 2023. Replaces all prior versions.
 *
 * Copyright (c) 2013-2023, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software or
 * otherwise create derivative works of the Spine Runtimes (collectively,
 * "Products"), provided that each user of the Products must obtain their own
 * Spine Editor license and redistribution of the Products in any form must
 * include this license and copyright notice.
 *
 * THE SPINE RUNTIMES ARE PROVIDED BY ESOTERIC SOFTWARE LLC "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ESOTERIC SOFTWARE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES,
 * BUSINESS INTERRUPTION, OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THE
 * SPINE RUNTIMES, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

using Spine.Unity;
using System;
using System.Collections;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Spine.Unity.Examples {
	public class SpineboyBeginnerView : MonoBehaviour {

		#region Inspector
		[Header("Components")]
		public SpineboyBeginnerModel model;
		public SkeletonAnimation skeletonAnimation;

		public AnimationReferenceAsset run, idle, aim, shoot, jump;
		public EventDataReferenceAsset footstepEvent;

		[Header("Audio")]
		public float footstepPitchOffset = 0.2f;
		public float gunsoundPitchOffset = 0.13f;
		public AudioSource footstepSource, gunSource, jumpSource;

		[Header("Effects")]
		public ParticleSystem gunParticles;
		#endregion

		SpineBeginnerBodyState previousViewState;

		void Start () {
			if (skeletonAnimation == null) return;
			model.ShootEvent += PlayShoot;
			model.StartAimEvent += StartPlayingAim;
			model.StopAimEvent += StopPlayingAim;
			skeletonAnimation.AnimationState.Event += HandleEvent;
		}

        //void HandleEvent (Spine.TrackEntry trackEntry, Spine.Event e) {
        //	if (e.Data == footstepEvent.EventData)
        //		PlayFootstepSound();
        //}

        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern IntPtr spine_event_get_event_data_handle_unity(IntPtr eventHandle);

        void HandleEvent(Spine.TrackEntry trackEntry, IntPtr eHandle)
        {
			if (spine_event_get_event_data_handle_unity(eHandle) == footstepEvent.EventData.eventDataHandle)
				PlayFootstepSound();
        }


        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_skeleton_get_scale_x_unity(IntPtr skeletonHandle);

        void Update () {
			if (skeletonAnimation == null) return;
			if (model == null) return;


			float scaleX = spine_skeleton_get_scale_x_unity(skeletonAnimation.Skeleton.skeletonHandle);

			//if ((skeletonAnimation.skeleton.ScaleX < 0) != model.facingLeft) {  // Detect changes in model.facingLeft
			//	Turn(model.facingLeft);
			//}
            if ((scaleX < 0) != model.facingLeft)
            {  // Detect changes in model.facingLeft
                Turn(model.facingLeft);
            }


            // Detect changes in model.state
            SpineBeginnerBodyState currentModelState = model.state;

			if (previousViewState != currentModelState) {
				PlayNewStableAnimation();
			}

			previousViewState = currentModelState;
		}

		void PlayNewStableAnimation () {
			SpineBeginnerBodyState newModelState = model.state;
			Animation nextAnimation;

			// Add conditionals to not interrupt transient animations.

			if (previousViewState == SpineBeginnerBodyState.Jumping && newModelState != SpineBeginnerBodyState.Jumping) {
				PlayFootstepSound();
			}

			if (newModelState == SpineBeginnerBodyState.Jumping) {
				jumpSource.Play();
				nextAnimation = jump;
			} else {
				if (newModelState == SpineBeginnerBodyState.Running) {
					nextAnimation = run;
				} else {
					nextAnimation = idle;
				}
			}

			skeletonAnimation.AnimationState.SetAnimation(0, nextAnimation, true);
		}

		void PlayFootstepSound () {
			footstepSource.Play();
			footstepSource.pitch = GetRandomPitch(footstepPitchOffset);
		}

		[ContextMenu("Check Tracks")]
		void CheckTracks () {
			AnimationState state = skeletonAnimation.AnimationState;
			Debug.Log(state.GetCurrent(0));
			Debug.Log(state.GetCurrent(1));
		}


        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern IntPtr spine_skeleton_data_find_animation_unity(IntPtr skeletonDataHandle, string startingAnimation);



        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern void spine_animation_state_track_set_attachment_threshold_unity(IntPtr animationStateHandle, int trackIndex, float attachmentThreshold);

        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern void spine_animation_state_track_set_mix_duration_unity(IntPtr animationStateHandle, int trackIndex, float mixDuration);

        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern void spine_animation_state_add_empty_animation_unity(IntPtr animationStateHandle, int trackIndex, float mixDuration, float delay);


        #region Transient Actions
        public void PlayShoot () {
			// Play the shoot animation on track 1.
			{
                //TrackEntry shootTrack = skeletonAnimation.AnimationState.SetAnimation(1, shoot, false);
                skeletonAnimation.AnimationState.SetAnimation(1, shoot, false);
                spine_animation_state_track_set_attachment_threshold_unity(skeletonAnimation.state.animationStateHandle, 1, 1f);
                spine_animation_state_track_set_mix_duration_unity(skeletonAnimation.state.animationStateHandle, 1, 0f);
                spine_animation_state_add_empty_animation_unity(skeletonAnimation.state.animationStateHandle, 1, 0.5f, 0.1f);
                //shootTrack.AttachmentThreshold = 1f;
                //shootTrack.MixDuration = 0f;
                //skeletonAnimation.state.AddEmptyAnimation(1, 0.5f, 0.1f);
            }


			// Play the aim animation on track 2 to aim at the mouse target.
			//TrackEntry aimTrack = skeletonAnimation.AnimationState.SetAnimation(2, aim, false);
			{
                skeletonAnimation.AnimationState.SetAnimation(2, aim, false);
                spine_animation_state_track_set_attachment_threshold_unity(skeletonAnimation.state.animationStateHandle, 2, 1f);
                spine_animation_state_track_set_mix_duration_unity(skeletonAnimation.state.animationStateHandle, 2, 0f);
                spine_animation_state_add_empty_animation_unity(skeletonAnimation.state.animationStateHandle, 2, 0.5f, 0.1f);

                //aimTrack.AttachmentThreshold = 1f;
                //aimTrack.MixDuration = 0f;
                //skeletonAnimation.state.AddEmptyAnimation(2, 0.5f, 0.1f);
            }



			gunSource.pitch = GetRandomPitch(gunsoundPitchOffset);
			gunSource.Play();
			//gunParticles.randomSeed = (uint)Random.Range(0, 100);
			gunParticles.Play();
		}

		public void StartPlayingAim () {
			// Play the aim animation on track 2 to aim at the mouse target.
			//TrackEntry aimTrack = skeletonAnimation.AnimationState.SetAnimation(2, aim, true);
			//aimTrack.AttachmentThreshold = 1f;
			//aimTrack.MixDuration = 0f;
			{
                skeletonAnimation.AnimationState.SetAnimation(2, aim, true);
                spine_animation_state_track_set_attachment_threshold_unity(skeletonAnimation.state.animationStateHandle, 2, 1f);
                spine_animation_state_track_set_mix_duration_unity(skeletonAnimation.state.animationStateHandle, 2, 0f);
            }
		}

		public void StopPlayingAim () {
            //skeletonAnimation.state.AddEmptyAnimation(2, 0.5f, 0.1f);
            spine_animation_state_add_empty_animation_unity(skeletonAnimation.state.animationStateHandle, 2, 0.5f, 0.1f);
        }

        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern void spine_skeleton_set_scale_x_unity(IntPtr skeletonHandle, float scaleX);

        public void Turn (bool facingLeft) {
			//skeletonAnimation.Skeleton.ScaleX = facingLeft ? -1f : 1f;
			// Maybe play a transient turning animation too, then call ChangeStableAnimation.

			spine_skeleton_set_scale_x_unity(skeletonAnimation.Skeleton.skeletonHandle, facingLeft ? -1f: 1f);
		}
		#endregion

		#region Utility
		public float GetRandomPitch (float maxPitchOffset) {
			return 1f + UnityEngine.Random.Range(-maxPitchOffset, maxPitchOffset);
		}
		#endregion
	}

}
