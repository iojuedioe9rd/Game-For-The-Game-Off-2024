using System;

namespace Vertex
{
    public static class Mathf
    {
        // Constants
        public const float E = 2.71828175f;
        public const float PI = 3.14159274f;
        public const float Tau = PI * 2;
        public const float RadToDegConstant = 180f / PI;  // Constant for converting radians to degrees
        public const float DegToRadConstant = PI / 180f;  // Constant for converting degrees to radians

        private const int maxRoundingDigits = 6;

        // Simple reciprocal estimate (no SIMD)
        public static float ReciprocalEstimate(float x)
        {
            if (x == 0) return float.NaN; // Avoid division by zero
            return 1.0f / x; // Basic reciprocal calculation
        }

        // Simple reciprocal square root estimate (no SIMD)
        public static float ReciprocalSqrtEstimate(float x)
        {
            if (x == 0) return float.NaN; // Avoid division by zero
            return 1.0f / (float)Math.Sqrt(x); // Basic reciprocal of sqrt calculation
        }

        // Approximate sine using Taylor series expansion (for small values)
        public static float Sin(float x)
        {
            // Use a simple Taylor series for sin(x) approximation
            float result = x;
            float term = x;
            for (int i = 3; i <= 9; i += 2)
            {
                term *= -x * x / ((i - 1) * i);
                result += term;
            }
            return result;
        }

        // Approximate cosine using Taylor series expansion (for small values)
        public static float Cos(float x)
        {
            // Use a simple Taylor series for cos(x) approximation
            float result = 1;
            float term = 1;
            for (int i = 2; i <= 10; i += 2)
            {
                term *= -x * x / ((i - 1) * i);
                result += term;
            }
            return result;
        }

        // Calculate the inverse tangent of x
        public static float Atan(float x)
        {
            // Using Math.Atan for simplicity
            return (float)Math.Atan(x);
        }

        public static float Atan2(float y, float x)
        {
            return (float)Math.Atan2(y, x); // Returns the angle in radians between the x-axis and the point (x, y)
        }

        // Calculate the square of a number
        public static float Square(float x)
        {
            return x * x;
        }

        // Round a number to the nearest integer (with optional precision)
        public static float Round(float value)
        {
            return (float)Math.Round(value, maxRoundingDigits);
        }

        // Calculate the absolute value of a number
        public static float Abs(float value)
        {
            return value < 0 ? -value : value;
        }

        // Normalize a vector represented by (x, y)
        public static (float, float) Normalize(float x, float y)
        {
            float magnitude = (float)Math.Sqrt(x * x + y * y);
            if (magnitude == 0)
                return (0, 0); // Avoid division by zero
            return (x / magnitude, y / magnitude);
        }

        // Simple linear interpolation between two values
        public static float Lerp(float a, float b, float t)
        {
            return a + t * (b - a);
        }

        // Clamp a value between a minimum and maximum range
        public static float Clamp(float value, float min, float max)
        {
            return value < min ? min : (value > max ? max : value);
        }

        // Simple exponential function (e^x)
        public static float Exp(float x)
        {
            return (float)Math.Pow(E, x);
        }

        // Convert radians to degrees
        public static float RadToDeg(float rad)
        {
            return rad * 180f / PI;
        }

        // Convert degrees to radians
        public static float DegToRad(float deg)
        {
            return deg * PI / 180f;
        }

        // Compute the sign of a number: 1 if positive, -1 if negative, 0 if zero
        public static int Sign(float value)
        {
            if (value > 0) return 1;
            if (value < 0) return -1;
            return 0;
        }

        // Smoothstep function for interpolation
        public static float SmoothStep(float edge0, float edge1, float x)
        {
            // Clamp x between 0 and 1, then apply the smoothstep formula
            x = Clamp((x - edge0) / (edge1 - edge0), 0f, 1f);
            return x * x * (3f - 2f * x);
        }

        // Linearly interpolates between a and b based on t, using cubic interpolation
        public static float LerpUnclamped(float a, float b, float t)
        {
            return a + t * (b - a);
        }

        // Cubic easing-in interpolation
        public static float EaseInCubic(float t)
        {
            return t * t * t;
        }

        // Cubic easing-out interpolation
        public static float EaseOutCubic(float t)
        {
            float f = t - 1;
            return f * f * f + 1;
        }

        // Calculate the cross product of two vectors (x1, y1) and (x2, y2)
        public static float CrossProduct(float x1, float y1, float x2, float y2)
        {
            return x1 * y2 - x2 * y1;
        }

        // Compute the distance between two points (x1, y1) and (x2, y2)
        public static float Distance(float x1, float y1, float x2, float y2)
        {
            return (float)Math.Sqrt(Square(x2 - x1) + Square(y2 - y1));
        }

        // Compute the dot product of two vectors (x1, y1) and (x2, y2)
        public static float DotProduct(float x1, float y1, float x2, float y2)
        {
            return x1 * x2 + y1 * y2;
        }

        // Convert a color from RGB to grayscale
        public static float RGBToGray(float r, float g, float b)
        {
            return 0.299f * r + 0.587f * g + 0.114f * b;
        }

        // Converts a floating-point number to a 0 to 1 range
        public static float Saturate(float value)
        {
            return Clamp(value, 0f, 1f);
        }

        // Simple function to convert between color channels
        public static float ToColorChannel(float value)
        {
            return Saturate(value);
        }

        // Step function that returns 0 or 1 depending on whether x is greater than or equal to edge
        public static float Step(float edge, float x)
        {
            return x < edge ? 0f : 1f;
        }
    }
}
