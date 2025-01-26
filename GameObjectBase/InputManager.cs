using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameObjectBase
{
   
    public static class InputManager
    {
        private static Dictionary<int, bool> keys;
        private static Dictionary<int, bool> mousekeys;
        public static GlmSharp.vec2 mousePos;
        public static GlmSharp.vec2 prevMousePos;
        public static GlmSharp.vec2 mouseDelta;

        public static void Init()
        {
            keys = new Dictionary<int, bool>();

            foreach(var key in Enum.GetValues(typeof(KeyboardSource))) keys[(int)key] = false;


            mousekeys = new Dictionary<int, bool>();

            foreach (var key in Enum.GetValues(typeof(MouseSource))) keys[(int)key] = false;

        }

        public static void PressKey(string name, int button)
        {
            keys[button] = true;
        }

        public static void ReleaseKey(string name, int button)
        {
            keys[button] = false;
        }

        public static void PressMouseKey(string name, int button)
        {
            mousekeys[button] = true;
        }

        public static void MousePos(GlmSharp.vec2 pos)
        {
            prevMousePos = mousePos;
            mousePos = pos;
            mouseDelta = mousePos - prevMousePos;
        }

        public static bool IsKeyDown(int key)
        {
            return keys[key];
        }

        public static bool IsMouseKeyDown(int key)
        {
            return keys[key];
        }

        public static GlmSharp.vec2 GetMousePos() => mousePos;

        public static GlmSharp.vec2 GetRelativePos() => mouseDelta;



    }
}
