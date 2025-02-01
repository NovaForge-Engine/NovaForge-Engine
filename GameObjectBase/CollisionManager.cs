using GameObjectBase.GameComponents;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameObjectBase
{
    class CollisionManager
    {
        private List<ColliderComponent> colliders = new List<ColliderComponent>();

        public CollisionManager() {
            Console.WriteLine("CollisionManager constructor");
        }

        public void RegisterCollider(ColliderComponent collider)
        {
            if (!colliders.Contains(collider))
            {
                Console.WriteLine("Register collider");
                colliders.Add(collider);
            }
        }

        public void UnregisterCollider(ColliderComponent collider)
        {
            colliders.Remove(collider);
        }

        public void Update()
        {
            for (int i = 0; i < colliders.Count; i++)
            {
                for (int j = i + 1; j < colliders.Count; j++)
                {
                    ColliderComponent a = colliders[i];
                    ColliderComponent b = colliders[j];

                    if (a.Intersects(b))
                    {
                        a.OnCollision(b);
                        if (b != null) b.OnCollision(a);

                        if(a.parent.Name == "player" && b.parent.Name == "monster")
                        {

                            MovementController movementController = b.parent.GetComponent<MovementController>();
                            movementController.Respawn();
                        }

                        //Console.WriteLine($"Collision detected: {a.parent.Name} <-> {b.parent.Name}");
                    }
                }
            }
        }
    }
}
