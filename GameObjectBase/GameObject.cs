using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;

namespace GameObjectsBase
{
    public class GameObject
    {
        private ObjectId _id;
        private List<Component> _components = new List<Component>();

        private GameObject _parent = null;
        private List<GameObject> _children = new List<GameObject>();

        private bool _isActive;
        private string _name = string.Empty;

        private ObjectsHolder _root;



        public ObjectsHolder Root => _root;
        public bool IsActive => _isActive;
        public string Name => _name;
        public ObjectId Id => _id;
        public GameObject Parent => _parent;

        public GameObject(ObjectId id, ObjectsHolder root, string name)
        {
            _id = id;
            _root = root;
            _name = name;
        }

        public ref string NameRef => ref _name ;

        public void SetName(string name) => _name = name;

        public void OnEnable() 
        {
            _components.ForEach(x => x.Init(this));
            _children.ForEach(x => x.OnEnable());
        } 
        public void OnDisable() 
        {
            _components.ForEach(x => x.OnDisable());
            _children.ForEach(x => x.OnDisable());
        } 

        public void Start()
        {
            _components.ForEach(x => x.Start());
            _children.ForEach(x => x.Start());
        } 

        
        public void Update() 
        {
            //Console.WriteLine("Update object with id " +  _id.Value);
            _components.ForEach(x => x.Update()); 
        }
        public void FixedUpdate() => _components.ForEach(x => x.FixedUpdate());

        public void OnDestroy()
        {
            _components.ForEach(x => x.OnDestroy());

            _components.Clear();
        }

        public List<Component> GetComponents() => _components;

        public void AddChild(GameObject child) 
        {
            child.SetParent(this);
            _children.Add(child);
        } 

        public void SetParent(GameObject parent) => _parent = parent;

        public void AddComponent(Component component)
        { 
            //var component = Activator.CreateInstance(type) as Component;

            if (_components.Any(x => x == component))
            {
                Console.WriteLine($"Cant add same component as {component}");
                return;
            }
            Console.WriteLine($"We can add same component as {component}");
            component.Init(this);
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

        private Type GetBaseType(Type type)
        {
            var derived = type;
            do
            {
                if (derived.BaseType == null)
                {
                    return derived;
                }
                derived = derived.BaseType;
            }
            while (derived != null);

            return derived;
        }


    }
}
