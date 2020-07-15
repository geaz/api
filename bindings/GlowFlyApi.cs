using System;
using System.Runtime.InteropServices;

namespace GlowFly.Api
{
    public enum AnalyzerSource
    {
        Basis = 0,
        Desktop = 1
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void OnFrequencyFunction(
        IntPtr GlowFlyApi,
        [MarshalAs(UnmanagedType.U1)] int volume,
        [MarshalAs(UnmanagedType.R4)] float decibel,
        [MarshalAs(UnmanagedType.U2)] int dominantFrequency);

    internal static class Bindings
    {
        private const string DllFile = "glowflyapi";

        [DllImport(DllFile, CallingConvention = CallingConvention.Cdecl)]
        internal extern static IntPtr glowfly_api_start([MarshalAs(UnmanagedType.LPStr)] string url, AnalyzerSource source);
        
        /// <summary>
        /// Posts a function to be executed on the main thread. You normally do not need
        /// to call this function, unless you want to tweak the native window.
        /// 
        /// Binding for:
        /// WEBVIEW_API void webview_dispatch(webview_t w, void (*fn)(webview_t w, void *arg), void *arg);
        /// </summary>
        /// <param name="webview">The webview to dispatch the function to</param>
        /// <param name="dispatchFunction">The function to execute on the webview thread</param>
        /// <param name="args">Paramters to pass to the dispatched function</param>
        [DllImport(DllFile, CallingConvention = CallingConvention.Cdecl)]
        internal extern static void glowfly_api_on_freq(IntPtr GlowFlyApi, OnFrequencyFunction onFrequencyFunction);
    }
}
