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

#if UNITY_2018_3 || UNITY_2019 || UNITY_2018_3_OR_NEWER
#define NEW_PREFAB_SYSTEM
#endif

#if UNITY_2018_2_OR_NEWER
#define HAS_CULL_TRANSPARENT_MESH
#endif

#define SPINE_OPTIONAL_ON_DEMAND_LOADING

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Unity.Collections;
using Unity.Collections.LowLevel.Unsafe;
using Unity.Mathematics;
using Unity.Profiling;
using UnityEngine;
using UnityEngine.UI;

namespace Spine.Unity {
#if NEW_PREFAB_SYSTEM
	[ExecuteAlways]
#else
	[ExecuteInEditMode]
#endif
	[RequireComponent(typeof(CanvasRenderer), typeof(RectTransform)), DisallowMultipleComponent]
	[AddComponentMenu("Spine/SkeletonGraphic (Unity UI Canvas)")]
	[HelpURL("http://esotericsoftware.com/spine-unity#SkeletonGraphic-Component")]
	public class SkeletonGraphic : MaskableGraphic, ISkeletonComponent, IAnimationStateComponent, ISkeletonAnimation, IHasSkeletonDataAsset {

		#region Inspector
		public SkeletonDataAsset skeletonDataAsset;
		public SkeletonDataAsset SkeletonDataAsset { get { return skeletonDataAsset; } }

		public Material additiveMaterial;
		public Material multiplyMaterial;
		public Material screenMaterial;

		[SpineSkin(dataField: "skeletonDataAsset", defaultAsEmptyString: true)]
		public string initialSkinName;
		public bool initialFlipX, initialFlipY;

		[SpineAnimation(dataField: "skeletonDataAsset")]
		public string startingAnimation;
		public bool startingLoop;
		public float timeScale = 1f;
		public bool freeze;
		protected float meshScale = 1f;
		public float MeshScale { get { return meshScale; } }

		public enum LayoutMode {
			None = 0,
			WidthControlsHeight,
			HeightControlsWidth,
			FitInParent,
			EnvelopeParent
		}
		public LayoutMode layoutScaleMode = LayoutMode.None;
		[SerializeField] protected Vector2 referenceSize = Vector2.one;
		[SerializeField] protected float referenceScale = 1f;
#if UNITY_EDITOR
		protected LayoutMode previousLayoutScaleMode = LayoutMode.None;
		[SerializeField] protected Vector2 rectTransformSize = Vector2.zero;
		[SerializeField] protected bool editReferenceRect = false;
		protected bool previousEditReferenceRect = false;

		public bool EditReferenceRect { get { return editReferenceRect; } set { editReferenceRect = value; } }
		public Vector2 RectTransformSize { get { return rectTransformSize; } }
#else
		protected const bool EditReferenceRect = false;
#endif
		/// <summary>Update mode to optionally limit updates to e.g. only apply animations but not update the mesh.</summary>
		public UpdateMode UpdateMode { get { return updateMode; } set { updateMode = value; } }
		protected UpdateMode updateMode = UpdateMode.FullUpdate;

		/// <summary>Update mode used when the MeshRenderer becomes invisible
		/// (when <c>OnBecameInvisible()</c> is called). Update mode is automatically
		/// reset to <c>UpdateMode.FullUpdate</c> when the mesh becomes visible again.</summary>
		public UpdateMode updateWhenInvisible = UpdateMode.FullUpdate;

		public bool allowMultipleCanvasRenderers = false;
		public List<CanvasRenderer> canvasRenderers = new List<CanvasRenderer>();
		protected List<SkeletonSubmeshGraphic> submeshGraphics = new List<SkeletonSubmeshGraphic>();
		protected int usedRenderersCount = 0;

		// Submesh Separation
		public const string SeparatorPartGameObjectName = "Part";
		/// <summary>Slot names used to populate separatorSlots list when the Skeleton is initialized. Changing this after initialization does nothing.</summary>
		[SerializeField] [SpineSlot] protected string[] separatorSlotNames = new string[0];

		/// <summary>Slots that determine where the render is split. This is used by components such as SkeletonRenderSeparator so that the skeleton can be rendered by two separate renderers on different GameObjects.</summary>
		[System.NonSerialized] public readonly List<Slot> separatorSlots = new List<Slot>();
		public bool enableSeparatorSlots = false;
		[SerializeField] protected List<Transform> separatorParts = new List<Transform>();
		public List<Transform> SeparatorParts { get { return separatorParts; } }
		public bool updateSeparatorPartLocation = true;
		public bool updateSeparatorPartScale = false;

		private bool wasUpdatedAfterInit = true;
		private Texture baseTexture = null;

		// TODO: MeshGenerator related local var, Move to MeshGenerator.cs, or we could use ExposedList

#if UNITY_EDITOR
        protected override void OnValidate () {
			// This handles Scene View preview.
			base.OnValidate();
			if (this.IsValid) {
				if (skeletonDataAsset == null) {
					Clear();
				} else if (skeletonDataAsset.skeletonJSON == null) {
					Clear();
				} else if (skeletonDataAsset.GetSkeletonData(true) != skeleton.Data) {
					Clear();
					Initialize(true);
					if (!allowMultipleCanvasRenderers && (skeletonDataAsset.atlasAssets.Length > 1 || skeletonDataAsset.atlasAssets[0].MaterialCount > 1))
						Debug.LogError("Unity UI does not support multiple textures per Renderer. Please enable 'Advanced - Multiple CanvasRenderers' to generate the required CanvasRenderer GameObjects. Otherwise your skeleton will not be rendered correctly.", this);
				} else {
					if (freeze) return;

					if (!Application.isPlaying) {
						Initialize(true);
						return;
					}

					if (!string.IsNullOrEmpty(initialSkinName)) {
						Skin skin = skeleton.Data.FindSkin(initialSkinName);
						if (skin != null) {
							if (skin == skeleton.Data.DefaultSkin)
								skeleton.SetSkin((Skin)null);
							else
								skeleton.SetSkin(skin);
						}

					}
				}
			} else {
				// Under some circumstances (e.g. sometimes on the first import) OnValidate is called
				// before SpineEditorUtilities.ImportSpineContent, causing an unnecessary exception.
				// The (skeletonDataAsset.skeletonJSON != null) condition serves to prevent this exception.
				if (skeletonDataAsset != null && skeletonDataAsset.skeletonJSON != null)
					Initialize(true);
			}
		}

		protected override void Reset () {

			base.Reset();
			if (material == null || material.shader != Shader.Find("Spine/SkeletonGraphic"))
				Debug.LogWarning("SkeletonGraphic works best with the SkeletonGraphic material.");
		}
#endif
		#endregion

		#region Runtime Instantiation
		/// <summary>Create a new GameObject with a SkeletonGraphic component.</summary>
		/// <param name="material">Material for the canvas renderer to use. Usually, the default SkeletonGraphic material will work.</param>
		public static SkeletonGraphic NewSkeletonGraphicGameObject (SkeletonDataAsset skeletonDataAsset, Transform parent, Material material) {
			SkeletonGraphic sg = SkeletonGraphic.AddSkeletonGraphicComponent(new GameObject("New Spine GameObject"), skeletonDataAsset, material);
			if (parent != null) sg.transform.SetParent(parent, false);
			return sg;
		}

		/// <summary>Add a SkeletonGraphic component to a GameObject.</summary>
		/// <param name="material">Material for the canvas renderer to use. Usually, the default SkeletonGraphic material will work.</param>
		public static SkeletonGraphic AddSkeletonGraphicComponent (GameObject gameObject, SkeletonDataAsset skeletonDataAsset, Material material) {
			SkeletonGraphic skeletonGraphic = gameObject.AddComponent<SkeletonGraphic>();
			if (skeletonDataAsset != null) {
				skeletonGraphic.material = material;
				skeletonGraphic.skeletonDataAsset = skeletonDataAsset;
				skeletonGraphic.Initialize(false);
			}
#if HAS_CULL_TRANSPARENT_MESH
			CanvasRenderer canvasRenderer = gameObject.GetComponent<CanvasRenderer>();
			if (canvasRenderer) canvasRenderer.cullTransparentMesh = false;
#endif
			return skeletonGraphic;
		}
		#endregion

		#region Overrides
		// API for taking over rendering.
		/// <summary>When true, no meshes and materials are assigned at CanvasRenderers if the used override
		/// AssignMeshOverrideSingleRenderer or AssignMeshOverrideMultipleRenderers is non-null.</summary>
		public bool disableMeshAssignmentOnOverride = true;
		/// <summary>Delegate type for overriding mesh and material assignment,
		/// used when <c>allowMultipleCanvasRenderers</c> is false.</summary>
		/// <param name="mesh">Mesh normally assigned at the main CanvasRenderer.</param>
		/// <param name="graphicMaterial">Material normally assigned at the main CanvasRenderer.</param>
		/// <param name="texture">Texture normally assigned at the main CanvasRenderer.</param>
		public delegate void MeshAssignmentDelegateSingle (Mesh mesh, Material graphicMaterial, Texture texture);
		/// <param name="meshCount">Number of meshes. Don't use <c>meshes.Length</c> as this might be higher
		/// due to pre-allocated entries.</param>
		/// <param name="meshes">Mesh array where each element is normally assigned to one of the <c>canvasRenderers</c>.</param>
		/// <param name="graphicMaterials">Material array where each element is normally assigned to one of the <c>canvasRenderers</c>.</param>
		/// <param name="textures">Texture array where each element is normally assigned to one of the <c>canvasRenderers</c>.</param>
		public delegate void MeshAssignmentDelegateMultiple (int meshCount, Mesh[] meshes, Material[] graphicMaterials, Texture[] textures);
		event MeshAssignmentDelegateSingle assignMeshOverrideSingle;
		event MeshAssignmentDelegateMultiple assignMeshOverrideMultiple;

		/// <summary>Allows separate code to take over mesh and material assignment for this SkeletonGraphic component.
		/// Used when <c>allowMultipleCanvasRenderers</c> is false.</summary>
		public event MeshAssignmentDelegateSingle AssignMeshOverrideSingleRenderer {
			add {
				assignMeshOverrideSingle += value;
				if (disableMeshAssignmentOnOverride && assignMeshOverrideSingle != null) {
					Initialize(false);
				}
			}
			remove {
				assignMeshOverrideSingle -= value;
				if (disableMeshAssignmentOnOverride && assignMeshOverrideSingle == null) {
					Initialize(false);
				}
			}
		}
		/// <summary>Allows separate code to take over mesh and material assignment for this SkeletonGraphic component.
		/// Used when <c>allowMultipleCanvasRenderers</c> is true.</summary>
		public event MeshAssignmentDelegateMultiple AssignMeshOverrideMultipleRenderers {
			add {
				assignMeshOverrideMultiple += value;
				if (disableMeshAssignmentOnOverride && assignMeshOverrideMultiple != null) {
					Initialize(false);
				}
			}
			remove {
				assignMeshOverrideMultiple -= value;
				if (disableMeshAssignmentOnOverride && assignMeshOverrideMultiple == null) {
					Initialize(false);
				}
			}
		}


		[System.NonSerialized] readonly Dictionary<Texture, Texture> customTextureOverride = new Dictionary<Texture, Texture>();
		/// <summary>Use this Dictionary to override a Texture with a different Texture.</summary>
		public Dictionary<Texture, Texture> CustomTextureOverride { get { return customTextureOverride; } }

		[System.NonSerialized] readonly Dictionary<Texture, Material> customMaterialOverride = new Dictionary<Texture, Material>();
		/// <summary>Use this Dictionary to override the Material where the Texture was used at the original atlas.</summary>
		public Dictionary<Texture, Material> CustomMaterialOverride { get { return customMaterialOverride; } }

		// This is used by the UI system to determine what to put in the MaterialPropertyBlock.
		Texture overrideTexture;
		public Texture OverrideTexture {
			get { return overrideTexture; }
			set {
				overrideTexture = value;
				canvasRenderer.SetTexture(this.mainTexture); // Refresh canvasRenderer's texture. Make sure it handles null.
			}
		}
		#endregion

		#region Internals
		public override Texture mainTexture {
			get {
				if (overrideTexture != null) return overrideTexture;
				return baseTexture;
			}
		}

		IntPtr generatorNativeHandle => meshGenerator.meshGeneratorHandle;

		protected override void Awake () {
			base.Awake();
			//if (generatorNativeHandle == IntPtr.Zero) { generatorNativeHandle = spine_mesh_generator_create_unity(); }

			spine_mesh_generator_set_settings_unity(generatorNativeHandle, ref meshGenerator.settings);

            this.onCullStateChanged.AddListener(OnCullStateChanged);

			SyncSubmeshGraphicsWithCanvasRenderers();
			if (!this.IsValid) {
#if UNITY_EDITOR
				// workaround for special import case of open scene where OnValidate and Awake are
				// called in wrong order, before setup of Spine assets.
				if (!Application.isPlaying) {
					if (this.skeletonDataAsset != null && this.skeletonDataAsset.skeletonJSON == null)
						return;
				}
#endif
				Initialize(false);
				if (this.IsValid) Rebuild(CanvasUpdate.PreRender);
			}
#if UNITY_EDITOR
			InitLayoutScaleParameters();
#endif
		}

		protected override void OnDestroy () {
			Clear();
			base.OnDestroy();

        }

		public override void Rebuild(CanvasUpdate update) {
			base.Rebuild(update);
			if (!this.IsValid) return;
			if (canvasRenderer.cull) return;
			if (update == CanvasUpdate.PreRender) {
				if (allowMultipleCanvasRenderers)
				{
                    GenerateDrawMeshRenderers();
                } 
				else
				{
                    PrepareInstructionsAndRenderers(isInRebuild: true);
                    UpdateMeshToInstructions();
                }

			}
			if (allowMultipleCanvasRenderers) canvasRenderer.Clear();
		}

		protected override void OnDisable () {
			base.OnDisable();
			foreach (CanvasRenderer canvasRenderer in canvasRenderers) {
				canvasRenderer.Clear();
			}
		}

		public virtual void Update () {
#if UNITY_EDITOR
			UpdateReferenceRectSizes();
			if (!Application.isPlaying) {
				Update(0f);
				return;
			}
#endif
			if (freeze || updateTiming != UpdateTiming.InUpdate) return;
			Update(unscaledTime ? Time.unscaledDeltaTime : Time.deltaTime);
		}

		virtual protected void FixedUpdate () {
			if (freeze || updateTiming != UpdateTiming.InFixedUpdate) return;
			Update(unscaledTime ? Time.unscaledDeltaTime : Time.deltaTime);
		}


        public virtual void Update (float deltaTime) {
			if (!this.IsValid) return;

			wasUpdatedAfterInit = true;
			if (updateMode < UpdateMode.OnlyAnimationStatus)
				return;
			UpdateAnimationStatus(deltaTime);

			if (updateMode == UpdateMode.OnlyAnimationStatus) {
				state.ApplyEventTimelinesOnly(skeleton, issueEvents: false);
				return;
			}

			ApplyAnimation();
		}

		protected void SyncSubmeshGraphicsWithCanvasRenderers () {
			submeshGraphics.Clear();

#if UNITY_EDITOR
			if (!Application.isPlaying)
				DestroyOldRawImages();
#endif
			foreach (CanvasRenderer canvasRenderer in canvasRenderers) {
				SkeletonSubmeshGraphic submeshGraphic = canvasRenderer.GetComponent<SkeletonSubmeshGraphic>();
				if (submeshGraphic == null) {
					submeshGraphic = canvasRenderer.gameObject.AddComponent<SkeletonSubmeshGraphic>();
					submeshGraphic.maskable = this.maskable;
					submeshGraphic.raycastTarget = false;
				}
				submeshGraphics.Add(submeshGraphic);
			}
		}

		protected void UpdateAnimationStatus (float deltaTime) {
			deltaTime *= timeScale;
			state.Update(deltaTime);
		}

        protected void ApplyAnimation () {
			if (BeforeApply != null)
				BeforeApply(this);


            if (updateMode != UpdateMode.OnlyEventTimelines)
				state.Apply(skeleton);
			else
			{
                state.ApplyEventTimelinesOnly(skeleton, issueEvents: true);
            }

            AfterAnimationApplied();

        }


        public void AfterAnimationApplied () {
			if (UpdateLocal != null)
				UpdateLocal(this);

            skeleton.UpdateWorldTransform();

            if (UpdateWorld != null) {
				UpdateWorld(this);
				skeleton.UpdateWorldTransform();
			}


            if (UpdateComplete != null)
				UpdateComplete(this);
		}

		public void LateUpdate () {
			if (!this.IsValid) return;
			// instantiation can happen from Update() after this component, leading to a missing Update() call.
			if (!wasUpdatedAfterInit) Update(0);
			if (freeze) return;
			if (updateMode != UpdateMode.FullUpdate) return;

			if (updateTiming == UpdateTiming.InLateUpdate)
				Update(unscaledTime ? Time.unscaledDeltaTime : Time.deltaTime);

			UpdateMesh();
		}

		protected void OnCullStateChanged (bool culled) {
			if (culled)
				OnBecameInvisible();
			else
				OnBecameVisible();
		}

		public void OnBecameVisible () {
			updateMode = UpdateMode.FullUpdate;
		}

		public void OnBecameInvisible () {
			updateMode = updateWhenInvisible;
		}

		public void ReapplySeparatorSlotNames () {
			if (!IsValid)
				return;

			separatorSlots.Clear();
			for (int i = 0, n = separatorSlotNames.Length; i < n; i++) {
				string slotName = separatorSlotNames[i];
				if (slotName == "")
					continue;
				Slot slot = skeleton.FindSlot(slotName);
				if (slot != null) {
					separatorSlots.Add(slot);
				}
#if UNITY_EDITOR
				else {
					Debug.LogWarning(slotName + " is not a slot in " + skeletonDataAsset.skeletonJSON.name);
				}
#endif
			}
			UpdateSeparatorPartParents();
		}
		#endregion

		#region API
		protected Skeleton skeleton;
		public Skeleton Skeleton {
			get {
				Initialize(false);
				return skeleton;
			}
			set {
				skeleton = value;
			}
		}
		public SkeletonData SkeletonData {
			get {
				Initialize(false);
				return skeleton == null ? null : skeleton.Data;
			}
		}

		public bool IsValid { get { return skeleton != null; } }

		public delegate void SkeletonRendererDelegate (SkeletonGraphic skeletonGraphic);
		public delegate void InstructionDelegate (SkeletonRendererInstruction instruction);

		/// <summary>OnRebuild is raised after the Skeleton is successfully initialized.</summary>
		public event SkeletonRendererDelegate OnRebuild;

		/// <summary>OnInstructionsPrepared is raised at the end of <c>LateUpdate</c> after render instructions
		/// are done, target renderers are prepared, and the mesh is ready to be generated.</summary>
		public event InstructionDelegate OnInstructionsPrepared;

		/// <summary>OnMeshAndMaterialsUpdated is raised at the end of <c>Rebuild</c> after the Mesh and
		/// all materials have been updated. Note that some Unity API calls are not permitted to be issued from
		/// <c>Rebuild</c>, so you may want to subscribe to <see cref="OnInstructionsPrepared"/> instead
		/// from where you can issue such preparation calls.</summary>
		public event SkeletonRendererDelegate OnMeshAndMaterialsUpdated;

		protected Spine.AnimationState state;


		public Spine.AnimationState AnimationState {
			get {
				Initialize(false);
				return state;
			}
		}

		[SerializeField] protected Spine.Unity.MeshGenerator meshGenerator = new MeshGenerator();
		public Spine.Unity.MeshGenerator MeshGenerator { get { return this.meshGenerator; } }
		DoubleBuffered<Spine.Unity.MeshRendererBuffers.SmartMesh> meshBuffers;
		SkeletonRendererInstruction currentInstructions = new SkeletonRendererInstruction();
		readonly ExposedList<Mesh> meshes = new ExposedList<Mesh>();
		readonly ExposedList<Material> usedMaterials = new ExposedList<Material>();
		readonly ExposedList<Texture> usedTextures = new ExposedList<Texture>();

		public ExposedList<Mesh> MeshesMultipleCanvasRenderers { get { return meshes; } }
		public ExposedList<Material> MaterialsMultipleCanvasRenderers { get { return usedMaterials; } }
		public ExposedList<Texture> TexturesMultipleCanvasRenderers { get { return usedTextures; } }

		public Mesh GetLastMesh () {
			return meshBuffers.GetCurrent().mesh;
		}

		public bool MatchRectTransformWithBounds () {
			if (!wasUpdatedAfterInit) Update(0);
			UpdateMesh();

			if (!this.allowMultipleCanvasRenderers)
				return MatchRectTransformSingleRenderer();
			else
				return MatchRectTransformMultipleRenderers();
		}

		protected bool MatchRectTransformSingleRenderer () {
			Mesh mesh = this.GetLastMesh();
			if (mesh == null) {
				return false;
			}
			if (mesh.vertexCount == 0 || mesh.bounds.size == Vector3.zero) {
				this.rectTransform.sizeDelta = new Vector2(50f, 50f);
				this.rectTransform.pivot = new Vector2(0.5f, 0.5f);
				return false;
			}
			mesh.RecalculateBounds();
			SetRectTransformBounds(mesh.bounds);
			return true;
		}

		protected bool MatchRectTransformMultipleRenderers () {
			bool anyBoundsAdded = false;
			Bounds combinedBounds = new Bounds();
			for (int i = 0; i < canvasRenderers.Count; ++i) {
				CanvasRenderer canvasRenderer = canvasRenderers[i];
				if (!canvasRenderer.gameObject.activeSelf)
					continue;

				Mesh mesh = meshes.Items[i];
				if (mesh == null || mesh.vertexCount == 0)
					continue;

				mesh.RecalculateBounds();
				Bounds bounds = mesh.bounds;
				if (anyBoundsAdded)
					combinedBounds.Encapsulate(bounds);
				else {
					anyBoundsAdded = true;
					combinedBounds = bounds;
				}
			}

			if (!anyBoundsAdded || combinedBounds.size == Vector3.zero) {
				this.rectTransform.sizeDelta = new Vector2(50f, 50f);
				this.rectTransform.pivot = new Vector2(0.5f, 0.5f);
				return false;
			}

			SetRectTransformBounds(combinedBounds);
			return true;
		}

		private void SetRectTransformBounds (Bounds combinedBounds) {
			Vector3 size = combinedBounds.size;
			Vector3 center = combinedBounds.center;
			Vector2 p = new Vector2(
				0.5f - (center.x / size.x),
				0.5f - (center.y / size.y)
			);

			SetRectTransformSize(this, size);
			this.rectTransform.pivot = p;

			foreach (Transform separatorPart in separatorParts) {
				RectTransform separatorTransform = separatorPart.GetComponent<RectTransform>();
				if (separatorTransform) {
					SetRectTransformSize(separatorTransform, size);
					separatorTransform.pivot = p;
				}
			}
			foreach (SkeletonSubmeshGraphic submeshGraphic in submeshGraphics) {
				SetRectTransformSize(submeshGraphic, size);
				submeshGraphic.rectTransform.pivot = p;
			}

			this.referenceSize = size;
		}

		public static void SetRectTransformSize (Graphic target, Vector2 size) {
			SetRectTransformSize(target.rectTransform, size);
		}

		public static void SetRectTransformSize (RectTransform targetRectTransform, Vector2 size) {
			Vector2 parentSize = Vector2.zero;
			if (targetRectTransform.parent != null) {
				RectTransform parentTransform = targetRectTransform.parent.GetComponent<RectTransform>();
				if (parentTransform)
					parentSize = parentTransform.rect.size;
			}
			Vector2 anchorAreaSize = Vector2.Scale(targetRectTransform.anchorMax - targetRectTransform.anchorMin, parentSize);
			targetRectTransform.sizeDelta = size - anchorAreaSize;
		}

		/// <summary>OnAnimationRebuild is raised after the SkeletonAnimation component is successfully initialized.</summary>
		public event ISkeletonAnimationDelegate OnAnimationRebuild;
		public event UpdateBonesDelegate BeforeApply;
		public event UpdateBonesDelegate UpdateLocal;
		public event UpdateBonesDelegate UpdateWorld;
		public event UpdateBonesDelegate UpdateComplete;

		[SerializeField] protected UpdateTiming updateTiming = UpdateTiming.InUpdate;
		public UpdateTiming UpdateTiming { get { return updateTiming; } set { updateTiming = value; } }

		[SerializeField] protected bool unscaledTime;
		public bool UnscaledTime { get { return unscaledTime; } set { unscaledTime = value; } }

		/// <summary> Occurs after the vertex data populated every frame, before the vertices are pushed into the mesh.</summary>
		public event Spine.Unity.MeshGeneratorDelegate OnPostProcessVertices;

		public void Clear () {
			skeleton = null;
			canvasRenderer.Clear();

			for (int i = 0; i < canvasRenderers.Count; ++i)
				canvasRenderers[i].Clear();
			DestroyMeshes();
			usedMaterials.Clear();
			usedTextures.Clear();
			DisposeMeshBuffers();
		}

		public void TrimRenderers () {
			List<CanvasRenderer> newList = new List<CanvasRenderer>();
			foreach (CanvasRenderer canvasRenderer in canvasRenderers) {
				if (canvasRenderer.gameObject.activeSelf) {
					newList.Add(canvasRenderer);
				} else {
					if (Application.isEditor && !Application.isPlaying)
						DestroyImmediate(canvasRenderer.gameObject);
					else
						Destroy(canvasRenderer.gameObject);
				}
			}
			canvasRenderers = newList;
			SyncSubmeshGraphicsWithCanvasRenderers();
		}


        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern IntPtr spine_skeleton_data_find_animation_unity(IntPtr skeletonDataHandle, string startingAnimation);


        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern int spine_animation_state_set_and_remove_animation_unity(IntPtr animationStateHandle, int trackIndex, IntPtr animationHandle, bool loop);



        public void Initialize (bool overwrite) {
			if (this.IsValid && !overwrite) return;
#if UNITY_EDITOR
			if (BuildUtilities.IsInSkeletonAssetBuildPreProcessing)
				return;
#endif
			if (this.skeletonDataAsset == null) return;
			SkeletonData skeletonData = this.skeletonDataAsset.GetSkeletonData(false);
			if (skeletonData == null) return;

			if (skeletonDataAsset.atlasAssets.Length <= 0 || skeletonDataAsset.atlasAssets[0].MaterialCount <= 0) return;

			this.skeleton = new Skeleton(skeletonData) {
				ScaleX = this.initialFlipX ? -1 : 1,
				ScaleY = this.initialFlipY ? -1 : 1
			};



			InitMeshBuffers();
			baseTexture = skeletonDataAsset.atlasAssets[0].PrimaryMaterial.mainTexture;
			canvasRenderer.SetTexture(this.mainTexture); // Needed for overwriting initializations.

			// Set the initial Skin and Animation
			if (!string.IsNullOrEmpty(initialSkinName))
				skeleton.SetSkin(initialSkinName);

			separatorSlots.Clear();
			for (int i = 0; i < separatorSlotNames.Length; i++)
				separatorSlots.Add(skeleton.FindSlot(separatorSlotNames[i]));

			if (OnRebuild != null)
				OnRebuild(this);

			wasUpdatedAfterInit = false;
			this.state = new Spine.AnimationState(skeletonDataAsset.GetAnimationStateData());
			if (state == null) {
				Clear();
				return;
			}

            if (!string.IsNullOrEmpty(startingAnimation))
            {
                IntPtr animationHandle = spine_skeleton_data_find_animation_unity(skeletonData.skeletonDataHandle, startingAnimation);
                if (animationHandle != IntPtr.Zero)
                {
                    spine_animation_state_set_and_remove_animation_unity(state.animationStateHandle, 0, animationHandle, startingLoop);

#if UNITY_EDITOR
                    if (!Application.isPlaying)
                        Update(0f);
#endif
                }
            }

            if (OnAnimationRebuild != null)
				OnAnimationRebuild(this);

            spine_mesh_generator_set_settings_unity(generatorNativeHandle, ref meshGenerator.settings);

        }


		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern void spine_mesh_generator_set_settings_unity(IntPtr meshRendererHandle, ref MeshGenerator.Settings settings);

        public void PrepareInstructionsAndRenderers (bool isInRebuild = false) {

            if (!this.allowMultipleCanvasRenderers) {

				meshGenerator.GenerateSingleSubmeshInstructionNative(currentInstructions, skeleton, null);


				if (canvasRenderers.Count > 0)
					DisableUnusedCanvasRenderers(usedCount: 0, isInRebuild: isInRebuild);
				usedRenderersCount = 0;
			} else {
				//MeshGenerator.GenerateSkeletonRendererInstruction(currentInstructions, skeleton, null,
				//	enableSeparatorSlots ? separatorSlots : null,
				//	enableSeparatorSlots ? separatorSlots.Count > 0 : false,
				//	false);

				meshGenerator.GenerateSkeletonRendererInstructionNative(currentInstructions, skeleton, null,
					enableSeparatorSlots ? separatorSlots : null, 
					enableSeparatorSlots ? separatorSlots.Count > 0 : false,
					false);

				int submeshCount = currentInstructions.submeshInstructions.Count;
				EnsureCanvasRendererCount(submeshCount);
				EnsureMeshesCount(submeshCount);
				EnsureUsedTexturesAndMaterialsCount(submeshCount);
				EnsureSeparatorPartCount();

				// submesh count, submesh item. force seperate
				PrepareRendererGameObjects(currentInstructions, isInRebuild);
			}
			if (OnInstructionsPrepared != null)
				OnInstructionsPrepared(this.currentInstructions);

        }

        static readonly ProfilerMarker s_UpdateMarker2 = new ProfilerMarker("SkeletonGraphic.PrepareInstructionsAndRenderers");

        static readonly ProfilerMarker s_UpdateMarker3 = new ProfilerMarker("SkeletonGraphic.UpdateMeshToInstructions");

        public void UpdateMesh () {


			if (!this.allowMultipleCanvasRenderers)
			{
                s_UpdateMarker2.Begin();
                PrepareInstructionsAndRenderers();

                s_UpdateMarker2.End();

                s_UpdateMarker3.Begin();

                UpdateMeshToInstructions();

                s_UpdateMarker3.End();

                return;
			}
			else
			{
				GenerateDrawMeshRenderers();

			}


		}

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_mesh_generator_generate_mesh_renderers_unity(IntPtr meshGeneratorHandle, IntPtr skeletonHandle, int[] outPrams);

        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern int spine_mesh_generator_get_mesh_renderers_count_unity(IntPtr meshGeneratorHandle);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_mesh_generator_get_page_indexes_unity(IntPtr meshGeneratorHandle, Int32[] pageIndex);

		[DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_mesh_generator_get_size_unity(IntPtr meshGeneratorHandle, int meshIndex, int[] outVertexCount, int[] outIndicesCount);


        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
		static extern void spine_mesh_generator_update_multiple_data_unity(IntPtr meshGeneratorHandle, int meshIndex, Vector3[] outVertexes, 
			Vector2[] outUvs, Color[] outColors, ushort[] outIndices, float meshScale);

        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        unsafe static extern void spine_mesh_generator_get_results_unity(IntPtr generatorHandle, float meshScale, int[] outSubmeshVertexCount, int[] outSubmeshTriangleCount,
    float* outVertex, float* outUv, UInt32* outColor, float[] outBounds, ushort[] outTriangles, bool calcTangents, float* outTangent);



        Vector3[] vb = new Vector3[4];
		Vector2[] ub = new Vector2[4];
		Color[] cb = new Color[4];
		ushort[] indices = new ushort[6];


        NativeArray<UInt32> nativeColorfb = new NativeArray<UInt32>(4, Allocator.Persistent);
        NativeArray<float3> nativeVertexfb = new NativeArray<float3>(4, Allocator.Persistent);
        NativeArray<float2> nativeUvfb = new NativeArray<float2>(4, Allocator.Persistent);
        NativeArray<float4> nativeTangentfb = new NativeArray<float4>(4, Allocator.Persistent);
        ushort[] triangles = new ushort[6];


        static readonly ProfilerMarker s_UpdateMarker11 = new ProfilerMarker("SkeletonGraphic.PrepareInstructionsAndRenderers");

        static readonly ProfilerMarker s_UpdateMarker12 = new ProfilerMarker("SkeletonGraphic.PrepareRendererGameObjects");

        static readonly ProfilerMarker s_UpdateMarker13 = new ProfilerMarker("SkeletonGraphic.UpdateMaterialsMultipleCanvasRenderers");

        static readonly ProfilerMarker s_UpdateMarker14 = new ProfilerMarker("SkeletonGraphic.UpdateMeshMultipleCanvasRenderers");

        static readonly ProfilerMarker s_UpdateMarker15 = new ProfilerMarker("SkeletonGraphic.Ensure");


        public void GenerateDrawMeshRenderers()
		{
            s_UpdateMarker11.Begin();

            int[] outParams = new int[10];
			spine_mesh_generator_generate_mesh_renderers_unity(meshGenerator.meshGeneratorHandle, skeleton.skeletonHandle, outParams);

            s_UpdateMarker11.End();

            //int submeshCount = spine_mesh_generator_get_mesh_renderers_count_unity(meshGenerator.meshGeneratorHandle);
            int submeshCount = outParams[0];

			if (!this.IsValid || submeshCount == 0) return;

			s_UpdateMarker15.Begin();


            EnsureCanvasRendererCount(submeshCount);
			EnsureMeshesCount(submeshCount);
			EnsureUsedTexturesAndMaterialsCount(submeshCount);
			EnsureSeparatorPartCount();

			s_UpdateMarker15.End();


            s_UpdateMarker12.Begin();

            //PrepareRendererGameObjects(currentInstructions, false);
            {

                DisableUnusedCanvasRenderers(usedCount: submeshCount, isInRebuild: false);

				Transform parent = this.separatorParts.Count == 0 ? this.transform : this.separatorParts[0];
				if (updateSeparatorPartLocation)
				{
					for (int p = 0; p < this.separatorParts.Count; ++p)
					{
						Transform separatorPart = separatorParts[p];
						if (separatorPart == null) continue;
						separatorPart.position = this.transform.position;
						separatorPart.rotation = this.transform.rotation;
					}
				}
				if (updateSeparatorPartScale)
				{
					Vector3 targetScale = this.transform.lossyScale;
					for (int p = 0; p < this.separatorParts.Count; ++p)
					{
						Transform separatorPart = separatorParts[p];
						if (separatorPart == null) continue;
						Transform partParent = separatorPart.parent;
						Vector3 parentScale = partParent == null ? Vector3.one : partParent.lossyScale;
						separatorPart.localScale = new Vector3(
							parentScale.x == 0f ? 1f : targetScale.x / parentScale.x,
							parentScale.y == 0f ? 1f : targetScale.y / parentScale.y,
							parentScale.z == 0f ? 1f : targetScale.z / parentScale.z);
					}
				}

				int separatorSlotGroupIndex = 0;
				int targetSiblingIndex = 0;
				for (int i = 0; i < submeshCount; i++)
				{
					CanvasRenderer canvasRenderer = canvasRenderers[i];
					if (canvasRenderer != null)
					{
						if (i >= usedRenderersCount)
							canvasRenderer.gameObject.SetActive(true);

						if (canvasRenderer.transform.parent != parent.transform)
							canvasRenderer.transform.SetParent(parent.transform, false);

						canvasRenderer.transform.SetSiblingIndex(targetSiblingIndex++);
					}

					SkeletonSubmeshGraphic submeshGraphic = submeshGraphics[i];
					if (submeshGraphic != null)
					{
						RectTransform dstTransform = submeshGraphic.rectTransform;
						dstTransform.localPosition = Vector3.zero;
						dstTransform.pivot = rectTransform.pivot;
						dstTransform.anchorMin = Vector2.zero;
						dstTransform.anchorMax = Vector2.one;
						dstTransform.sizeDelta = Vector2.zero;
					}

					//SubmeshInstruction submeshInstructionItem = currentInstructions.submeshInstructions.Items[i];
					//if (submeshInstructionItem.forceSeparate)
					//{
					//    targetSiblingIndex = 0;
					//    parent = separatorParts[++separatorSlotGroupIndex];
					//}
				}
				usedRenderersCount = submeshCount;
			}

			s_UpdateMarker12.End();

			s_UpdateMarker13.Begin();

            skeleton.SetColor(this.color);


            //UpdateMaterialsMultipleCanvasRenderers(currentInstructions);
            {
                bool useOriginalTextureAndMaterial = (customMaterialOverride.Count == 0 && customTextureOverride.Count == 0);

                BlendModeMaterials blendModeMaterials = skeletonDataAsset.blendModeMaterials;
                bool hasBlendModeMaterials = blendModeMaterials.RequiresBlendModeMaterials;

                bool pmaVertexColors = meshGenerator.settings.pmaVertexColors;
                Material[] usedMaterialItems = usedMaterials.Items;
                Texture[] usedTextureItems = usedTextures.Items;
				List<AtlasPage> atlasPages = skeletonDataAsset.atlasAssets[0].GetAtlas().Pages;
				Int32[] pageIndexes = new Int32[submeshCount];
				spine_mesh_generator_get_page_indexes_unity(meshGenerator.meshGeneratorHandle, pageIndexes);
                for (int i = 0; i < submeshCount; i++)
                {
                    //SubmeshInstruction submeshInstructionItem = currentInstructions.submeshInstructions.Items[i];
                    //Material submeshMaterial = submeshInstructionItem.material;
					AtlasPage atlasPage = atlasPages[pageIndexes[i]];
					Material submeshMaterial = (Material)atlasPage.rendererObject;
                    if (useOriginalTextureAndMaterial)
                    {
                        if (submeshMaterial == null)
                        {
                            usedMaterialItems[i] = null;
                            usedTextureItems[i] = null;
                            continue;
                        }
                        usedTextureItems[i] = submeshMaterial.mainTexture;
                        if (!hasBlendModeMaterials)
                        {
                            usedMaterialItems[i] = this.materialForRendering;
                        }
                        else
                        {
                            BlendMode blendMode = blendModeMaterials.BlendModeForMaterial(submeshMaterial);
                            Material usedMaterial = this.materialForRendering;
                            if (blendMode == BlendMode.Additive && !pmaVertexColors && additiveMaterial)
                            {
                                usedMaterial = additiveMaterial;
                            }
                            else if (blendMode == BlendMode.Multiply && multiplyMaterial)
                                usedMaterial = multiplyMaterial;
                            else if (blendMode == BlendMode.Screen && screenMaterial)
                                usedMaterial = screenMaterial;
                            usedMaterialItems[i] = submeshGraphics[i].GetModifiedMaterial(usedMaterial);
                        }
                    }
                    else
                    {
                        Texture originalTexture = submeshMaterial.mainTexture;
                        Material usedMaterial;
                        Texture usedTexture;
                        if (!customMaterialOverride.TryGetValue(originalTexture, out usedMaterial))
                            usedMaterial = material;
                        if (!customTextureOverride.TryGetValue(originalTexture, out usedTexture))
                            usedTexture = originalTexture;

                        usedMaterialItems[i] = submeshGraphics[i].GetModifiedMaterial(usedMaterial);
                        usedTextureItems[i] = usedTexture;
                    }
                }
            }


			s_UpdateMarker13.End();

			s_UpdateMarker14.Begin();

            //UpdateMeshMultipleCanvasRenderers(currentInstructions);
            {
                meshScale = (canvas == null) ? 100 : canvas.referencePixelsPerUnit;
                if (layoutScaleMode != LayoutMode.None)
                {
                    meshScale *= referenceScale;
                    if (!EditReferenceRect)
                        meshScale *= GetLayoutScale(layoutScaleMode);
                }
                // Generate meshes.
                Mesh[] meshesItems = meshes.Items;
                bool useOriginalTextureAndMaterial = (customMaterialOverride.Count == 0 && customTextureOverride.Count == 0);

                BlendModeMaterials blendModeMaterials = skeletonDataAsset.blendModeMaterials;
                bool hasBlendModeMaterials = blendModeMaterials.RequiresBlendModeMaterials;
#if HAS_CULL_TRANSPARENT_MESH
                bool mainCullTransparentMesh = this.canvasRenderer.cullTransparentMesh;
#endif
                bool pmaVertexColors = meshGenerator.settings.pmaVertexColors;
                Material[] usedMaterialItems = usedMaterials.Items;
                Texture[] usedTextureItems = usedTextures.Items;

				int[] vertexCounts = new int[submeshCount];
				int[] triangleCounts = new int[submeshCount];
				float[] meshBounds = new float[submeshCount * 4];

				int totalVertexCount = outParams[1];
				int totalTriangleCount = outParams[2];

				if (nativeVertexfb.Length < totalVertexCount)
				{
					nativeVertexfb.Dispose();
					nativeVertexfb = new NativeArray<float3>(totalVertexCount, Allocator.Persistent);
					nativeUvfb.Dispose();
					nativeUvfb = new NativeArray<float2>(totalVertexCount, Allocator.Persistent);
					nativeColorfb.Dispose();
					nativeColorfb = new NativeArray<UInt32>(totalVertexCount, Allocator.Persistent);
				}

				if (triangles.Length < totalTriangleCount)
				{
					triangles = new ushort[totalTriangleCount];
				}

				unsafe
				{
					float* ptrVertex = (float*)NativeArrayUnsafeUtility.GetUnsafePtr(nativeVertexfb);
					float* ptrUv = (float*)NativeArrayUnsafeUtility.GetUnsafePtr(nativeUvfb);
                    UInt32* ptrColor = (UInt32*)NativeArrayUnsafeUtility.GetUnsafePtr(nativeColorfb);
					float* ptrTangent = (float*)NativeArrayUnsafeUtility.GetUnsafePtr(nativeTangentfb);


					spine_mesh_generator_get_results_unity(meshGenerator.meshGeneratorHandle, meshScale, vertexCounts, triangleCounts,
						ptrVertex, ptrUv, ptrColor, meshBounds, triangles, false, ptrTangent);


                }

				int vertexStart = 0;
				int triangleStart = 0;
                for (int i = 0; i < submeshCount; i++)
                {

                    Mesh targetMesh = meshesItems[i];
					targetMesh.subMeshCount = 1;
					//int[] vertexCounts = new int[1];
					//int[] indicesCounts = new int[1];

					//spine_mesh_generator_get_size_unity(meshGenerator.meshGeneratorHandle, i, vertexCounts, indicesCounts);
					//int vertexCount = vertexCounts[0];
					//int indicesCount = triangleCounts[0];
					int vertexCount = vertexCounts[i];
					int indicesCount = triangleCounts[i];

					//int vbSize = vertexCount;
					//int ibSize = indicesCount;
					//if (vb.Length < vertexCount)
					//{
					//	vb = new Vector3[vertexCount];
					//	ub = new Vector2[vertexCount];
					//	cb = new Color[vertexCount];
					//}
					//else
					//{
					//	vbSize = vb.Length;
					//}

					//if (indices.Length < indicesCount)
					//{
					//	indices = new ushort[indicesCount];
					//}

					//spine_mesh_generator_update_multiple_data_unity(meshGenerator.meshGeneratorHandle, i, vb, ub, cb, indices, meshScale);

					//targetMesh.vertices = vb;
					//targetMesh.uv = ub;
					//targetMesh.colors = cb;

					targetMesh.SetVertices<float3>(nativeVertexfb, vertexStart, vertexCount);
					targetMesh.SetUVs<float2>(0, nativeUvfb, vertexStart, vertexCount);
					targetMesh.SetColors<UInt32>(nativeColorfb, vertexStart, vertexCount);
					targetMesh.SetTriangles(triangles, triangleStart, indicesCount, 0);

					vertexStart += vertexCount;
					triangleStart += indicesCount;

					//targetMesh.SetTriangles(indices, 0, indices.Length, 0, false);


                    CanvasRenderer canvasRenderer = canvasRenderers[i];
                    if (assignMeshOverrideSingle == null || !disableMeshAssignmentOnOverride)
                        canvasRenderer.SetMesh(targetMesh);
                    else
                        canvasRenderer.SetMesh(null);

                    SkeletonSubmeshGraphic submeshGraphic = submeshGraphics[i];
                    if (useOriginalTextureAndMaterial && hasBlendModeMaterials)
                    {
                        bool allowCullTransparentMesh = true;
                        BlendMode materialBlendMode = blendModeMaterials.BlendModeForMaterial(usedMaterialItems[i]);
                        if ((materialBlendMode == BlendMode.Normal ) || //&& submeshInstructionItem.hasPMAAdditiveSlot) ||
                            (materialBlendMode == BlendMode.Additive && pmaVertexColors))
                        {
                            allowCullTransparentMesh = false;
                        }
#if HAS_CULL_TRANSPARENT_MESH
                        canvasRenderer.cullTransparentMesh = allowCullTransparentMesh ?
                            mainCullTransparentMesh : false;
#endif
                    }
                    canvasRenderer.materialCount = 1;
                }

#if SPINE_OPTIONAL_ON_DEMAND_LOADING
                if (Application.isPlaying)
                    HandleOnDemandLoading();
#endif
                bool assignAtCanvasRenderer = (assignMeshOverrideSingle == null || !disableMeshAssignmentOnOverride);
                if (assignAtCanvasRenderer)
                {
                    for (int i = 0; i < submeshCount; i++)
                    {
                        CanvasRenderer canvasRenderer = canvasRenderers[i];
                        canvasRenderer.SetMaterial(usedMaterialItems[i], usedTextureItems[i]);
                    }
                }

                if (assignMeshOverrideMultiple != null)
                    assignMeshOverrideMultiple(submeshCount, meshesItems, usedMaterialItems, usedTextureItems);
            }

            if (OnMeshAndMaterialsUpdated != null){
				OnMeshAndMaterialsUpdated(this);
			}

			s_UpdateMarker14.End();
    }


        static readonly ProfilerMarker s_UpdateMarker4 = new ProfilerMarker("SkeletonGraphic.UpdateMaterialsMultipleCanvasRenderers");

        static readonly ProfilerMarker s_UpdateMarker5 = new ProfilerMarker("SkeletonGraphic.UpdateMeshMultipleCanvasRenderersNative");

        public void UpdateMeshToInstructions () {
			// rawVertexCount
			if (!this.IsValid || currentInstructions.rawVertexCount < 0) return;
			skeleton.SetColor(this.color);

			if (!this.allowMultipleCanvasRenderers) {
				UpdateMeshSingleCanvasRenderer(currentInstructions);
			} else {

                s_UpdateMarker4.Begin();
                //submesh count, submesh item
                UpdateMaterialsMultipleCanvasRenderers(currentInstructions);

                s_UpdateMarker4.End();

				s_UpdateMarker5.Begin();
                //UpdateMeshMultipleCanvasRenderers(currentInstructions);
                UpdateMeshMultipleCanvasRenderersNative(currentInstructions);

                s_UpdateMarker5.End();

            }

			if (OnMeshAndMaterialsUpdated != null)
				OnMeshAndMaterialsUpdated(this);
		}

		public bool HasMultipleSubmeshInstructions () {
			if (!IsValid)
				return false;
			return MeshGenerator.RequiresMultipleSubmeshesByDrawOrder(skeleton);
		}
		#endregion

		protected void InitMeshBuffers () {
			if (meshBuffers != null) {
				meshBuffers.GetNext().Clear();
				meshBuffers.GetNext().Clear();
			} else {
				meshBuffers = new DoubleBuffered<MeshRendererBuffers.SmartMesh>();
			}
		}

		protected void DisposeMeshBuffers () {
			if (meshBuffers != null) {
				meshBuffers.GetNext().Dispose();
				meshBuffers.GetNext().Dispose();
				meshBuffers = null;
			}
		}


        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        static extern bool spine_mesh_generator_generate_instruction_is_not_equal_unity(IntPtr meshGeneratorHandle);


        protected void UpdateMeshSingleCanvasRenderer (SkeletonRendererInstruction currentInstructions) {


            MeshRendererBuffers.SmartMesh smartMesh = meshBuffers.GetNext();
			bool updateTriangles = spine_mesh_generator_generate_instruction_is_not_equal_unity(meshGenerator.meshGeneratorHandle);
			meshGenerator.Begin();

            Mesh mesh = smartMesh.mesh;

            meshScale = (canvas == null) ? 100 : canvas.referencePixelsPerUnit;
            if (layoutScaleMode != LayoutMode.None)
            {
                meshScale *= referenceScale;
                if (!EditReferenceRect)
                    meshScale *= GetLayoutScale(layoutScaleMode);
            }

            bool useAddSubmesh = currentInstructions.hasActiveClipping && currentInstructions.submeshInstructions.Count > 0;
			if (useAddSubmesh) {
				meshGenerator.AddSubmesh(currentInstructions.submeshInstructions.Items[0], updateTriangles);
			} else {
				
                meshGenerator.BuildAndGetMeshFromNative(mesh, updateTriangles, meshScale, currentInstructions);

            }


			if (assignMeshOverrideSingle != null)
				assignMeshOverrideSingle(mesh, this.canvasRenderer.GetMaterial(), this.mainTexture);

			bool assignAtCanvasRenderer = (assignMeshOverrideSingle == null || !disableMeshAssignmentOnOverride);
			if (assignAtCanvasRenderer)
				canvasRenderer.SetMesh(mesh);
			else
				canvasRenderer.SetMesh(null);

			bool assignTexture = false;
			if (currentInstructions.submeshInstructions.Count > 0) {
				Material material = currentInstructions.submeshInstructions.Items[0].material;
				if (material != null && baseTexture != material.mainTexture) {
					baseTexture = material.mainTexture;
					if (overrideTexture == null && assignAtCanvasRenderer)
						assignTexture = true;
				}
			}

#if SPINE_OPTIONAL_ON_DEMAND_LOADING
			if (Application.isPlaying)
				HandleOnDemandLoading();
#endif
			if (assignTexture)
				canvasRenderer.SetTexture(this.mainTexture);


        }

        protected void UpdateMaterialsMultipleCanvasRenderers (SkeletonRendererInstruction currentInstructions) {
			int submeshCount = currentInstructions.submeshInstructions.Count;
			bool useOriginalTextureAndMaterial = (customMaterialOverride.Count == 0 && customTextureOverride.Count == 0);

			BlendModeMaterials blendModeMaterials = skeletonDataAsset.blendModeMaterials;
			bool hasBlendModeMaterials = blendModeMaterials.RequiresBlendModeMaterials;

			bool pmaVertexColors = meshGenerator.settings.pmaVertexColors;
			Material[] usedMaterialItems = usedMaterials.Items;
			Texture[] usedTextureItems = usedTextures.Items;
			for (int i = 0; i < submeshCount; i++) {
				SubmeshInstruction submeshInstructionItem = currentInstructions.submeshInstructions.Items[i];
				Material submeshMaterial = submeshInstructionItem.material;
				if (useOriginalTextureAndMaterial) {
					if (submeshMaterial == null) {
						usedMaterialItems[i] = null;
						usedTextureItems[i] = null;
						continue;
					}
					usedTextureItems[i] = submeshMaterial.mainTexture;
					if (!hasBlendModeMaterials) {
						usedMaterialItems[i] = this.materialForRendering;
					} else {
						BlendMode blendMode = blendModeMaterials.BlendModeForMaterial(submeshMaterial);
						Material usedMaterial = this.materialForRendering;
						if (blendMode == BlendMode.Additive && !pmaVertexColors && additiveMaterial) {
							usedMaterial = additiveMaterial;
						} else if (blendMode == BlendMode.Multiply && multiplyMaterial)
							usedMaterial = multiplyMaterial;
						else if (blendMode == BlendMode.Screen && screenMaterial)
							usedMaterial = screenMaterial;
						usedMaterialItems[i] = submeshGraphics[i].GetModifiedMaterial(usedMaterial);
					}
				} else {
					Texture originalTexture = submeshMaterial.mainTexture;
					Material usedMaterial;
					Texture usedTexture;
					if (!customMaterialOverride.TryGetValue(originalTexture, out usedMaterial))
						usedMaterial = material;
					if (!customTextureOverride.TryGetValue(originalTexture, out usedTexture))
						usedTexture = originalTexture;

					usedMaterialItems[i] = submeshGraphics[i].GetModifiedMaterial(usedMaterial);
					usedTextureItems[i] = usedTexture;
				}
			}
		}


        [DllImport(Spine.Unity.SpineUnityLibName.SpineLibName)]
        unsafe static extern void spine_mesh_generator_build_mesh_get_buffer_color32_unity(IntPtr generatorHandle, bool updateTriangles, float meshScale,
float* outVertex, float* outUv, UInt32* outColor, float[] outBounds, int[] outTriangles, bool calcTangent, float* outTangent);


        protected void UpdateMeshMultipleCanvasRenderersNative(SkeletonRendererInstruction currentInstructions)
        {
            meshScale = (canvas == null) ? 100 : canvas.referencePixelsPerUnit;
            if (layoutScaleMode != LayoutMode.None)
            {
                meshScale *= referenceScale;
                if (!EditReferenceRect)
                    meshScale *= GetLayoutScale(layoutScaleMode);
            }
            // Generate meshes.
            int submeshCount = currentInstructions.submeshInstructions.Count;
            Mesh[] meshesItems = meshes.Items;
            bool useOriginalTextureAndMaterial = (customMaterialOverride.Count == 0 && customTextureOverride.Count == 0);

            BlendModeMaterials blendModeMaterials = skeletonDataAsset.blendModeMaterials;
            bool hasBlendModeMaterials = blendModeMaterials.RequiresBlendModeMaterials;
#if HAS_CULL_TRANSPARENT_MESH
            bool mainCullTransparentMesh = this.canvasRenderer.cullTransparentMesh;
#endif
            bool pmaVertexColors = meshGenerator.settings.pmaVertexColors;
            Material[] usedMaterialItems = usedMaterials.Items;
            Texture[] usedTextureItems = usedTextures.Items;
            for (int i = 0; i < submeshCount; i++)
            {
                SubmeshInstruction submeshInstructionItem = currentInstructions.submeshInstructions.Items[i];

				//            meshGenerator.Begin();
				//            meshGenerator.AddSubmesh(submeshInstructionItem);

				//Mesh targetMesh = meshesItems[i];
				//meshGenerator.ScaleVertexData(meshScale);
				//if (OnPostProcessVertices != null) OnPostProcessVertices.Invoke(this.meshGenerator.Buffers);
				//meshGenerator.FillVertexData(targetMesh);
				//meshGenerator.FillTriangles(targetMesh);
				//meshGenerator.FillLateVertexData(targetMesh);


				Mesh targetMesh = meshesItems[i];
				meshGenerator.ClearNativeFb(currentInstructions.submeshInstructions.Items[i].rawVertexCount, currentInstructions.submeshInstructions.Items[i].rawTriangleCount);
				meshGenerator.BuildMeshIndexAndGetMeshFromNative(targetMesh, i, true, meshScale, currentInstructions);

				CanvasRenderer canvasRenderer = canvasRenderers[i];
				if (assignMeshOverrideSingle == null || !disableMeshAssignmentOnOverride)
					canvasRenderer.SetMesh(targetMesh);
				else
					canvasRenderer.SetMesh(null);

				SkeletonSubmeshGraphic submeshGraphic = submeshGraphics[i];
                if (useOriginalTextureAndMaterial && hasBlendModeMaterials)
                {
                    bool allowCullTransparentMesh = true;
                    BlendMode materialBlendMode = blendModeMaterials.BlendModeForMaterial(usedMaterialItems[i]);
                    if ((materialBlendMode == BlendMode.Normal && submeshInstructionItem.hasPMAAdditiveSlot) ||
                        (materialBlendMode == BlendMode.Additive && pmaVertexColors))
                    {
                        allowCullTransparentMesh = false;
                    }
#if HAS_CULL_TRANSPARENT_MESH
                    canvasRenderer.cullTransparentMesh = allowCullTransparentMesh ?
                        mainCullTransparentMesh : false;
#endif
                }
                canvasRenderer.materialCount = 1;
            }

#if SPINE_OPTIONAL_ON_DEMAND_LOADING
            if (Application.isPlaying)
                HandleOnDemandLoading();
#endif
            bool assignAtCanvasRenderer = (assignMeshOverrideSingle == null || !disableMeshAssignmentOnOverride);
            if (assignAtCanvasRenderer)
            {
                for (int i = 0; i < submeshCount; i++)
                {
                    CanvasRenderer canvasRenderer = canvasRenderers[i];
                    canvasRenderer.SetMaterial(usedMaterialItems[i], usedTextureItems[i]);
                }
            }

            if (assignMeshOverrideMultiple != null)
                assignMeshOverrideMultiple(submeshCount, meshesItems, usedMaterialItems, usedTextureItems);
        }

        protected void UpdateMeshMultipleCanvasRenderers (SkeletonRendererInstruction currentInstructions) {
			meshScale = (canvas == null) ? 100 : canvas.referencePixelsPerUnit;
			if (layoutScaleMode != LayoutMode.None) {
				meshScale *= referenceScale;
				if (!EditReferenceRect)
					meshScale *= GetLayoutScale(layoutScaleMode);
			}
			// Generate meshes.
			int submeshCount = currentInstructions.submeshInstructions.Count;
			Mesh[] meshesItems = meshes.Items;
			bool useOriginalTextureAndMaterial = (customMaterialOverride.Count == 0 && customTextureOverride.Count == 0);

			BlendModeMaterials blendModeMaterials = skeletonDataAsset.blendModeMaterials;
			bool hasBlendModeMaterials = blendModeMaterials.RequiresBlendModeMaterials;
#if HAS_CULL_TRANSPARENT_MESH
			bool mainCullTransparentMesh = this.canvasRenderer.cullTransparentMesh;
#endif
			bool pmaVertexColors = meshGenerator.settings.pmaVertexColors;
			Material[] usedMaterialItems = usedMaterials.Items;
			Texture[] usedTextureItems = usedTextures.Items;
			for (int i = 0; i < submeshCount; i++) {
				SubmeshInstruction submeshInstructionItem = currentInstructions.submeshInstructions.Items[i];
				meshGenerator.Begin();
				meshGenerator.AddSubmesh(submeshInstructionItem);

				Mesh targetMesh = meshesItems[i];
				meshGenerator.ScaleVertexData(meshScale);
				if (OnPostProcessVertices != null) OnPostProcessVertices.Invoke(this.meshGenerator.Buffers);
				meshGenerator.FillVertexData(targetMesh);
				meshGenerator.FillTriangles(targetMesh);
				meshGenerator.FillLateVertexData(targetMesh);

				CanvasRenderer canvasRenderer = canvasRenderers[i];
				if (assignMeshOverrideSingle == null || !disableMeshAssignmentOnOverride)
					canvasRenderer.SetMesh(targetMesh);
				else
					canvasRenderer.SetMesh(null);

				SkeletonSubmeshGraphic submeshGraphic = submeshGraphics[i];
				if (useOriginalTextureAndMaterial && hasBlendModeMaterials) {
					bool allowCullTransparentMesh = true;
					BlendMode materialBlendMode = blendModeMaterials.BlendModeForMaterial(usedMaterialItems[i]);
					if ((materialBlendMode == BlendMode.Normal && submeshInstructionItem.hasPMAAdditiveSlot) ||
						(materialBlendMode == BlendMode.Additive && pmaVertexColors)) {
						allowCullTransparentMesh = false;
					}
#if HAS_CULL_TRANSPARENT_MESH
					canvasRenderer.cullTransparentMesh = allowCullTransparentMesh ?
						mainCullTransparentMesh : false;
#endif
				}
				canvasRenderer.materialCount = 1;
			}

#if SPINE_OPTIONAL_ON_DEMAND_LOADING
			if (Application.isPlaying)
				HandleOnDemandLoading();
#endif
			bool assignAtCanvasRenderer = (assignMeshOverrideSingle == null || !disableMeshAssignmentOnOverride);
			if (assignAtCanvasRenderer) {
				for (int i = 0; i < submeshCount; i++) {
					CanvasRenderer canvasRenderer = canvasRenderers[i];
					canvasRenderer.SetMaterial(usedMaterialItems[i], usedTextureItems[i]);
				}
			}

			if (assignMeshOverrideMultiple != null)
				assignMeshOverrideMultiple(submeshCount, meshesItems, usedMaterialItems, usedTextureItems);
		}

#if SPINE_OPTIONAL_ON_DEMAND_LOADING
		void HandleOnDemandLoading () {
			foreach (AtlasAssetBase atlasAsset in skeletonDataAsset.atlasAssets) {
				if (atlasAsset.TextureLoadingMode != AtlasAssetBase.LoadingMode.Normal) {
					atlasAsset.BeginCustomTextureLoading();

					if (!this.allowMultipleCanvasRenderers) {
						Texture loadedTexture = null;
						atlasAsset.RequireTextureLoaded(this.mainTexture, ref loadedTexture, null);
						if (loadedTexture)
							this.baseTexture = loadedTexture;
					} else {
						Texture[] textureItems = usedTextures.Items;
						for (int i = 0, count = usedTextures.Count; i < count; ++i) {
							Texture loadedTexture = null;
							atlasAsset.RequireTextureLoaded(textureItems[i], ref loadedTexture, null);
							if (loadedTexture)
								usedTextures.Items[i] = loadedTexture;
						}
					}
					atlasAsset.EndCustomTextureLoading();
				}
			}
		}
#endif

		protected void EnsureCanvasRendererCount (int targetCount) {
#if UNITY_EDITOR
			RemoveNullCanvasRenderers();
#endif
			int currentCount = canvasRenderers.Count;
			for (int i = currentCount; i < targetCount; ++i) {
				GameObject go = new GameObject(string.Format("Renderer{0}", i), typeof(RectTransform));
				go.transform.SetParent(this.transform, false);
				go.transform.localPosition = Vector3.zero;
				CanvasRenderer canvasRenderer = go.AddComponent<CanvasRenderer>();
				canvasRenderers.Add(canvasRenderer);
				SkeletonSubmeshGraphic submeshGraphic = go.AddComponent<SkeletonSubmeshGraphic>();
				submeshGraphic.maskable = this.maskable;
				submeshGraphic.raycastTarget = false;
				submeshGraphic.rectTransform.pivot = rectTransform.pivot;
				submeshGraphic.rectTransform.anchorMin = Vector2.zero;
				submeshGraphic.rectTransform.anchorMax = Vector2.one;
				submeshGraphic.rectTransform.sizeDelta = Vector2.zero;
				submeshGraphics.Add(submeshGraphic);
			}
		}

		protected void PrepareRendererGameObjects (SkeletonRendererInstruction currentInstructions,
			bool isInRebuild = false) {

			int submeshCount = currentInstructions.submeshInstructions.Count;
			DisableUnusedCanvasRenderers(usedCount: submeshCount, isInRebuild: isInRebuild);

			Transform parent = this.separatorParts.Count == 0 ? this.transform : this.separatorParts[0];
			if (updateSeparatorPartLocation) {
				for (int p = 0; p < this.separatorParts.Count; ++p) {
					Transform separatorPart = separatorParts[p];
					if (separatorPart == null) continue;
					separatorPart.position = this.transform.position;
					separatorPart.rotation = this.transform.rotation;
				}
			}
			if (updateSeparatorPartScale) {
				Vector3 targetScale = this.transform.lossyScale;
				for (int p = 0; p < this.separatorParts.Count; ++p) {
					Transform separatorPart = separatorParts[p];
					if (separatorPart == null) continue;
					Transform partParent = separatorPart.parent;
					Vector3 parentScale = partParent == null ? Vector3.one : partParent.lossyScale;
					separatorPart.localScale = new Vector3(
						parentScale.x == 0f ? 1f : targetScale.x / parentScale.x,
						parentScale.y == 0f ? 1f : targetScale.y / parentScale.y,
						parentScale.z == 0f ? 1f : targetScale.z / parentScale.z);
				}
			}

			int separatorSlotGroupIndex = 0;
			int targetSiblingIndex = 0;
			for (int i = 0; i < submeshCount; i++) {
				CanvasRenderer canvasRenderer = canvasRenderers[i];
				if (canvasRenderer != null) {
					if (i >= usedRenderersCount)
						canvasRenderer.gameObject.SetActive(true);

					if (canvasRenderer.transform.parent != parent.transform && !isInRebuild)
						canvasRenderer.transform.SetParent(parent.transform, false);

					canvasRenderer.transform.SetSiblingIndex(targetSiblingIndex++);
				}

				SkeletonSubmeshGraphic submeshGraphic = submeshGraphics[i];
				if (submeshGraphic != null) {
					RectTransform dstTransform = submeshGraphic.rectTransform;
					dstTransform.localPosition = Vector3.zero;
					dstTransform.pivot = rectTransform.pivot;
					dstTransform.anchorMin = Vector2.zero;
					dstTransform.anchorMax = Vector2.one;
					dstTransform.sizeDelta = Vector2.zero;
				}

				SubmeshInstruction submeshInstructionItem = currentInstructions.submeshInstructions.Items[i];
				if (submeshInstructionItem.forceSeparate) {
					targetSiblingIndex = 0;
					parent = separatorParts[++separatorSlotGroupIndex];
				}
			}
			usedRenderersCount = submeshCount;
		}

		protected void DisableUnusedCanvasRenderers (int usedCount, bool isInRebuild = false) {
#if UNITY_EDITOR
			RemoveNullCanvasRenderers();
#endif
			for (int i = usedCount; i < canvasRenderers.Count; i++) {
				canvasRenderers[i].Clear();
				if (!isInRebuild) // rebuild does not allow disabling Graphic and thus removing it from rebuild list.
					canvasRenderers[i].gameObject.SetActive(false);
			}
		}

#if UNITY_EDITOR
		private void RemoveNullCanvasRenderers () {
			if (Application.isEditor && !Application.isPlaying) {
				for (int i = canvasRenderers.Count - 1; i >= 0; --i) {
					if (canvasRenderers[i] == null) {
						canvasRenderers.RemoveAt(i);
						submeshGraphics.RemoveAt(i);
					}
				}
			}
		}

		private void DestroyOldRawImages () {
			foreach (CanvasRenderer canvasRenderer in canvasRenderers) {
				RawImage oldRawImage = canvasRenderer.GetComponent<RawImage>();
				if (oldRawImage != null) {
					DestroyImmediate(oldRawImage);
				}
			}
		}
#endif

		protected void EnsureMeshesCount (int targetCount) {
			int oldCount = meshes.Count;
			meshes.EnsureCapacity(targetCount);
			for (int i = oldCount; i < targetCount; i++)
				meshes.Add(SpineMesh.NewSkeletonMesh());
		}

		protected void EnsureUsedTexturesAndMaterialsCount (int targetCount) {
			int oldCount = usedMaterials.Count;
			usedMaterials.EnsureCapacity(targetCount);
			usedTextures.EnsureCapacity(targetCount);
			for (int i = oldCount; i < targetCount; i++) {
				usedMaterials.Add(null);
				usedTextures.Add(null);
			}
		}

		protected void DestroyMeshes () {
			foreach (Mesh mesh in meshes) {
#if UNITY_EDITOR
				if (Application.isEditor && !Application.isPlaying)
					UnityEngine.Object.DestroyImmediate(mesh);
				else
					UnityEngine.Object.Destroy(mesh);
#else
				UnityEngine.Object.Destroy(mesh);
#endif
			}
			meshes.Clear();
		}

		protected void EnsureSeparatorPartCount () {
#if UNITY_EDITOR
			RemoveNullSeparatorParts();
#endif
			int targetCount = separatorSlots.Count + 1;
			if (targetCount == 1)
				return;

#if UNITY_EDITOR
			if (Application.isEditor && !Application.isPlaying) {
				for (int i = separatorParts.Count - 1; i >= 0; --i) {
					if (separatorParts[i] == null) {
						separatorParts.RemoveAt(i);
					}
				}
			}
#endif
			int currentCount = separatorParts.Count;
			for (int i = currentCount; i < targetCount; ++i) {
				GameObject go = new GameObject(string.Format("{0}[{1}]", SeparatorPartGameObjectName, i), typeof(RectTransform));
				go.transform.SetParent(this.transform, false);

				RectTransform dstTransform = go.transform.GetComponent<RectTransform>();
				dstTransform.localPosition = Vector3.zero;
				dstTransform.pivot = rectTransform.pivot;
				dstTransform.anchorMin = Vector2.zero;
				dstTransform.anchorMax = Vector2.one;
				dstTransform.sizeDelta = Vector2.zero;

				separatorParts.Add(go.transform);
			}
		}

		protected void UpdateSeparatorPartParents () {
			int usedCount = separatorSlots.Count + 1;
			if (usedCount == 1) {
				usedCount = 0; // placed directly at the SkeletonGraphic parent
				for (int i = 0; i < canvasRenderers.Count; ++i) {
					CanvasRenderer canvasRenderer = canvasRenderers[i];
					if (canvasRenderer.transform.parent.name.Contains(SeparatorPartGameObjectName)) {
						canvasRenderer.transform.SetParent(this.transform, false);
						canvasRenderer.transform.localPosition = Vector3.zero;
					}
				}
			}
			for (int i = 0; i < separatorParts.Count; ++i) {
				bool isUsed = i < usedCount;
				separatorParts[i].gameObject.SetActive(isUsed);
			}
		}

#if UNITY_EDITOR
		private void RemoveNullSeparatorParts () {
			if (Application.isEditor && !Application.isPlaying) {
				for (int i = separatorParts.Count - 1; i >= 0; --i) {
					if (separatorParts[i] == null) {
						separatorParts.RemoveAt(i);
					}
				}
			}
		}

		protected void InitLayoutScaleParameters () {
			previousLayoutScaleMode = layoutScaleMode;
		}

		protected void UpdateReferenceRectSizes () {
			if (rectTransformSize == Vector2.zero)
				rectTransformSize = GetCurrentRectSize();

			HandleChangedEditReferenceRect();

			if (layoutScaleMode != previousLayoutScaleMode) {
				if (layoutScaleMode != LayoutMode.None) {
					SetRectTransformSize(this, rectTransformSize);
				} else {
					rectTransformSize = referenceSize / referenceScale;
					referenceScale = 1f;
					SetRectTransformSize(this, rectTransformSize);
				}
			}
			if (editReferenceRect || layoutScaleMode == LayoutMode.None) {
				referenceSize = GetCurrentRectSize();
			}
			previousLayoutScaleMode = layoutScaleMode;
		}

		protected void HandleChangedEditReferenceRect () {
			if (editReferenceRect == previousEditReferenceRect) return;
			previousEditReferenceRect = editReferenceRect;

			if (editReferenceRect) {
				rectTransformSize = GetCurrentRectSize();
				ResetRectToReferenceRectSize();
			} else {
				SetRectTransformSize(this, rectTransformSize);
			}
		}

		public void ResetRectToReferenceRectSize () {
			referenceScale = referenceScale * GetLayoutScale(previousLayoutScaleMode);
			float referenceAspect = referenceSize.x / referenceSize.y;
			Vector2 newSize = GetCurrentRectSize();

			LayoutMode mode = previousLayoutScaleMode;
			float frameAspect = newSize.x / newSize.y;
			if (mode == LayoutMode.FitInParent)
				mode = frameAspect > referenceAspect ? LayoutMode.HeightControlsWidth : LayoutMode.WidthControlsHeight;
			else if (mode == LayoutMode.EnvelopeParent)
				mode = frameAspect > referenceAspect ? LayoutMode.WidthControlsHeight : LayoutMode.HeightControlsWidth;

			if (mode == LayoutMode.WidthControlsHeight)
				newSize.y = newSize.x / referenceAspect;
			else if (mode == LayoutMode.HeightControlsWidth)
				newSize.x = newSize.y * referenceAspect;
			SetRectTransformSize(this, newSize);
		}

		public Vector2 GetReferenceRectSize () {
			return referenceSize * GetLayoutScale(layoutScaleMode);
		}
#endif

		protected float GetLayoutScale (LayoutMode mode) {
			Vector2 currentSize = GetCurrentRectSize();
			float referenceAspect = referenceSize.x / referenceSize.y;
			float frameAspect = currentSize.x / currentSize.y;
			if (mode == LayoutMode.FitInParent)
				mode = frameAspect > referenceAspect ? LayoutMode.HeightControlsWidth : LayoutMode.WidthControlsHeight;
			else if (mode == LayoutMode.EnvelopeParent)
				mode = frameAspect > referenceAspect ? LayoutMode.WidthControlsHeight : LayoutMode.HeightControlsWidth;

			if (mode == LayoutMode.WidthControlsHeight) {
				return currentSize.x / referenceSize.x;
			} else if (mode == LayoutMode.HeightControlsWidth) {
				return currentSize.y / referenceSize.y;
			}
			return 1f;
		}

		private Vector2 GetCurrentRectSize () {
			return this.rectTransform.rect.size;
		}
	}
}

