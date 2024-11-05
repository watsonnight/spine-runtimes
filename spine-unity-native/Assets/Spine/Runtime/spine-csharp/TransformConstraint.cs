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

namespace Spine {
	/// <summary>
	/// <para>
	/// Stores the current pose for a transform constraint. A transform constraint adjusts the world transform of the constrained
	/// bones to match that of the target bone.</para>
	/// <para>
	/// See <a href="http://esotericsoftware.com/spine-transform-constraints">Transform constraints</a> in the Spine User Guide.</para>
	/// </summary>
	public class TransformConstraint : IUpdatable {
		internal readonly TransformConstraintData data;
		internal readonly ExposedList<Bone> bones;
		internal Bone target;
		internal float mixRotate, mixX, mixY, mixScaleX, mixScaleY, mixShearY;

		internal bool active;

		public TransformConstraint (TransformConstraintData data, Skeleton skeleton) {
			if (data == null) throw new ArgumentNullException("data", "data cannot be null.");
			if (skeleton == null) throw new ArgumentNullException("skeleton", "skeleton cannot be null.");
			this.data = data;
			mixRotate = data.mixRotate;
			mixX = data.mixX;
			mixY = data.mixY;
			mixScaleX = data.mixScaleX;
			mixScaleY = data.mixScaleY;
			mixShearY = data.mixShearY;
			bones = new ExposedList<Bone>();
			foreach (BoneData boneData in data.bones)
				bones.Add(skeleton.bones.Items[boneData.Index]);

			target = skeleton.bones.Items[data.target.Index];
		}

		/// <summary>Copy constructor.</summary>
		public TransformConstraint (TransformConstraint constraint, Skeleton skeleton) {
			if (constraint == null) throw new ArgumentNullException("constraint cannot be null.");
			if (skeleton == null) throw new ArgumentNullException("skeleton cannot be null.");
			data = constraint.data;
			bones = new ExposedList<Bone>(constraint.Bones.Count);
			foreach (Bone bone in constraint.Bones)
				bones.Add(skeleton.Bones.Items[bone.data.Index]);
			target = skeleton.Bones.Items[constraint.target.data.Index];
			mixRotate = constraint.mixRotate;
			mixX = constraint.mixX;
			mixY = constraint.mixY;
			mixScaleX = constraint.mixScaleX;
			mixScaleY = constraint.mixScaleY;
			mixShearY = constraint.mixShearY;
		}

		public void Update () {
			if (mixRotate == 0 && mixX == 0 && mixY == 0 && mixScaleX == 0 && mixScaleY == 0 && mixShearY == 0) return;
			if (data.local) {
				if (data.relative)
					ApplyRelativeLocal();
				else
					ApplyAbsoluteLocal();
			} else {
				if (data.relative)
					ApplyRelativeWorld();
				else
					ApplyAbsoluteWorld();
			}
		}

		void ApplyAbsoluteWorld () {
			float mixRotate = this.mixRotate, mixX = this.mixX, mixY = this.mixY, mixScaleX = this.mixScaleX,
			mixScaleY = this.mixScaleY, mixShearY = this.mixShearY;
			bool translate = mixX != 0 || mixY != 0;

			Bone target = this.target;
			float ta = target.A, tb = target.B, tc = target.C, td = target.D;
			float degRadReflect = ta * td - tb * tc > 0 ? MathUtils.DegRad : -MathUtils.DegRad;
			float offsetRotation = data.offsetRotation * degRadReflect, offsetShearY = data.offsetShearY * degRadReflect;

			Bone[] bones = this.bones.Items;
			for (int i = 0, n = this.bones.Count; i < n; i++) {
				Bone bone = bones[i];

				if (mixRotate != 0) {
					float a = bone.A, b = bone.B, c = bone.C, d = bone.D;
					float r = MathUtils.Atan2(tc, ta) - MathUtils.Atan2(c, a) + offsetRotation;
					if (r > MathUtils.PI)
						r -= MathUtils.PI2;
					else if (r < -MathUtils.PI) //
						r += MathUtils.PI2;
					r *= mixRotate;
					float cos = MathUtils.Cos(r), sin = MathUtils.Sin(r);
					bone.A = cos * a - sin * c;
					bone.B = cos * b - sin * d;
					bone.C = sin * a + cos * c;
					bone.D = sin * b + cos * d;
				}

				if (translate) {
					float tx, ty; //Vector2 temp = this.temp;
					target.LocalToWorld(data.offsetX, data.offsetY, out tx, out ty); //target.localToWorld(temp.set(data.offsetX, data.offsetY));
					bone.WorldX += (tx - bone.WorldX) * mixX;
					bone.WorldY += (ty - bone.WorldY) * mixY;
				}

				if (mixScaleX != 0) {
					float s = (float)Math.Sqrt(bone.A * bone.A + bone.C * bone.C);
					if (s != 0) s = (s + ((float)Math.Sqrt(ta * ta + tc * tc) - s + data.offsetScaleX) * mixScaleX) / s;
					bone.A *= s;
					bone.C *= s;
				}
				if (mixScaleY != 0) {
					float s = (float)Math.Sqrt(bone.B * bone.B + bone.D * bone.D);
					if (s != 0) s = (s + ((float)Math.Sqrt(tb * tb + td * td) - s + data.offsetScaleY) * mixScaleY) / s;
					bone.B *= s;
					bone.D *= s;
				}

				if (mixShearY > 0) {
					float b = bone.B, d = bone.D;
					float by = MathUtils.Atan2(d, b);
					float r = MathUtils.Atan2(td, tb) - MathUtils.Atan2(tc, ta) - (by - MathUtils.Atan2(bone.C, bone.A));
					if (r > MathUtils.PI)
						r -= MathUtils.PI2;
					else if (r < -MathUtils.PI) //
						r += MathUtils.PI2;
					r = by + (r + offsetShearY) * mixShearY;
					float s = (float)Math.Sqrt(b * b + d * d);
					bone.B = MathUtils.Cos(r) * s;
					bone.D = MathUtils.Sin(r) * s;
				}

				bone.UpdateAppliedTransform();
			}
		}

		void ApplyRelativeWorld () {
			float mixRotate = this.mixRotate, mixX = this.mixX, mixY = this.mixY, mixScaleX = this.mixScaleX,
			mixScaleY = this.mixScaleY, mixShearY = this.mixShearY;
			bool translate = mixX != 0 || mixY != 0;

			Bone target = this.target;
			float ta = target.A, tb = target.B, tc = target.C, td = target.D;
			float degRadReflect = ta * td - tb * tc > 0 ? MathUtils.DegRad : -MathUtils.DegRad;
			float offsetRotation = data.offsetRotation * degRadReflect, offsetShearY = data.offsetShearY * degRadReflect;

			Bone[] bones = this.bones.Items;
			for (int i = 0, n = this.bones.Count; i < n; i++) {
				Bone bone = bones[i];

				if (mixRotate != 0) {
					float a = bone.A, b = bone.B, c = bone.C, d = bone.D;
					float r = MathUtils.Atan2(tc, ta) + offsetRotation;
					if (r > MathUtils.PI)
						r -= MathUtils.PI2;
					else if (r < -MathUtils.PI) //
						r += MathUtils.PI2;
					r *= mixRotate;
					float cos = MathUtils.Cos(r), sin = MathUtils.Sin(r);
					bone.A = cos * a - sin * c;
					bone.B = cos * b - sin * d;
					bone.C = sin * a + cos * c;
					bone.D = sin * b + cos * d;
				}

				if (translate) {
					float tx, ty; //Vector2 temp = this.temp;
					target.LocalToWorld(data.offsetX, data.offsetY, out tx, out ty); //target.localToWorld(temp.set(data.offsetX, data.offsetY));
					bone.WorldX += tx * mixX;
					bone.WorldY += ty * mixY;
				}

				if (mixScaleX != 0) {
					float s = ((float)Math.Sqrt(ta * ta + tc * tc) - 1 + data.offsetScaleX) * mixScaleX + 1;
					bone.A *= s;
					bone.C *= s;
				}
				if (mixScaleY != 0) {
					float s = ((float)Math.Sqrt(tb * tb + td * td) - 1 + data.offsetScaleY) * mixScaleY + 1;
					bone.B *= s;
					bone.D *= s;
				}

				if (mixShearY > 0) {
					float r = MathUtils.Atan2(td, tb) - MathUtils.Atan2(tc, ta);
					if (r > MathUtils.PI)
						r -= MathUtils.PI2;
					else if (r < -MathUtils.PI) //
						r += MathUtils.PI2;
					float b = bone.B, d = bone.D;
					r = MathUtils.Atan2(d, b) + (r - MathUtils.PI / 2 + offsetShearY) * mixShearY;
					float s = (float)Math.Sqrt(b * b + d * d);
					bone.B = MathUtils.Cos(r) * s;
					bone.D = MathUtils.Sin(r) * s;
				}

				bone.UpdateAppliedTransform();
			}
		}

		void ApplyAbsoluteLocal () {
			float mixRotate = this.mixRotate, mixX = this.mixX, mixY = this.mixY, mixScaleX = this.mixScaleX,
			mixScaleY = this.mixScaleY, mixShearY = this.mixShearY;

			Bone target = this.target;

			Bone[] bones = this.bones.Items;
			for (int i = 0, n = this.bones.Count; i < n; i++) {
				Bone bone = bones[i];

				float rotation = bone.AppliedRotation;
				if (mixRotate != 0) {
					float r = target.AppliedRotation - rotation + data.offsetRotation;
					r -= (16384 - (int)(16384.499999999996 - r / 360)) * 360;
					rotation += r * mixRotate;
				}

				float x = bone.AX, y = bone.AY;
				x += (target.AX - x + data.offsetX) * mixX;
				y += (target.AY - y + data.offsetY) * mixY;

				float scaleX = bone.AScaleX, scaleY = bone.AScaleY;
				if (mixScaleX != 0 && scaleX != 0)
					scaleX = (scaleX + (target.AScaleX - scaleX + data.offsetScaleX) * mixScaleX) / scaleX;
				if (mixScaleY != 0 && scaleY != 0)
					scaleY = (scaleY + (target.AScaleY - scaleY + data.offsetScaleY) * mixScaleY) / scaleY;

				float shearY = bone.AShearY;
				if (mixShearY != 0) {
					float r = target.AShearY - shearY + data.offsetShearY;
					r -= (16384 - (int)(16384.499999999996 - r / 360)) * 360;
					shearY += r * mixShearY;
				}

				bone.UpdateWorldTransform(x, y, rotation, scaleX, scaleY, bone.AShearX, shearY);
			}
		}

		void ApplyRelativeLocal () {
			float mixRotate = this.mixRotate, mixX = this.mixX, mixY = this.mixY, mixScaleX = this.mixScaleX,
			mixScaleY = this.mixScaleY, mixShearY = this.mixShearY;

			Bone target = this.target;

			Bone[] bones = this.bones.Items;
			for (int i = 0, n = this.bones.Count; i < n; i++) {
				Bone bone = bones[i];

				float rotation = bone.AppliedRotation + (target.AppliedRotation + data.offsetRotation) * mixRotate;
				float x = bone.AX + (target.AX + data.offsetX) * mixX;
				float y = bone.AY + (target.AY + data.offsetY) * mixY;
				float scaleX = bone.AScaleX * (((target.AScaleX - 1 + data.offsetScaleX) * mixScaleX) + 1);
				float scaleY = bone.AScaleY * (((target.AScaleY - 1 + data.offsetScaleY) * mixScaleY) + 1);
				float shearY = bone.AShearY + (target.AShearY + data.offsetShearY) * mixShearY;

				bone.UpdateWorldTransform(x, y, rotation, scaleX, scaleY, bone.AShearX, shearY);
			}
		}

		/// <summary>The bones that will be modified by this transform constraint.</summary>
		public ExposedList<Bone> Bones { get { return bones; } }
		/// <summary>The target bone whose world transform will be copied to the constrained bones.</summary>
		public Bone Target { get { return target; } set { target = value; } }
		/// <summary>A percentage (0-1) that controls the mix between the constrained and unconstrained rotation.</summary>
		public float MixRotate { get { return mixRotate; } set { mixRotate = value; } }
		/// <summary>A percentage (0-1) that controls the mix between the constrained and unconstrained translation X.</summary>
		public float MixX { get { return mixX; } set { mixX = value; } }
		/// <summary>A percentage (0-1) that controls the mix between the constrained and unconstrained translation Y.</summary>
		public float MixY { get { return mixY; } set { mixY = value; } }
		/// <summary>A percentage (0-1) that controls the mix between the constrained and unconstrained scale X.</summary>
		public float MixScaleX { get { return mixScaleX; } set { mixScaleX = value; } }
		/// <summary>A percentage (0-1) that controls the mix between the constrained and unconstrained scale Y.</summary>
		public float MixScaleY { get { return mixScaleY; } set { mixScaleY = value; } }
		/// <summary>A percentage (0-1) that controls the mix between the constrained and unconstrained shear Y.</summary>
		public float MixShearY { get { return mixShearY; } set { mixShearY = value; } }
		public bool Active { get { return active; } }
		/// <summary>The transform constraint's setup pose data.</summary>
		public TransformConstraintData Data { get { return data; } }

		override public string ToString () {
			return data.name;
		}
	}
}
