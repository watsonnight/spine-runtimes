
using UnityEngine;


namespace Spine.Unity
{
    public class SpineUnityLibName
    {
        public const string SpineLibNameStatic = "__Internal";

        public const string SpineLibNameDynamic = "spine-cpp-lite";

        public const string OSX_SpineLibNameDynamic = "spine-cpp-lite-osx";



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
#if UNITY_EDITOR
    #if UNITY_EDITOR_OSX
            public const string SpineLibName = OSX_SpineLibNameDynamic;
    #else
            public const string SpineLibName = SpineLibNameDynamic;
    #endif
#else
        public const string SpineLibName = SpineLibNameDynamic;
#endif
    }
}
