using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Vertex;

namespace Sandbox
{
    public class Gun: Vertex.Object
    {
        Player player;
        private float gun_Timer = 0.0f;
        public virtual string GetTexName() { return "assets/textures/Gun.png"; }

        public virtual void Shoot(Vector2 dir, float disGunToPlayer)
        {
            gun_Timer = 0.0f;
            Logger.Info("Player has Shooted. Player pos: ", player.Pos, "ID: ", UUID);

            Vector2 gunPos = dir * disGunToPlayer;

            Bullet bullet = Entity.NewEntity<Bullet>("Bullet", player.Pos + new Vector3(gunPos, 0), Vector3.One, Vector3.Zero);
            bullet.dir = new Vector3(dir, 0);
        }

        public Gun(Player player)
        {
            this.player = player;
        }

        public virtual bool CanShoot() { return Input.IsMouseButtonPressed(MouseCode.ButtonLeft) && gun_Timer >= 0.5f; }

        public virtual void Update(float ts) 
        {
            gun_Timer += ts;
        }

    }
}
