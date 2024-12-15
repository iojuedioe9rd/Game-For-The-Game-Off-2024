using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Vertex;

namespace Sandbox
{
    public class Bullet: Entity
    {
        public float speed = 10f;
        public Vector3 dir;

        protected override void OnUpdate(float ts)
        {
            Pos += dir * speed * ts;
            base.OnUpdate(ts);
        }

        protected override void OnDraw()
        {
            Renderer2D.DrawQuad(new Vector3(Pos.XY, 0.1f), Size, "assets/textures/TNT.png", 1, new Colour(1,1,1,1));
        }
    }
}
