using GameObjectsBase;
using GlmSharp;
using SharpDX;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameObjectBase.GameComponents {
    
    internal class HealthComponent : Component {
        static float maxHealth = 100.0f;
        float health = maxHealth;
        private bool flag = false;
        public override void FixedUpdate() {
		// None
        }

        public override void OnDestroy() {
		// None
        }

        public override void OnDisable() {
		// None
        }

        public override void OnEnable() {
		// None
        }

        public override void Start()
        {         
            var collider = parent.GetComponent<ColliderComponent>();
            if (collider != null)
            {
                collider.OnCollisionEvent += OnCollision;
            }
        }

        private void OnCollision(ColliderComponent col)
        {
            if (col.parent.Name != "monster") return;

            Damage(0.001f);
            MovementController movementController = col.parent.GetComponent<MovementController>();
            movementController.Respawn();
        }

        public override void Update() {
		// None
        }

        public void OverrideHealth(float health) {
            this.health = health;
            if (health < 0) { this.health = 0; Damage(0); };
            if (health > maxHealth) { this.health = maxHealth; };
        }

        public void Damage(float damage) {
            Console.WriteLine("Health: " + health);
            health -= damage;
            if (health <= 0) { 
                health = 0;
                InternalCalls.Close();
            }
        }

        public float GetHealth() {
            return health;
        }

        public float GetMaxHealth() {
            return maxHealth;
        }
    }
}
