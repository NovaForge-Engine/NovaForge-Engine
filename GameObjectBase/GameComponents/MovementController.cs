using GameObjectsBase;
using GlmSharp;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameObjectBase.GameComponents
{
    internal class MovementController : Component
    {
        public TransformComponent target;
        float speed = 0.001f;
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
            MoveToTarget();
        }

        void MoveToTarget()
        {
            
            if (target == null)
            {
                Console.WriteLine("MoveToTarget");
                return;
            }
            TransformComponent comp = parent.GetComponent<TransformComponent>();
            vec3 directionToMove = target.Position - comp.Position;
            float distanceToTarget = directionToMove.Length;

            if (distanceToTarget > 0.1f)
            {
                
                vec3 moveDirection = directionToMove.Normalized;
                comp.SetPosition(comp.Position + moveDirection * speed);

                // Rotate towards the target
                GlmSharp.quat targetRotation = MathTypes.LookAtLH(-directionToMove, vec3.UnitY);
                comp.SetRotation(targetRotation);
            }
        }
    }
}
