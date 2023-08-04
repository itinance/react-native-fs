import * as React from 'react';

import { View } from 'react-native';

import TestBaseMethods from './TestBaseMethods';
import TestConstants from './TestConstants';

export default function App() {
  return (
    <View>
      <TestConstants />
      <TestBaseMethods />
    </View>
  );
}
