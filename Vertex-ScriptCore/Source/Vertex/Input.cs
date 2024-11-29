using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Vertex
{
    public class Input
    {
        public static bool IsKeyDown(KeyCode keycode)
        {
            return InternalCalls.Input_IsKeyDown(keycode);
        }

        public static bool IsKeyUp(KeyCode keycode)
        {
            return InternalCalls.Input_IsKeyUp(keycode);
        }

        public static bool IsMouseButtonPressed(MouseCode mouseCode)
        {
            return InternalCalls.Input_IsMouseButtonPressed(mouseCode);
        }

        public static Vector2 GetMousePos(bool isWorld = false)
        {
            Vector2 pos = new Vector2(0,0);
            if(isWorld)
            {
                InternalCalls.Input_GetMousePosWorld(ref pos);
            }
            else
            {
                InternalCalls.Input_GetMousePos(ref pos);
            }
            
            return pos;
        }
    }
}
