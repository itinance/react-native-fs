import React from 'react';
import { Text, View } from 'react-native';

const RNFS = require('@dr.pogodin/react-native-fs');

import TestCase from './TestCase';

import styles from './styles';

const constants = [
  'CachesDirectoryPath',
  'DocumentDirectoryPath',
  'DownloadDirectoryPath',
  'ExternalCachesDirectoryPath',
  'ExternalDirectoryPath',
  'ExternalStorageDirectoryPath',
  'LibraryDirectoryPath',
  'MainBundlePath',
  'PicturesDirectoryPath',
  'RoamingDirectoryPath',
  'TemporaryDirectoryPath',
];

export default function TestConstants() {
  return (
    <View>
      <Text style={styles.title}>Constants</Text>
      {constants.map((name) => (
        <TestCase
          name={name}
          details={RNFS[name]}
          key={name}
          status={RNFS[name] !== undefined ? 'pass' : 'fail'}
        />
      ))}
    </View>
  );
}
