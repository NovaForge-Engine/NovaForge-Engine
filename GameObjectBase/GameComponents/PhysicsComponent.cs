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
                if (InputManager.IsKeyDown((int)KeyboardSource.KEY_W))
                {
                    vec3 pos = comp.Position;
                    pos += new vec3(0, 0, 0.005f);
                    comp.SetPosition(pos);
                }
                if (InputManager.IsKeyDown((int)KeyboardSource.KEY_S))
                {
                    vec3 pos = comp.Position;
                    pos -= new vec3(0, 0, 0.005f);
                    comp.SetPosition(pos);
                }
                if (InputManager.IsKeyDown((int)KeyboardSource.KEY_A))
                {
                    vec3 pos = comp.Position;
                    pos -= new vec3(0.005f, 0,0);
                    comp.SetPosition(pos);
                }
                if (InputManager.IsKeyDown((int)KeyboardSource.KEY_D))
                {
                    vec3 pos = comp.Position;
                    pos += new vec3(0.005f, 0, 0);
                    comp.SetPosition(pos);
                }
            }
        }
    }
}
