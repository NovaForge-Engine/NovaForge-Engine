﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Xml.Linq;

namespace GameObjectsBase
{
    public class ObjectsHolder
    {
        private const string NewGameObjectName = "GameObject";

        private List<GameObject> _objects = new List<GameObject>();
        private ObjectId _lastObjectId = ObjectId.Zero;

        public Action<GameObject> OnObjectAdd;
        public Action<GameObject> OnObjectRemove;
        public Action<GameObject> OnObjectUpdate;

        public List<GameObject> GetAllGameObjects()
        {
            return _objects;
        }

        public void Update()
        {
            foreach (var item in _objects.Where(x => x.IsActive))
            {
                item.Update();
            }
        }

        public void FixedUpdate()
        {
            foreach (var item in _objects.Where(x => x.IsActive))
            {
                item.FixedUpdate();
            }
        }

        public GameObject FindObjectById(ObjectId tag) => FindObjectById(tag.Value);

        public GameObject FindObjectById(int tag)
        {
            foreach (GameObject obj in _objects)
            {
                if (obj.Id.Value == tag)
                {
                    return obj;
                }
            }

            return null;
        }

        public GameObject FindObjectOfType<T>() where T : Component
        {
            foreach(GameObject obj in _objects)
            {
                if (obj.ContainsComponent<T>()) return obj;
            }

            return null;
        }

        public GameObject FindObjectByName(string name)
        {
            foreach (GameObject obj in _objects)
            {
                if (obj.Name == name) return obj;
            }

            return null;
        }

        public bool TryFindObjectByName(string name, out GameObject resultObject)
        {
            resultObject = FindObjectByName(name);
            return resultObject != null;
        }

        public bool TryFindObjectById(ObjectId id, out GameObject resultObject)
        {
            resultObject = FindObjectById(id);
            return resultObject != null;
        }

        public bool TryFindObjectOfType<T>(out GameObject resultObject) where T : Component
        {
            resultObject = FindObjectOfType<T>();
            return resultObject != null;
        }

        public void AddNewObject()
        {
            _lastObjectId = _lastObjectId.Next();
            GameObject obj = new GameObject(_lastObjectId, this, NewGameObjectName + _lastObjectId.Value);

            _objects.Add(obj);

            obj.Enable();
            obj.Start();
            
            OnObjectAdd?.Invoke(obj);
        }

        public void DestroyObject(GameObject obj)
        {
            obj.Disable();
            obj.OnDestroy();

            _objects.Remove(obj);
            OnObjectRemove?.Invoke(obj);
        }
    }
}
