using System.Collections.Generic;
using System.ComponentModel;

namespace GameObjectsBase
{
    public class GameObject
    {
        private ObjectId _id;
        private List<Component> _components = new List<Component>();
        private List<GameObject> _children = new List<GameObject>();
        private bool _isActive;
        private string _name = string.Empty;

        private ObjectsHolder _root;

        public ObjectsHolder Root => _root;
        public bool IsActive => _isActive;
        public string Name => _name;
        public ObjectId Id => _id;

        public GameObject(ObjectId id, ObjectsHolder root, string name)
        {
            _id = id;
            _root = root;
            _name = name;
        }

        public void OnEnable() => _components.ForEach(x => x.OnEnable());
        public void OnDisable() => _components.ForEach(x => x.OnDisable());

        public void Start() => _components.ForEach(x => x.Start());
        public void Update() => _components.ForEach(x => x.Update());
        public void FixedUpdate() => _components.ForEach(x => x.FixedUpdate());

        public void OnDestroy()
        {
            _components.ForEach(x => x.OnDestroy());

            _components.Clear();
        }

        public void AddChild(GameObject child) => _children.Add(child);

        public void AddComponent(Component component)
        {
            _components.Add(component);
        }

        public void RemoveComponent(Component component)
        {
            _components.Remove(component);
        }

        public T GetComponent<T>() where T : Component
        {
            return _components.Find(x => x is T) as T;
        }

        public bool TryGetComponent<T>(out T component) where T : Component
        {
            component = GetComponent<T>();
            return component != null;
        }

        public T GetComponentInChildren<T>() where T : Component
        {
            foreach(var child in _children)
            {
                if (child.ContainsComponent<T>())
                {
                    return child.GetComponent<T>();
                }
            }

            T component = null;
            foreach(var child in _children)
            {
                component = child.GetComponentInChildren<T>();
            }

            return component;
        }

        public bool TryGetComponentInChildren<T>(out T component) where T : Component
        {
            component = GetComponentInChildren<T>();
            return component != null;
        }

        public bool ContainsComponent<T>()
        {
            foreach (var component in _components)
            {
                if (component is T) return true;
            }

            return false;
        }

        public void SetActive(bool active)
        {
            _isActive = active;

            foreach(var child in _children) 
                child.SetActive(active); 

            if (active) 
                OnEnable();
            else 
                OnDisable();
        }

        public void Disable()
        {
            _isActive = false;
            OnDisable();
        }

        public void Enable()
        {
            _isActive = true;
            OnEnable();
        }
    }
}
