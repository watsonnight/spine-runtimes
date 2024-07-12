
using UnityEngine;


namespace Spine.Unity
{
    public class SpineUnityLibName
    {
        public const string SpineLibNameStatic = "__Internal";

        public const string SpineLibNameDynamic = "spine-cpp-lite";


        public static bool IsStatic
        {
            get
            {
                if (Application.isEditor)
                {
                    return false;
                }
                else
                {
                    if (Application.platform == RuntimePlatform.WebGLPlayer
                        || Application.platform == RuntimePlatform.WeixinMiniGamePlayer
                        )
                    {
                        return true;
                    }
                }

                return false;
            }
        }

#if UNITY_WEBGL
        public const string SpineLibName = SpineLibNameStatic;
#else
        public const string SpineLibName = SpineLibNameDynamic;
#endif
    }
}
