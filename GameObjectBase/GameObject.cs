using System.Collections.Generic;

namespace GameObjectsBase
{
    public class GameObject
    {
        private ObjectId _id;
        private List<Component> _components = new List<Component>();
        private bool _isActive;
        private string _name = string.Empty;

        private SystemRoot _root;

        public SystemRoot Root => _root;
        public bool IsActive => _isActive;
        public string Name => _name;
        public ObjectId Id => _id;

        public GameObject(ObjectId id, SystemRoot root, string name)
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
