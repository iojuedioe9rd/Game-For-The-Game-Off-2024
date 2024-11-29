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
        private float gun_Timer = 0.0f;
        public virtual string GetTexName() { return "assets/textures/Checkerboard.png"; }

        public virtual void Shoot(Vector2 dir, float dis)
        {

        }

        public virtual bool CanShoot() { return Input.IsMouseButtonPressed(MouseCode.ButtonLeft) && gun_Timer >= 0.5f; }

        public virtual void Update(float ts) 
        {
            gun_Timer += ts;
        }

    }
}
