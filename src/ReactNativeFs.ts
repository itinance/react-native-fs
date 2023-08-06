import { NativeModules, Platform } from 'react-native';

import { type Spec } from './NativeReactNativeFs';

const LINKING_ERROR =
  `The package '@dr.pogodin/react-native-fs' doesn't seem to be linked. Make sure: \n\n` +
  Platform.select({ ios: "- You have run 'pod install'\n", default: '' }) +
  '- You rebuilt the app after installing the package\n' +
  '- You are not using Expo Go\n';

// @ts-expect-error
const isTurboModuleEnabled = global.__turboModuleProxy != null;

const ReactNativeFsModule = isTurboModuleEnabled
  ? require('./NativeReactNativeFs').default
  : NativeModules.ReactNativeFs;

const ReactNativeFs = ReactNativeFsModule
  ? ReactNativeFsModule
  : new Proxy(
      {},
      {
        get() {
          throw new Error(LINKING_ERROR);
        },
      },
    );

export default ReactNativeFs as Spec;
