using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameObjectsBase
{
    public static class GameObjectsExtensions
    {
        public static GameObject FindObjectById(this GameObject sourceObject, ObjectId tag) => sourceObject.Root.FindObjectById(tag);

        public static GameObject FindObjectById(this GameObject sourceObject, int tag) => sourceObject.Root.FindObjectById(tag);

        public static GameObject FindObjectOfType<T>(this GameObject sourceObject) where T : Component => sourceObject.Root.FindObjectOfType<T>();

        public static GameObject FindObjectByName(this GameObject sourceObject, string name) => sourceObject.Root.FindObjectByName(name);

        public static bool TryFindObjectByName(this GameObject sourceObject, string name, out GameObject outObj)
        {
            outObj = FindObjectByName(sourceObject, name);
            return outObj != null;
        }

        public static bool TryFindObjectById(this GameObject sourceObject, ObjectId id, out GameObject outObj)
        {
            outObj = FindObjectById(sourceObject, id);
            return outObj != null;
        }

        public static bool TryFindObjectOfType<T>(this GameObject sourceObject, out GameObject resutlObject) where T : Component
        {
            resutlObject = FindObjectOfType<T>(sourceObject);
            return resutlObject != null;
        }

        public static void Destroy(this GameObject sourceObject, GameObject targetObject) => sourceObject.Root.DestroyObject(targetObject);
    }
}
