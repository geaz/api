using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace GlowFly.Api
{
    public class GlowFlyApi : IDisposable
    {
        private bool _disposed = false;
        private readonly List<OnFrequencyFunction> freqCallBacks = new List<OnFrequencyFunction>();
        private readonly IntPtr _apiPtr;

        public GlowFlyApi(string url, AnalyzerSource source)
        {
            _apiPtr = Bindings.Init(url, source);
        }

        public void Start()
        {
            Bindings.Start(_apiPtr);
        }

        public void OnFrequencyUpdate(OnFrequencyFunction func)
        {
            freqCallBacks.Add(func);
            Bindings.OnFreqUpdate(_apiPtr, func);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!_disposed)
            {
                Bindings.Stop(_apiPtr);
                freqCallBacks.Clear();
                _disposed = true;
            }
        }
    }

    public enum AnalyzerSource
    {
        Basis = 0,
        Desktop = 1
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void OnFrequencyFunction(
        byte volume,
        ushort dominantFrequency);

    internal static class Bindings
    {
        private const string DllFile = "glowflyapi";

        [DllImport(DllFile, EntryPoint = "glowfly_api_init", CallingConvention = CallingConvention.Cdecl)]
        internal extern static IntPtr Init([MarshalAs(UnmanagedType.LPStr)] string url, AnalyzerSource source);

        [DllImport(DllFile, EntryPoint = "glowfly_api_start", CallingConvention = CallingConvention.Cdecl)]
        internal extern static IntPtr Start(IntPtr GlowFlyApi);

        [DllImport(DllFile, EntryPoint = "glowfly_api_stop", CallingConvention = CallingConvention.Cdecl)]
        internal extern static IntPtr Stop(IntPtr GlowFlyApi);

        [DllImport(DllFile, EntryPoint = "glowfly_api_on_freq", CallingConvention = CallingConvention.Cdecl)]
        internal extern static void OnFreqUpdate(IntPtr GlowFlyApi, OnFrequencyFunction onFrequencyFunction);
    }
}
