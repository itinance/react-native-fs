import * as React from 'react';

import { SafeAreaView, ScrollView } from 'react-native';

import TestBaseMethods from './TestBaseMethods';
import TestConstants from './TestConstants';

export default function App() {
  return (
    <SafeAreaView>
      <ScrollView>
        <TestConstants />
        <TestBaseMethods />
      </ScrollView>
    </SafeAreaView>
  );
}
