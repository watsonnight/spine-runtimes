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

using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Spine.Unity.Examples {
	public class SpineboyTargetController : MonoBehaviour {

		public SkeletonAnimation skeletonAnimation;

		[SpineBone(dataField: "skeletonAnimation")]
		public string boneName;
		public Camera cam;

		//Bone bone;
		IntPtr boneHandle = IntPtr.Zero;

		void OnValidate () {
			if (skeletonAnimation == null) skeletonAnimation = GetComponent<SkeletonAnimation>();
		}

        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern IntPtr spine_skeleton_find_bone_unity(IntPtr skeletonHandle, string boneName);



        void Start () {
			//bone = skeletonAnimation.Skeleton.FindBone(boneName);
			boneHandle = spine_skeleton_find_bone_unity(skeletonAnimation.Skeleton.skeletonHandle, boneName);
		}

        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern void spine_bone_set_local_position_unity(IntPtr boneHandle, Vector3 position);

        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_skeleton_get_scale_x_unity(IntPtr skeletonHandle);

        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_skeleton_get_scale_y_unity(IntPtr skeletonHandle);

        void Update () {
			Vector3 mousePosition = Input.mousePosition;
			Vector3 worldMousePosition = cam.ScreenToWorldPoint(mousePosition);
			Vector3 skeletonSpacePoint = skeletonAnimation.transform.InverseTransformPoint(worldMousePosition);
			//skeletonSpacePoint.x *= skeletonAnimation.Skeleton.ScaleX;
			//skeletonSpacePoint.y *= skeletonAnimation.Skeleton.ScaleY;
			//bone.SetLocalPosition(skeletonSpacePoint);

			skeletonSpacePoint.x *= spine_skeleton_get_scale_x_unity(skeletonAnimation.Skeleton.skeletonHandle);
			skeletonSpacePoint.y *= spine_skeleton_get_scale_y_unity(skeletonAnimation.Skeleton.skeletonHandle);

			spine_bone_set_local_position_unity(boneHandle, skeletonSpacePoint);
		}
	}

}
