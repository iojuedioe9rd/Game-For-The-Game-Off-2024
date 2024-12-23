﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Vertex
{
    

    public static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Input_GetMousePosWorld(ref Vector2 pos);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Input_GetWindowSize(ref Vector2 windowSize);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Input_GetMousePos(ref Vector2 pos);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsMouseButtonPressed(MouseCode button);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog(string text, int parameter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog_Vector(ref Vector3 parameter, out Vector3 result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float NativeLog_VectorDot(ref Vector3 parameter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BeginLog(int flags);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void LogMsg(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void EndLog();

        #region Rendering

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Renderer2D_DrawQuad(ref Vector3 pos, ref Vector3 size, ref Colour colour);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Renderer2D_DrawQuadTex(ref Vector3 pos, ref Vector3 size, string textureFilename, float tilingFactor, ref Colour tintColour);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Renderer2D_DrawQuadTexRot(ref Vector3 pos, ref Vector3 size, string textureFilename, float Rotation, float tilingFactor, ref Colour tintColour);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Texture2D_FromFilename(string filename);

        #endregion

        #region Entity

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetTranslation(string entityID, out Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetTranslation(string entityID, ref Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetSize(string entityID, out Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetSize(string entityID, ref Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetRotation(string entityID, out Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetRotation(string entityID, ref Vector3 translation);

        

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string Entity_FindEntityByName(string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string Entity_NewEntity(string FullName, string name, ref Vector3 translation, ref Vector3 size, ref Vector3 rotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_RemoveEntity(string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string[] Entity_FindEntitiesByName(string name);

        #endregion

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode keycode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyUp(KeyCode keycode);

        #region RB 2D Entity

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2D_ApplyLinearImpulse(ref IntPtr rigidbody2DID, ref Vector2 impulse, ref Vector2 point, bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2D_ApplyLinearImpulseToCenter(ref IntPtr rigidbody2DID, ref Vector2 impulse, bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2D_GetTransform(ref IntPtr rigidbody2DID, ref Vector4 transform2D);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2D_SetTransform(ref IntPtr rigidbody2DID, ref Vector4 transform2D);

        #endregion

        #region Object

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string Object_GenerateUUID();

        #endregion

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static object GetScriptInstance(string entityID);
    }
}
