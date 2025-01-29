using GameObjectsBase;
using GlmSharp;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameObjectBase.GameComponents
{
    public class PhysicsComponent : Component, IAddOnceComponent
    {
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
             GameObject gameObject;
            if (!GameObjectsExtensions.TryFindObjectByName(this.parent, "player", out gameObject))
            {
                return;
            };
             TransformComponent comp = gameObject.GetComponent<TransformComponent>();
            if(comp != null)
            {
                vec3 rightVector = comp.Rotation * new vec3(1, 0, 0);
                vec3 frontVector = comp.Rotation * new vec3(0, 0, 1);

                vec3 direction = new vec3(0, 0, 0);

                if (InputManager.IsKeyDown((int)KeyboardSource.KEY_W))
                {

                    direction -= frontVector * 0.005f;

                }
                if (InputManager.IsKeyDown((int)KeyboardSource.KEY_S))
                {
                    direction += frontVector * 0.005f;

                }
                
                if (InputManager.IsKeyDown((int)KeyboardSource.KEY_A))
                {
                    quat rot = comp.Rotation;
                    quat rotation = GlmSharp.quat.FromAxisAngle(glm.Radians(0.5f), new vec3(0, 1, 0));
                    rot *= rotation;
                    comp.SetRotation(rot);
                }
                if (InputManager.IsKeyDown((int)KeyboardSource.KEY_D))
                {
                    quat rot = comp.Rotation;
                    quat rotation = GlmSharp.quat.FromAxisAngle(glm.Radians(-0.5f), new vec3(0, 1, 0));
                    rot *= rotation;
                    comp.SetRotation(rot);
                }
                vec3 pos = comp.Position;
                comp.SetPosition(pos + direction);


            }
        }
    }
}
