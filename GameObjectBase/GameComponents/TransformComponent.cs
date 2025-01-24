using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using GameObjectsBase;
using GlmSharp;

namespace GameObjectBase.GameComponents
{
    public class TransformComponent : Component
    {
        [SerializableField] private vec3 position;
        [SerializableField] private quat rotation;
        [SerializableField] private vec3 scale;

        public vec3 Position { get => position; set => position = value; }
        public quat Rotation { get => rotation; set => rotation = value; }
        public vec3 Scale { get => scale; set => scale = value; }

        public TransformComponent()
        {
            position = new vec3(0, 0, 0);
            rotation = new quat(0, 0, 0, 1);
            scale = new vec3(1, 1, 1);
        }

        public override void FixedUpdate()
        {
            throw new NotImplementedException();
        }

        public override void OnDestroy()
        {
            return;
            throw new NotImplementedException();
        }

        public override void OnDisable()
        {
            return;
            throw new NotImplementedException();
        }

        public override void OnEnable()
        {
            return;
            throw new NotImplementedException();
        }

        public override void Start()
        {
            return;
            throw new NotImplementedException();
        }

        public override void Update()
        {
            return;



            throw new NotImplementedException();
        }
    }
}
