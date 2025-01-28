using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using GameObjectsBase;
using GlmSharp;

namespace GameObjectBase.GameComponents
{
    public class TransformComponent : Component, IAddOnceComponent
    {
        [SerializableField] private vec3 position;
        [SerializableField] private quat rotation;
        [SerializableField] private vec3 scale;

        private mat4 worldMatrix;
        private mat4 InvWorldMartix;

        public vec3 Position { get => position; set => position = value; }
        public quat Rotation { get => rotation; set => rotation = value; }
        public vec3 Scale { get => scale; set => scale = value; }

        public bool isDirty = false;

        public TransformComponent()
        {
            position = new vec3(0, 0, 0);
            rotation = new quat(0, 0, 0, 1);
            scale = new vec3(1, 1, 1);
        }

        public TransformComponent(vec3 position, quat rotation, vec3 scale)
        {
            this.position = position;
            this.rotation = rotation;
            this.scale = scale;
        }

        public void SetPosition(vec3 position)
        {
            this.position = position;
            isDirty = true;
        }
        public void SetRotation(quat rotation)
        {
            this.rotation = rotation;
            isDirty = true;
        }
        public void SetScale(vec3 scale)
        {
            this.scale = scale;
            isDirty = true;
        }

        public mat4 GetWorldMatrix()
        {
            return mat4.Translate(position) * rotation.ToMat4 * mat4.Scale(scale);
        }

        public override void FixedUpdate()
        {
        }

        public override void OnDestroy()
        {
        }

        public override void OnDisable()
        {
        }

        public override void OnEnable()
        {
        }

        public override void Start()
        {
        }

        public override void Update()
        {
            if (isDirty){
                worldMatrix = GetWorldMatrix();
                InternalCalls.GameObject_SetWorldMatrix((ulong)parent.Id.Value, worldMatrix);
                isDirty = false;
            }
        }
    }
}
