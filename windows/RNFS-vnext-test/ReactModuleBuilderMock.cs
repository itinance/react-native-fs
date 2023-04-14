﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.UI.Xaml;



namespace Microsoft.ReactNative.Managed.UnitTests
{
    class ReactModuleBuilderMock : IReactModuleBuilder
    {
        private List<InitializerDelegate> m_initializers = new List<InitializerDelegate>();
        private Dictionary<string, Tuple<MethodReturnType, MethodDelegate>> m_methods =
            new Dictionary<string, Tuple<MethodReturnType, MethodDelegate>>();
        private Dictionary<string, SyncMethodDelegate> m_syncMethods =
            new Dictionary<string, SyncMethodDelegate>();
        private List<ConstantProviderDelegate> m_constantProviders = new List<ConstantProviderDelegate>();
        private Action<string, string, JSValue> m_jsEventHandler;
        private Action<string, string, JSValue> m_jsFunctionHandler;

        public bool IsResolveCallbackCalled { get; private set; }
        public bool IsRejectCallbackCalled { get; private set; }

        public T CreateModule<T>(ReactModuleInfo moduleInfo) where T : class
        {
            var reactContext = new ReactContextMock(this);
            var module = (T)moduleInfo.ModuleProvider(this);
            foreach (var initializer in m_initializers)
            {
                initializer(reactContext);
            }

            return module;
        }

        public void AddInitializer(InitializerDelegate initializer)
        {
            m_initializers.Add(initializer);
        }

        public void AddConstantProvider(ConstantProviderDelegate constantProvider)
        {
            m_constantProviders.Add(constantProvider);
        }

        public void AddMethod(string name, MethodReturnType returnType, MethodDelegate method)
        {
            m_methods.Add(name, Tuple.Create(returnType, method));
        }

        public void AddSyncMethod(string name, SyncMethodDelegate method)
        {
            m_syncMethods.Add(name, method);
        }

        public void Call0(string methodName) =>
            GetMethod0(methodName)?.Invoke(ArgReader(), ArgWriter(), null, null);

        public void Call0<T1>(string methodName, T1 arg1) =>
            GetMethod0(methodName)?.Invoke(ArgReader(arg1), ArgWriter(), null, null);

        public void Call0<T1, T2>(string methodName, T1 arg1, T2 arg2) =>
            GetMethod0(methodName)?.Invoke(ArgReader(arg1, arg2), ArgWriter(), null, null);

        public void Call0<T1, T2, T3>(string methodName, T1 arg1, T2 arg2, T3 arg3) =>
            GetMethod0(methodName)?.Invoke(ArgReader(arg1, arg2, arg3), ArgWriter(), null, null);

        public Task<bool> Call1<TResult>(string methodName, Action<TResult> resolve)
        {
            var promise = new TaskCompletionSource<bool>();
            GetMethod1(methodName)?.Invoke(ArgReader(), ArgWriter(), ResolveCallback(resolve, promise), null);
            return promise.Task;
        }

        public Task<bool> Call1<T1, TResult>(string methodName, T1 arg1, Action<TResult> resolve)
        {
            var promise = new TaskCompletionSource<bool>();
            GetMethod1(methodName)?.Invoke(ArgReader(arg1), ArgWriter(), ResolveCallback(resolve, promise), null);
            return promise.Task;
        }

        public Task<bool> Call1<T1, T2, TResult>(string methodName, T1 arg1, T2 arg2, Action<TResult> resolve)
        {
            var promise = new TaskCompletionSource<bool>();
            GetMethod1(methodName)?.Invoke(ArgReader(arg1, arg2), ArgWriter(), ResolveCallback(resolve, promise), null);
            return promise.Task;
        }

        public Task<bool> Call1<T1, T2, T3, TResult>(string methodName, T1 arg1, T2 arg2, T3 arg3, Action<TResult> resolve)
        {
            var promise = new TaskCompletionSource<bool>();
            GetMethod1(methodName)?.Invoke(ArgReader(arg1, arg2, arg3), ArgWriter(), ResolveCallback(resolve, promise), null);
            return promise.Task;
        }

        public Task<bool> Call2<TResult, TError>(string methodName, Action<TResult> resolve, Action<TError> reject)
        {
            var promise = new TaskCompletionSource<bool>();
            GetMethod2(methodName)?.Invoke(ArgReader(), ArgWriter(),
                ResolveCallback(resolve, promise), RejectCallback(reject, promise));
            return promise.Task;
        }

        public Task<bool> Call2<T1, TResult, TError>(string methodName, T1 arg1,
            Action<TResult> resolve, Action<TError> reject)
        {
            var promise = new TaskCompletionSource<bool>();
            GetMethod2(methodName)?.Invoke(ArgReader(arg1), ArgWriter(),
                ResolveCallback(resolve, promise), RejectCallback(reject, promise));
            return promise.Task;
        }

        public Task<bool> Call2<T1, T2, TResult, TError>(string methodName, T1 arg1, T2 arg2,
            Action<TResult> resolve, Action<TError> reject)
        {
            var promise = new TaskCompletionSource<bool>();
            GetMethod2(methodName)?.Invoke(ArgReader(arg1, arg2), ArgWriter(),
                ResolveCallback(resolve, promise), RejectCallback(reject, promise));
            return promise.Task;
        }

        public Task<bool> Call2<T1, T2, T3, TResult, TError>(string methodName, T1 arg1, T2 arg2, T3 arg3,
            Action<TResult> resolve, Action<TError> reject)
        {
            var promise = new TaskCompletionSource<bool>();
            GetMethod2(methodName)?.Invoke(ArgReader(arg1, arg2, arg3), ArgWriter(),
                ResolveCallback(resolve, promise), RejectCallback(reject, promise));
            return promise.Task;
        }

        public void CallSync<TResult>(string methodName, out TResult result)
        {
            var writer = ArgWriter();
            GetSyncMethod(methodName)?.Invoke(ArgReader(), writer);
            result = GetResult<TResult>(writer);
        }

        public void CallSync<T1, TResult>(string methodName, T1 arg1, out TResult result)
        {
            var writer = ArgWriter();
            GetSyncMethod(methodName)?.Invoke(ArgReader(arg1), writer);
            result = GetResult<TResult>(writer);
        }

        public void CallSync<T1, T2, TResult>(string methodName, T1 arg1, T2 arg2, out TResult result)
        {
            var writer = ArgWriter();
            GetSyncMethod(methodName)?.Invoke(ArgReader(arg1, arg2), writer);
            result = GetResult<TResult>(writer);
        }

        public void CallSync<T1, T2, T3, TResult>(string methodName, T1 arg1, T2 arg2, T3 arg3, out TResult result)
        {
            var writer = ArgWriter();
            GetSyncMethod(methodName)?.Invoke(ArgReader(arg1, arg2, arg3), writer);
            result = GetResult<TResult>(writer);
        }

        private MethodDelegate GetMethod0(string methodName) =>
            (m_methods.TryGetValue(methodName, out var tuple) && tuple.Item1 == MethodReturnType.Void) ? tuple.Item2 : null;

        private MethodDelegate GetMethod1(string methodName) =>
            (m_methods.TryGetValue(methodName, out var tuple) && tuple.Item1 == MethodReturnType.Callback) ? tuple.Item2 : null;

        private MethodDelegate GetMethod2(string methodName) =>
            (m_methods.TryGetValue(methodName, out var tuple)
            && (tuple.Item1 == MethodReturnType.TwoCallbacks) || tuple.Item1 == MethodReturnType.Promise) ? tuple.Item2 : null;

        private SyncMethodDelegate GetSyncMethod(string methodName) =>
            m_syncMethods.TryGetValue(methodName, out var syncMethod) ? syncMethod : null;

        private MethodResultCallback ResolveCallback<T>(Action<T> resolve, TaskCompletionSource<bool> promise = null)
        {
            return (IJSValueWriter writer) =>
            {
                resolve(GetResult<T>(writer));
                IsResolveCallbackCalled = true;
                promise?.SetResult(true);
            };
        }

        private MethodResultCallback RejectCallback<T>(Action<T> reject, TaskCompletionSource<bool> promise = null)
        {
            return (IJSValueWriter writer) =>
            {
                reject(GetResult<T>(writer));
                IsRejectCallbackCalled = true;
                promise?.SetResult(false);
            };
        }

        private static T GetResult<T>(IJSValueWriter writer)
        {
            var resulReader = new JSValueTreeReader((writer as JSValueTreeWriter).TakeValue());
            resulReader.ReadArgs(out T result);
            return result;
        }

        private static IJSValueWriter ArgWriter() => new JSValueTreeWriter();

        private static IJSValueReader ArgReader() => CreateArgReader(w => w.WriteArgs());

        private static IJSValueReader ArgReader<T1>(T1 arg1) => CreateArgReader(w => w.WriteArgs(arg1));

        private static IJSValueReader ArgReader<T1, T2>(T1 arg1, T2 arg2) =>
            CreateArgReader(w => w.WriteArgs(arg1, arg2));

        private static IJSValueReader ArgReader<T1, T2, T3>(T1 arg1, T2 arg2, T3 arg3) =>
            CreateArgReader(w => w.WriteArgs(arg1, arg2, arg3));

        private static IJSValueReader CreateArgReader(Func<IJSValueWriter, IJSValueWriter> argWriter) =>
            new JSValueTreeReader((argWriter(new JSValueTreeWriter()) as JSValueTreeWriter).TakeValue());

        public IReadOnlyDictionary<string, JSValue> GetConstants()
        {
            var constantWriter = new JSValueTreeWriter();
            constantWriter.WriteObjectBegin();
            foreach (var constantProvider in m_constantProviders)
            {
                constantProvider(constantWriter);
            }

            constantWriter.WriteObjectEnd();
            return constantWriter.TakeValue().Object;
        }

        public void ExpectEvent(string eventEmitterName, string eventName, Action<JSValue> checkValue)
        {
            m_jsEventHandler = (string actualEventEmitterName, string actualEventName, JSValue value) =>
            {
                Assert.AreEqual(eventEmitterName, actualEventEmitterName);
                Assert.AreEqual(eventName, actualEventName);
                checkValue(value);
            };
        }

        public void ExpectFunction(string moduleName, string functionName, Action<IReadOnlyList<JSValue>> checkValues)
        {
            m_jsFunctionHandler = (string actualModuleName, string actualFunctionName, JSValue value) =>
            {
                Assert.AreEqual(moduleName, actualModuleName);
                Assert.AreEqual(functionName, actualFunctionName);
                Assert.AreEqual(JSValueType.Array, value.Type);
                checkValues(value.Array);
            };
        }

        public void CallJSFunction(string moduleName, string functionName, JSValueArgWriter paramsArgWriter)
        {
            var writer = new JSValueTreeWriter();
            paramsArgWriter(writer);
            m_jsFunctionHandler(moduleName, functionName, writer.TakeValue());
        }

        public void EmitJSEvent(string eventEmitterName, string eventName, JSValueArgWriter paramsArgWriter)
        {
            var writer = new JSValueTreeWriter();
            paramsArgWriter(writer);
            m_jsEventHandler(eventEmitterName, eventName, writer.TakeValue());
        }
    }

    class ReactContextMock : IReactContext
    {
        private ReactModuleBuilderMock m_builder;

        public ReactContextMock(ReactModuleBuilderMock builder)
        {
            m_builder = builder;
        }

        public void DispatchEvent(FrameworkElement view, string eventName, JSValueArgWriter eventDataArgWriter)
        {
            throw new NotImplementedException();
        }

        public void CallJSFunction(string moduleName, string functionName, JSValueArgWriter paramsArgWriter)
        {
            m_builder.CallJSFunction(moduleName, functionName, paramsArgWriter);
        }

        public void EmitJSEvent(string eventEmitterName, string eventName, JSValueArgWriter paramsArgWriter)
        {
            m_builder.EmitJSEvent(eventEmitterName, eventName, paramsArgWriter);
        }
    }
}