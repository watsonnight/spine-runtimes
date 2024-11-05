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

namespace Spine {
	/// <summary>
	/// Stores a bone's current pose.
	/// <para>
	/// A bone has a local transform which is used to compute its world transform. A bone also has an applied transform, which is a
	/// local transform that can be applied to compute the world transform. The local transform and applied transform may differ if a
	/// constraint or application code modifies the world transform after it was computed from the local transform.
	/// </para>
	/// </summary>
	public class Bone : IUpdatable {
		static public bool yDown;

		internal BoneData data;
		internal Skeleton skeleton;
		internal Bone parent;
		internal ExposedList<Bone> children = new ExposedList<Bone>();
		// internal float x, y, rotation, scaleX, scaleY, shearX, shearY;
		// internal float ax, ay, arotation, ascaleX, ascaleY, ashearX, ashearY;
		//
		// internal float a, b, worldX;
		// internal float c, d, worldY;
		//
		// internal bool sorted, active;

		public IntPtr boneHandle;
		
		
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_get_world_x_unity(IntPtr boneHandle);
  
        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_get_world_y_unity(IntPtr boneHandle);
  
        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_get_a_unity(IntPtr boneHandle);
        
        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_get_b_unity(IntPtr boneHandle);
  
        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_get_c_unity(IntPtr boneHandle);
        
        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_get_d_unity(IntPtr boneHandle);
        
        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_set_a_unity(IntPtr boneHandle,float newValue);
        
        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_set_b_unity(IntPtr boneHandle,float newValue);
  
        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_set_c_unity(IntPtr boneHandle,float newValue);
        
        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_set_d_unity(IntPtr boneHandle,float newValue);
  
        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_get_world_rotation_x_unity(IntPtr boneHandle);
  
        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_get_world_rotation_y_unity(IntPtr boneHandle);
  
        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_get_scale_x_unity(IntPtr boneHandle);
  
        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_get_scale_y_unity(IntPtr boneHandle);
  
        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_get_world_scale_x_unity(IntPtr boneHandle);
  
        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_get_world_scale_y_unity(IntPtr boneHandle);

        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_get_x_unity(IntPtr boneHandle);
        
        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern float spine_bone_set_x_unity(IntPtr boneHandle,float newX);
        

		public BoneData Data { get { return data; } }
		public Skeleton Skeleton { get { return skeleton; } }
		public Bone Parent { get { return parent; } }
		public ExposedList<Bone> Children { get { return children; } }
		/// <summary>Returns false when the bone has not been computed because <see cref="BoneData.SkinRequired"/> is true and the
		/// <see cref="Skeleton.Skin">active skin</see> does not <see cref="Skin.Bones">contain</see> this bone.</summary>
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern bool spine_bone_get_active_unity(IntPtr boneHandle);
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void  spine_bone_set_active_unity(IntPtr boneHandle,bool value);
		public bool Active { get { return  spine_bone_get_active_unity(boneHandle); } set{spine_bone_set_active_unity(boneHandle,value);}}
		
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern bool spine_bone_get_sorted_unity(IntPtr boneHandle);
		
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void  spine_bone_set_sorted_unity(IntPtr boneHandle,bool value);
		
		public bool Sorted { get { return spine_bone_get_sorted_unity(boneHandle); }
			set { spine_bone_set_sorted_unity(boneHandle, value); }
		}
		
		/// <summary>The local X translation.</summary>
		public float X { get { return spine_bone_get_x_unity(boneHandle); }
			set { spine_bone_set_x_unity(boneHandle, value); }
		}
		/// <summary>The local Y translation.</summary>
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bone_get_y_unity(IntPtr boneHandle);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bone_set_y_unity(IntPtr boneHandle, float newY);

		public float Y
		{
			get { return spine_bone_get_y_unity(boneHandle); }
			set { spine_bone_set_y_unity(boneHandle, value); }
		}
		/// <summary>The local rotation.</summary>
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bone_get_rotation_unity(IntPtr boneHandle);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bone_set_rotation_unity(IntPtr boneHandle, float newRotation);

		public float Rotation
		{
			get { return spine_bone_get_rotation_unity(boneHandle); }
			set { spine_bone_set_rotation_unity(boneHandle, value); }
		}

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bone_set_scale_x_unity(IntPtr boneHandle,float newX);
		/// <summary>The local scaleX.</summary>
		public float ScaleX { get { return spine_bone_get_scale_x_unity(boneHandle); } set { spine_bone_set_scale_x_unity(boneHandle,value); } }

		/// <summary>The local scaleY.</summary>
		 [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bone_set_scale_y_unity(IntPtr boneHandle,float newX);
		public float ScaleY { get { return spine_bone_get_scale_y_unity(boneHandle); } set { spine_bone_set_scale_y_unity(boneHandle,value); } }

		/// <summary>The local shearX.</summary>
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bone_get_shear_x_unity(IntPtr boneHandle);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bone_set_shear_x_unity(IntPtr boneHandle, float newShearX);

		public float ShearX
		{
			get { return spine_bone_get_shear_x_unity(boneHandle); }
			set { spine_bone_set_shear_x_unity(boneHandle, value); }
		}

		/// <summary>The local shearY.</summary>
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bone_get_shear_y_unity(IntPtr boneHandle);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bone_set_shear_y_unity(IntPtr boneHandle, float newShearY);

		public float ShearY
		{
			get { return spine_bone_get_shear_y_unity(boneHandle); }
			set { spine_bone_set_shear_y_unity(boneHandle, value); }
		}

		/// <summary>The rotation, as calculated by any constraints.</summary>
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bone_get_appliedRotation_unity(IntPtr boneHandle);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bone_set_appliedRotation_unity(IntPtr boneHandle, float newAppliedRotation);

		public float AppliedRotation
		{
			get { return spine_bone_get_appliedRotation_unity(boneHandle); }
			set { spine_bone_set_appliedRotation_unity(boneHandle, value); }
		}

		/// <summary>The applied local x translation.</summary>
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bone_get_ax_unity(IntPtr boneHandle);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bone_set_ax_unity(IntPtr boneHandle, float newAx);

		public float AX
		{
			get { return spine_bone_get_ax_unity(boneHandle); }
			set { spine_bone_set_ax_unity(boneHandle, value); }
		}

		/// <summary>The applied local y translation.</summary>
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bone_get_ay_unity(IntPtr boneHandle);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bone_set_ay_unity(IntPtr boneHandle, float newAY);

		public float AY
		{
			get { return spine_bone_get_ay_unity(boneHandle); }
			set { spine_bone_set_ay_unity(boneHandle, value); }
		}

		/// <summary>The applied local scaleX.</summary>
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bone_get_ascaleX_unity(IntPtr boneHandle);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bone_set_ascaleX_unity(IntPtr boneHandle, float newAScaleX);

		public float AScaleX
		{
			get { return spine_bone_get_ascaleX_unity(boneHandle); }
			set { spine_bone_set_ascaleX_unity(boneHandle, value); }
		}

		/// <summary>The applied local scaleY.</summary>
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bone_get_ascaleY_unity(IntPtr boneHandle);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bone_set_ascaleY_unity(IntPtr boneHandle, float newAScaleY);

		public float AScaleY
		{
			get { return spine_bone_get_ascaleY_unity(boneHandle); }
			set { spine_bone_set_ascaleY_unity(boneHandle, value); }
		}


		/// <summary>The applied local shearX.</summary>
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bone_get_ashearX_unity(IntPtr boneHandle);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bone_set_ashearX_unity(IntPtr boneHandle, float newAShearX);

		public float AShearX
		{
			get { return spine_bone_get_ashearX_unity(boneHandle); }
			set { spine_bone_set_ashearX_unity(boneHandle, value); }
		}


		/// <summary>The applied local shearY.</summary>
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bone_get_ashearY_unity(IntPtr boneHandle);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bone_set_ashearY_unity(IntPtr boneHandle, float newAShearY);

		public float AShearY
		{
			get { return spine_bone_get_ashearY_unity(boneHandle); }
			set { spine_bone_set_ashearY_unity(boneHandle, value); }
		}

		/// <summary>Part of the world transform matrix for the X axis. If changed, <see cref="UpdateAppliedTransform()"/> should be called.</summary>
		public float A { 
			get { return spine_bone_get_a_unity(boneHandle); } 
			set { spine_bone_set_a_unity(boneHandle,value); } 
		}
		/// <summary>Part of the world transform matrix for the Y axis. If changed, <see cref="UpdateAppliedTransform()"/> should be called.</summary>
		public float B { get { return spine_bone_get_b_unity(boneHandle); } set { spine_bone_set_b_unity(boneHandle,value); } }
		/// <summary>Part of the world transform matrix for the X axis. If changed, <see cref="UpdateAppliedTransform()"/> should be called.</summary>
		public float C { get { return spine_bone_get_c_unity(boneHandle); } set { spine_bone_set_c_unity(boneHandle,value); } }
		/// <summary>Part of the world transform matrix for the Y axis. If changed, <see cref="UpdateAppliedTransform()"/> should be called.</summary>
		public float D { get { return spine_bone_get_d_unity(boneHandle); } set { spine_bone_set_d_unity(boneHandle,value); } }

		/// <summary>The world X position. If changed, <see cref="UpdateAppliedTransform()"/> should be called.</summary>
	
		 [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bone_set_worldx_unity(IntPtr boneHandle,float newX);
		public float WorldX { get { return spine_bone_get_world_x_unity(boneHandle); } set { spine_bone_set_worldx_unity(boneHandle,value); } }
		/// <summary>The world Y position. If changed, <see cref="UpdateAppliedTransform()"/> should be called.</summary>
		
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bone_set_worldy_unity(IntPtr boneHandle,float newX);
		public float WorldY { get { return spine_bone_get_world_y_unity(boneHandle); } set { spine_bone_set_worldy_unity(boneHandle,value); } }
		public float WorldRotationX { get { return spine_bone_get_world_rotation_x_unity(boneHandle); } }
		public float WorldRotationY { get { return spine_bone_get_world_rotation_y_unity(boneHandle); } }

		/// <summary>Returns the magnitide (always positive) of the world scale X.</summary>
		public float WorldScaleX { get { return spine_bone_get_world_scale_x_unity(boneHandle); } }
		/// <summary>Returns the magnitide (always positive) of the world scale Y.</summary>
		public float WorldScaleY { get { return spine_bone_get_world_scale_y_unity(boneHandle); } }

		public Bone (BoneData data, Skeleton skeleton, Bone parent) {
			if (data == null) throw new ArgumentNullException("data", "data cannot be null.");
			if (skeleton == null) throw new ArgumentNullException("skeleton", "skeleton cannot be null.");
			this.data = data;
			this.skeleton = skeleton;
			this.parent = parent;
			SetToSetupPose();
		}

		/// <summary>Copy constructor. Does not copy the <see cref="Children"/> bones.</summary>
		/// <param name="parent">May be null.</param>
		public Bone (Bone bone, Skeleton skeleton, Bone parent) {
			if (bone == null) throw new ArgumentNullException("bone", "bone cannot be null.");
			if (skeleton == null) throw new ArgumentNullException("skeleton", "skeleton cannot be null.");
			this.skeleton = skeleton;
			this.parent = parent;
			data = bone.data;
			X = bone.X;
			Y = bone.Y;
			Rotation = bone.Rotation;
			ScaleX = bone.ScaleX;
			ScaleY = bone.ScaleY;
			ShearX = bone.ShearX;
			ShearY = bone.ShearY;
		}

		/// <summary>Computes the world transform using the parent bone and this bone's local applied transform.</summary>
		public void Update () {
			UpdateWorldTransform(AX, AY, AppliedRotation, AScaleX, AScaleY, AShearX, AShearY);
		}

		/// <summary>Computes the world transform using the parent bone and this bone's local transform.</summary>
		public void UpdateWorldTransform () {
			UpdateWorldTransform(X, Y, Rotation, ScaleX, ScaleY, ShearX, ShearY);
		}

		/// <summary>Computes the world transform using the parent bone and the specified local transform. The applied transform is set to the
		/// specified local transform. Child bones are not updated.
		/// <para>
		/// See <a href="http://esotericsoftware.com/spine-runtime-skeletons#World-transforms">World transforms</a> in the Spine
		/// Runtimes Guide.</para></summary>
		public void UpdateWorldTransform (float x, float y, float rotation, float scaleX, float scaleY, float shearX, float shearY) {
			AX = x;
			AY = y;
			AppliedRotation = rotation;
			AScaleX = scaleX;
			AScaleY = scaleY;
			AShearX = shearX;
			AShearY = shearY;

			Bone parent = this.parent;
			if (parent == null) { // Root bone.
				float rotationY = rotation + 90 + shearY, sx = skeleton.ScaleX, sy = skeleton.ScaleY;
				A = MathUtils.CosDeg(rotation + shearX) * scaleX * sx;
				B = MathUtils.CosDeg(rotationY) * scaleY * sx;
				C = MathUtils.SinDeg(rotation + shearX) * scaleX * sy;
				D = MathUtils.SinDeg(rotationY) * scaleY * sy;
				WorldX = x * sx + skeleton.x;
				WorldY = y * sy + skeleton.y;
				return;
			}

			float pa = parent.A, pb = parent.B, pc = parent.C, pd = parent.D;
			WorldX = pa * x + pb * y + parent.WorldX;
			WorldY = pc * x + pd * y + parent.WorldY;

			switch (data.TransformMode) {
			case TransformMode.Normal: {
				float rotationY = rotation + 90 + shearY;
				float la = MathUtils.CosDeg(rotation + shearX) * scaleX;
				float lb = MathUtils.CosDeg(rotationY) * scaleY;
				float lc = MathUtils.SinDeg(rotation + shearX) * scaleX;
				float ld = MathUtils.SinDeg(rotationY) * scaleY;
				A = pa * la + pb * lc;
				B = pa * lb + pb * ld;
				C = pc * la + pd * lc;
				D = pc * lb + pd * ld;
				return;
			}
			case TransformMode.OnlyTranslation: {
				float rotationY = rotation + 90 + shearY;
				A = MathUtils.CosDeg(rotation + shearX) * scaleX;
				B = MathUtils.CosDeg(rotationY) * scaleY;
				C = MathUtils.SinDeg(rotation + shearX) * scaleX;
				D = MathUtils.SinDeg(rotationY) * scaleY;
				break;
			}
			case TransformMode.NoRotationOrReflection: {
				float s = pa * pa + pc * pc, prx;
				if (s > 0.0001f) {
					s = Math.Abs(pa * pd - pb * pc) / s;
					pa /= skeleton.ScaleX;
					pc /= skeleton.ScaleY;
					pb = pc * s;
					pd = pa * s;
					prx = MathUtils.Atan2(pc, pa) * MathUtils.RadDeg;
				} else {
					pa = 0;
					pc = 0;
					prx = 90 - MathUtils.Atan2(pd, pb) * MathUtils.RadDeg;
				}
				float rx = rotation + shearX - prx;
				float ry = rotation + shearY - prx + 90;
				float la = MathUtils.CosDeg(rx) * scaleX;
				float lb = MathUtils.CosDeg(ry) * scaleY;
				float lc = MathUtils.SinDeg(rx) * scaleX;
				float ld = MathUtils.SinDeg(ry) * scaleY;
				A = pa * la - pb * lc;
				B = pa * lb - pb * ld;
				C = pc * la + pd * lc;
				D = pc * lb + pd * ld;
				break;
			}
			case TransformMode.NoScale:
			case TransformMode.NoScaleOrReflection: {
				float cos = MathUtils.CosDeg(rotation), sin = MathUtils.SinDeg(rotation);
				float za = (pa * cos + pb * sin) / skeleton.ScaleX;
				float zc = (pc * cos + pd * sin) / skeleton.ScaleY;
				float s = (float)Math.Sqrt(za * za + zc * zc);
				if (s > 0.00001f) s = 1 / s;
				za *= s;
				zc *= s;
				s = (float)Math.Sqrt(za * za + zc * zc);
				if (data.TransformMode == TransformMode.NoScale
					&& (pa * pd - pb * pc < 0) != (skeleton.ScaleX < 0 != skeleton.ScaleY < 0)) s = -s;

				float r = MathUtils.PI / 2 + MathUtils.Atan2(zc, za);
				float zb = MathUtils.Cos(r) * s;
				float zd = MathUtils.Sin(r) * s;
				float la = MathUtils.CosDeg(shearX) * scaleX;
				float lb = MathUtils.CosDeg(90 + shearY) * scaleY;
				float lc = MathUtils.SinDeg(shearX) * scaleX;
				float ld = MathUtils.SinDeg(90 + shearY) * scaleY;
				A = za * la + zb * lc;
				B = za * lb + zb * ld;
				C = zc * la + zd * lc;
				D = zc * lb + zd * ld;
				break;
			}
			}

			A *= skeleton.ScaleX;
			B *= skeleton.ScaleX;
			C *= skeleton.ScaleY;
			D *= skeleton.ScaleY;
		}

		public void SetToSetupPose () {
			BoneData data = this.data;
			X = data.X;
			Y = data.Y;
			Rotation = data.Rotation;
			ScaleX = data.ScaleX;
			ScaleY = data.ScaleY;
			ShearX = data.ShearX;
			ShearY = data.ShearY;
		}

		/// <summary>
		/// Computes the applied transform values from the world transform.
		/// <para>
		/// If the world transform is modified (by a constraint, <see cref="RotateWorld(float)"/>, etc) then this method should be called so
		/// the applied transform matches the world transform. The applied transform may be needed by other code (eg to apply another
		/// constraint).
		/// </para><para>
		///  Some information is ambiguous in the world transform, such as -1,-1 scale versus 180 rotation. The applied transform after
		/// calling this method is equivalent to the local transform used to compute the world transform, but may not be identical.
		/// </para></summary>
		public void UpdateAppliedTransform () {
			Bone parent = this.parent;
			if (parent == null) {
				AX = WorldX - skeleton.x;
				AY = WorldY - skeleton.y;
				AppliedRotation = MathUtils.Atan2(C, A) * MathUtils.RadDeg;
				AScaleX = (float)Math.Sqrt(A * A + C * C);
				AScaleY = (float)Math.Sqrt(B * B + D * D);
				AShearX = 0;
				AShearY = MathUtils.Atan2(A * B + C * D, A * D - B * C) * MathUtils.RadDeg;
				return;
			}
			float pa = parent.A, pb = parent.B, pc = parent.C, pd = parent.D;
			float pid = 1 / (pa * pd - pb * pc);
			float dx = WorldX - parent.WorldX, dy = WorldY - parent.WorldY;
			AX = (dx * pd * pid - dy * pb * pid);
			AY = (dy * pa * pid - dx * pc * pid);
			float ia = pid * pd;
			float id = pid * pa;
			float ib = pid * pb;
			float ic = pid * pc;
			float ra = ia * A - ib * C;
			float rb = ia * B - ib * D;
			float rc = id * C - ic * A;
			float rd = id * D - ic * B;
			AShearX = 0;
			AScaleX = (float)Math.Sqrt(ra * ra + rc * rc);
			if (AScaleX > 0.0001f) {
				float det = ra * rd - rb * rc;
				AScaleY = det / AScaleX;
				AShearY = MathUtils.Atan2(ra * rb + rc * rd, det) * MathUtils.RadDeg;
				AppliedRotation = MathUtils.Atan2(rc, ra) * MathUtils.RadDeg;
			} else {
				AScaleX = 0;
				AScaleY = (float)Math.Sqrt(rb * rb + rd * rd);
				AShearY = 0;
				AppliedRotation = 90 - MathUtils.Atan2(rd, rb) * MathUtils.RadDeg;
			}
		}

		public void WorldToLocal (float worldX, float worldY, out float localX, out float localY) {
			float a = this.A, b = this.B, c = this.C, d = this.D;
			float det = a * d - b * c;
			float x = worldX - this.WorldX, y = worldY - this.WorldY;
			localX = (x * d - y * b) / det;
			localY = (y * a - x * c) / det;
		}

		public void LocalToWorld (float localX, float localY, out float worldX, out float worldY) {
			worldX = localX * A + localY * B + this.WorldX;
			worldY = localX * C + localY * D + this.WorldY;
		}

		public float WorldToLocalRotationX {
			get {
				Bone parent = this.parent;
				if (parent == null) return AppliedRotation;
				float pa = parent.A, pb = parent.B, pc = parent.C, pd = parent.D, a = this.A, c = this.C;
				return MathUtils.Atan2(pa * c - pc * a, pd * a - pb * c) * MathUtils.RadDeg;
			}
		}

		public float WorldToLocalRotationY {
			get {
				Bone parent = this.parent;
				if (parent == null) return AppliedRotation;
				float pa = parent.A, pb = parent.B, pc = parent.C, pd = parent.D, b = this.B, d = this.D;
				return MathUtils.Atan2(pa * d - pc * b, pd * b - pb * d) * MathUtils.RadDeg;
			}
		}

		public float WorldToLocalRotation (float worldRotation) {
			float sin = MathUtils.SinDeg(worldRotation), cos = MathUtils.CosDeg(worldRotation);
			return MathUtils.Atan2(A * sin - C * cos, D * cos - B * sin) * MathUtils.RadDeg + Rotation - ShearX;
		}

		public float LocalToWorldRotation (float localRotation) {
			localRotation -= Rotation - ShearX;
			float sin = MathUtils.SinDeg(localRotation), cos = MathUtils.CosDeg(localRotation);
			return MathUtils.Atan2(cos * C + sin * D, cos * A + sin * B) * MathUtils.RadDeg;
		}

		/// <summary>
		/// Rotates the world transform the specified amount.
		/// <para>
		/// After changes are made to the world transform, <see cref="UpdateAppliedTransform()"/> should be called and <see cref="Update()"/> will
		/// need to be called on any child bones, recursively.
		/// </para></summary>
		public void RotateWorld (float degrees) {
			float a = A, b = this.B, c = this.C, d = this.D;
			float cos = MathUtils.CosDeg(degrees), sin = MathUtils.SinDeg(degrees);
			A = cos * a - sin * c;
			this.B = cos * b - sin * d;
			this.C = sin * a + cos * c;
			this.D = sin * b + cos * d;
		}

		override public string ToString () {
			return data.Name;
		}
	}
}


