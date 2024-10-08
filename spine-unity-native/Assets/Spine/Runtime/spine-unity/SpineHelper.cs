using System.Collections.Generic;
using Spine;

public static class SpineHelper
{
    internal static bool useFrameLimit = true;   //�Ƿ���֡
    internal static float frameTime = 1f / 15f;
    public static int FrameLimit
    {
        set
        {
            useFrameLimit = value > 0;
            if (useFrameLimit)
                frameTime = 1f / (float)value;
        }
    }
    /// <summary>
    /// ignore skeleton we dont want to filter, because its data is unspported currently.
    /// </summary>
    public static readonly HashSet<string> FilterBoneNames = new HashSet<string>()
    {
        "ch_ross_SkeletonData" , "animal_sanxi_SkeletonData" , "tansuoche_SkeletonData" , "ch_ball_SkeletonData",
        "ch_iceboy_SkeletonData" , "ch_boss_gaotashouhuzhe_SkeletonData",
    };

    /// <summary>
    /// get the animation direciton sign
    /// </summary>
    /// <param name="animationName"></param>
    /// <returns></returns>
    public static string GetAnimationDirection(string animationName)
    {
        string result = string.Empty;
        // check _ exists
        int index = animationName.LastIndexOf('_');
        if (index > 0)
        {
            result = animationName[index + 1].ToString();
        }
        return result;
    }

    private static bool IsMatchFilterBoneName(char dirString, string boneName)
    {
        bool result = false;
        if (boneName.Length >= 2 && boneName[1] == '_')
        {
            //char bonePrefix = char.ToLower(boneName[0]);
            char bonePrefix = boneName[0];
            // we disable other 2 side's bone
            // just use quick search like f_ , s_ , b_ c_??
            //result = dirString != bonePrefix && bonePrefix != 'c';
            //Debug.Log($"{bonePrefix}, {dirString}");
            switch (dirString)
            {
                case 'f':
                    result = bonePrefix == 'b' || bonePrefix == 's' || bonePrefix == 'B' || bonePrefix == 'S';
                    break;
                case 'b':
                    result = bonePrefix == 'f' || bonePrefix == 's' || bonePrefix == 'F' || bonePrefix == 'S';
                    break;
                case 's':
                    result = bonePrefix == 'f' || bonePrefix == 'b' || bonePrefix == 'F' || bonePrefix == 'B';
                    break;
            }
        }
        return result;
    }

    /// <summary>
    /// update the filter bones by animation's name
    /// </summary>
    /// <param name="skeleton"></param>
    /// <param name="animationName"></param>
    public static void UpdateSkeletonHideBones(Skeleton skeleton, string animationName)
    {
        // get direction string
        string dirString = GetAnimationDirection(animationName);
        if (!string.IsNullOrWhiteSpace(dirString))
        {
            UpdateSkeletonHideBonesByPrefix(skeleton, dirString);
        }
    }

    /// <summary>
    /// update the filter bones by prefix (direciton)
    /// </summary>
    /// <param name="skeleton"></param>
    /// <param name="prefix">diretion</param>
    public static void UpdateSkeletonHideBonesByPrefix(Skeleton skeleton, string prefix)
    {
        //if (!string.IsNullOrWhiteSpace(prefix))
        //{
        //    // check all bones and set 
        //    int count = skeleton.directionNamedBones.Count;
        //    if (count > 0)
        //    {
        //        BoneData[] bones = skeleton.directionNamedBones.Items;
        //        for (int i = 0; i < count; i++)
        //        {
        //            BoneData bone = bones[i];
        //            //bone.active = !IsMatchFilterBoneName(dirString, bone.data.name);
        //            // force set sort , if it's true we skip during the cache state
        //            bone.skinRequired = IsMatchFilterBoneName(prefix[0], bone.name); ;
        //        }
        //        //skeleton.SetToSetupPose();
        //        // try to reset all slots first
        //        skeleton.SetSlotAttachmentsToSetupPoseWithoutOverride();
        //        skeleton.SetBonesToSetupPose();
        //        skeleton.UpdateCache();
        //        //skeleton.SetToSetupPose();
        //    }
        //}
    }

    /// <summary>
    /// clear the filter bones to full update all bones
    /// </summary>
    /// <param name="skeleton"></param>
    public static void ClearSkeletonHideBones(Skeleton skeleton)
    {
        //int count = skeleton.directionNamedBones.Count;
        //if (count > 0)
        //{
        //    BoneData[] bones = skeleton.directionNamedBones.Items;
        //    for (int i = 0; i < count; i++)
        //    {
        //        BoneData bone = bones[i];
        //        bone.skinRequired = false;
        //    }
        //    skeleton.SetToSetupPose();
        //    skeleton.UpdateCache();
        //}
    }

    public static void ClearSkeletonAllBones(Skeleton skeleton)
    {
        //int count = skeleton.bones.Count;
        //if (count > 0)
        //{
        //    Bone[] bones = skeleton.bones.Items;
        //    for (int i = 0; i < count; i++)
        //    {
        //        BoneData bone = bones[i].Data;
        //        bone.skinRequired = false;
        //    }
        //    ClearSkeletonHideBones(skeleton);
        //}
    }

    public static void SetSlotAttachmentsToSetupPoseWithoutOverride(this Skeleton skeleton)
    {
        //[H5] 注释了没使用的功能
        //var slotsItems = skeleton.slots.Items;
        //for (int i = 0; i < skeleton.slots.Count; i++)
        //{
        //    Slot slot = slotsItems[i];
        //    if (slot.AttachmentOverride != null)
        //    {
        //        continue;
        //    }
        //    string attachmentName = slot.data.attachmentName;
        //    slot.Attachment = string.IsNullOrEmpty(attachmentName) ? null : skeleton.GetAttachment(i, attachmentName);
        //}
    }
}
