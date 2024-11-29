using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Vertex;

namespace Sandbox
{
    public class Parallax : Entity
    {
        
        float Time = 0f;
        public Player player;


        public float startPos = -2.0f;
        float length = 12f * 5;
        float parallaxEffectSpeed1 = 1f;
        Vector3 parallaxEffectPos1 = Vector3.Zero;

        float parallaxEffectSpeed2 = 0.7f;
        Vector3 parallaxEffectPos2 = Vector3.Zero;

        protected override void OnCreate()
        {
            player = FindEntityByName("Player").As<Player>();
            base.OnCreate();
        }

        void ParallaxEffect1()
        {
            float temp = (player.Pos.X * (1 - parallaxEffectSpeed1));
            float dist = (player.Pos.X * parallaxEffectSpeed1);

            parallaxEffectPos1 = new Vector3((startPos / 5 - 32) + dist, parallaxEffectPos1.Y, 0);

            if (temp > startPos + (length / 100)) startPos += length / 100;
            else if(temp < startPos - (length / 100))
            {
                startPos -= length / 100;
            }
        }

        void ParallaxEffect2()
        {
            float temp = (player.Pos.X * (1 - parallaxEffectSpeed2));
            float dist = (player.Pos.X * parallaxEffectSpeed2);

            parallaxEffectPos2 = new Vector3(startPos + dist, parallaxEffectPos2.X, 0);

            if (temp > startPos + length) startPos += length;
            else if (temp < startPos - length)
            {
                startPos -= length;
            }
        }

        protected override void OnUpdate(float ts)
        {
            Time += ts;
            base.OnUpdate(ts);

            //ParallaxEffect1();
            //ParallaxEffect2();
        }

        protected override void OnDraw()
        {
            Renderer2D.DrawQuad(new Vector3(Pos.XY, 3 * -100), new Vector3(1000, 1000, 1), "Assets/textures/bg1_2.png", 100f, new Colour(1, 1, 1, 1));

            for (int i = 0; i < 50 / 3; i++)
            {
                Vector2 XY = new Vector2(parallaxEffectPos1.X - 50 + (i * length), parallaxEffectPos1.Y);
                Renderer2D.DrawQuad(new Vector3(XY, 1 * -100), new Vector3(length, 12 * 1.777778f * 4, 1), "Assets/textures/bg1_0.png", 1f, new Colour(1, 1, 1, 1));
            }

            for (int i = 0; i < 50 / 3; i++)
            {
                Vector2 XY = new Vector2(parallaxEffectPos2.X - 50 + (i * length), parallaxEffectPos2.Y);
                Renderer2D.DrawQuad(new Vector3(XY, 2 * -100), new Vector3(length, 12 * 1.777778f * 4, 1), "Assets/textures/bg1_1.png", 1f, new Colour(1, 1, 1, 1));
            }



            base.OnDraw();
        }
    }
}
