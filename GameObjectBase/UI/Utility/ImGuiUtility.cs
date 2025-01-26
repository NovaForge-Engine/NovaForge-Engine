using GameObjectsBase;
using static GameObjectBase.Root;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameObjectBase.UI.Utils
{
    public static class ImGuiUtils
    {
        public static Action EmptyVoidFunction() { return () => { }; }
    }

    public class AppState
    {
        public IntPtr outputTexture = new IntPtr(1000);
        public SharpDX.Vector2 textureUV0 = new SharpDX.Vector2(0.0f);
        public SharpDX.Vector2 textureUV1 = new SharpDX.Vector2(1.0f);
        public SharpDX.Vector4 textureTintCol = new SharpDX.Vector4(1.0f);
        public SharpDX.Vector4 textureBorderCol = new SharpDX.Vector4(1.0f);

        public List<GameObject> gameObjects = getAllGameObjects();

        public float assetPadding = 16.0f;
        public float assetThumbnailSize = 120.0f;

        public float panelButtonSize = 64.0f;
        public SharpDX.Vector4 playButtonCol = new SharpDX.Vector4(0.0f, 0.67f, 0.0f, 1.0f);
        public SharpDX.Vector4 stopButtonCol = new SharpDX.Vector4(0.67f, 0.0f, 0.0f, 1.0f);
        public bool isPlayMode = false;
    }
}