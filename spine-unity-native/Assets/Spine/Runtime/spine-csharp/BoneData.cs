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
	public class BoneData {
		// internal int index;
		// internal string name;
		internal BoneData parent;
		// internal float length;
		// internal float x, y, rotation, scaleX = 1, scaleY = 1, shearX, shearY;
		// internal TransformMode transformMode = TransformMode.Normal;
		// internal bool skinRequired;
		public  IntPtr boneDataHandle;

		/// <summary>The index of the bone in Skeleton.Bones</summary>
		//public int Index { get { return index; } }
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern int spine_bonedata_get_index_unity(IntPtr boneDataHandle);
		public int Index{ get { return spine_bonedata_get_index_unity(boneDataHandle); } }

		/// <summary>The name of the bone, which is unique across all bones in the skeleton.</summary>
		//public string Name { get { return name; } }
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern IntPtr spine_bonedata_get_data_name_unity(IntPtr boneDataHandle);

		public string Name
		{
			get
			{
				IntPtr namePtr = spine_bonedata_get_data_name_unity(boneDataHandle);
				string name = Marshal.PtrToStringUTF8(namePtr);
				return name;
			}
		}

		/// <summary>May be null.</summary>
		public BoneData Parent { get { return parent; } }

		// public float Length { get { return length; } set { length = value; } }
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bonedata_set_length_unity(IntPtr boneDataHandle, float newLength);
		
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		 static extern float spine_bonedata_get_length_unity(IntPtr boneDataHandle);
		public float Length
		{
			get { return spine_bonedata_get_length_unity(boneDataHandle); }
			set { spine_bonedata_set_length_unity(boneDataHandle, value); }
		}

		/// <summary>Local X translation.</summary>
		// public float X { get { return x; } set { x = value; } }
		
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bonedata_set_x_unity(IntPtr boneDataHandle, float newX);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bonedata_get_x_unity(IntPtr boneDataHandle);

		public float X
		{
			get { return spine_bonedata_get_x_unity(boneDataHandle); }
			set { spine_bonedata_set_x_unity(boneDataHandle, value); }
		}

		/// <summary>Local Y translation.</summary>
		// public float Y { get { return y; } set { y = value; } }
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bonedata_set_y_unity(IntPtr boneDataHandle, float newY);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bonedata_get_y_unity(IntPtr boneDataHandle);

		public float Y
		{
			get { return spine_bonedata_get_y_unity(boneDataHandle); }
			set { spine_bonedata_set_y_unity(boneDataHandle, value); }
		}

		/// <summary>Local rotation.</summary>
		//public float Rotation { get { return rotation; } set { rotation = value; } }
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bonedata_set_rotation_unity(IntPtr boneDataHandle, float newRotation);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bonedata_get_rotation_unity(IntPtr boneDataHandle);

		public float Rotation
		{
			get { return spine_bonedata_get_rotation_unity(boneDataHandle); }
			set { spine_bonedata_set_rotation_unity(boneDataHandle, value); }
		}

		/// <summary>Local scaleX.</summary>
		//public float ScaleX { get { return scaleX; } set { scaleX = value; } }
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bonedata_set_scaleX_unity(IntPtr boneDataHandle, float newScaleX);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bonedata_get_scaleX_unity(IntPtr boneDataHandle);

		public float ScaleX
		{
			get { return spine_bonedata_get_scaleX_unity(boneDataHandle); }
			set { spine_bonedata_set_scaleX_unity(boneDataHandle, value); }
		}

		/// <summary>Local scaleY.</summary>
		//public float ScaleY { get { return scaleY; } set { scaleY = value; } }
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bonedata_set_scaleY_unity(IntPtr boneDataHandle, float newScaleY);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bonedata_get_scaleY_unity(IntPtr boneDataHandle);

		public float ScaleY
		{
			get { return spine_bonedata_get_scaleY_unity(boneDataHandle); }
			set { spine_bonedata_set_scaleY_unity(boneDataHandle, value); }
		}

		/// <summary>Local shearX.</summary>
		//public float ShearX { get { return shearX; } set { shearX = value; } }
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bonedata_set_shearX_unity(IntPtr boneDataHandle, float newShearX);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bonedata_get_shearX_unity(IntPtr boneDataHandle);

		public float ShearX
		{
			get { return spine_bonedata_get_shearX_unity(boneDataHandle); }
			set { spine_bonedata_set_shearX_unity(boneDataHandle, value); }
		}

		/// <summary>Local shearY.</summary>
		//public float ShearY { get { return shearY; } set { shearY = value; } }
		
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bonedata_set_shearY_unity(IntPtr boneDataHandle, float newShearY);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern float spine_bonedata_get_shearY_unity(IntPtr boneDataHandle);

		public float ShearY
		{
			get { return spine_bonedata_get_shearY_unity(boneDataHandle); }
			set { spine_bonedata_set_shearY_unity(boneDataHandle, value); }
		}

		/// <summary>The transform mode for how parent world transforms affect this bone.</summary>
		//public TransformMode TransformMode { get { return transformMode; } set { transformMode = value; } }
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern int spine_bonedata_get_transformMode_unity(IntPtr boneDataHandle);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bonedata_set_transformMode_unity(IntPtr boneDataHandle, int transformMode);

		public TransformMode TransformMode
		{
			get {
				int value = spine_bonedata_get_transformMode_unity(boneDataHandle);
				return ConvertTransformMode(value);
			}
			set {
				int intValue = ConvertToCPlusPlusEnum(value);;
				spine_bonedata_set_transformMode_unity(boneDataHandle, intValue);
			}
		}
		
		private TransformMode ConvertTransformMode(int value)
		{
			switch (value)
			{
				case 0:
					return TransformMode.Normal;
				case 1:
					return TransformMode.OnlyTranslation;
				case 2:
					return TransformMode.NoRotationOrReflection;
				case 3:
					return TransformMode.NoScale;
				case 4:
					return TransformMode.NoScaleOrReflection;
				default:
					return TransformMode.Normal;
			}
		}
		
		private int ConvertToCPlusPlusEnum(TransformMode mode)
		{
			switch (mode)
			{
				case TransformMode.Normal:
					return 0;
				case TransformMode.OnlyTranslation:
					return 1;
				case TransformMode.NoRotationOrReflection:
					return 2;
				case TransformMode.NoScale:
					return 3;
				case TransformMode.NoScaleOrReflection:
					return 4;
				default:
					return 0;
			}
		}

		///<summary>When true, <see cref="Skeleton.UpdateWorldTransform()"/> only updates this bone if the <see cref="Skeleton.Skin"/> contains this
		/// bone.</summary>
		/// <seealso cref="Skin.Bones"/>
		//public bool SkinRequired { get { return skinRequired; } set { skinRequired = value; } }
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_bonedata_set_skinRequired_unity(IntPtr boneDataHandle, bool newSkinRequired);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern bool spine_bonedata_get_skinRequired_unity(IntPtr boneDataHandle);

		public bool SkinRequired
		{
			get { return spine_bonedata_get_skinRequired_unity(boneDataHandle); }
			set { spine_bonedata_set_skinRequired_unity(boneDataHandle, value); }
		}
		
		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern IntPtr spine_bonedata_create_unity(int index,string name);

		/// <param name="parent">May be null.</param>
		public BoneData (int index, string name, BoneData parent) {
			if (index < 0) throw new ArgumentException("index must be >= 0", "index");
			if (name == null) throw new ArgumentNullException("name", "name cannot be null.");
			// this.index = index;
			// this.name = name;
			boneDataHandle = spine_bonedata_create_unity(index, name);
			this.parent = parent;
		}
		
		public BoneData (BoneData parent) {
			this.parent = parent;
		}
		
		

		
		override public string ToString () {
			return Name;
		}
	}

	[Flags]
	public enum TransformMode {
		//0000 0 Flip Scale Rotation
		Normal = 0, // 0000
		OnlyTranslation = 7, // 0111
		NoRotationOrReflection = 1, // 0001
		NoScale = 2, // 0010
		NoScaleOrReflection = 6, // 0110
	}
}
