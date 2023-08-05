import { isEqual } from 'lodash';
import React from 'react';
import { Text, View } from 'react-native';

import {
  copyFileAssets,
  exists,
  existsAssets,
  mkdir,
  readDirAssets,
  readFile,
  readFileAssets,
  TemporaryDirectoryPath,
  unlink,
  writeFile,
} from '@dr.pogodin/react-native-fs';

import TestCase, { type StatusOrEvaluator } from './TestCase';

import styles from './styles';

/*
function logCharCodes(datum: string) {
  for (let i = 0; i < datum.length; ++i) {
    console.log(datum.charCodeAt(i).toString(16));
  }
}
*/

const tests: { [name: string]: StatusOrEvaluator } = {
  'copyFileAssets()': async () => {
    const path = `${TemporaryDirectoryPath}/good-utf8.txt`;
    try {
      await unlink(path);
    } catch {}
    try {
      if (await exists(path)) return 'fail';
      await copyFileAssets('test/good-utf8.txt', path);
      const res = await readFile(path);
      if (res !== 'GÖÖÐ\n') return 'fail';
      return 'pass';
    } catch {
      return 'fail';
    }
  },
  'exists()': async () => {
    const path = `${TemporaryDirectoryPath}/test-exists-file`;
    try {
      await unlink(path);
    } catch {}
    try {
      if (await exists(path)) return 'fail';
      await writeFile(path, 'xxx');
      if (!(await exists(path))) return 'fail';
      return 'pass';
    } catch {
      return 'fail';
    }
  },
  'existsAssets()': async () => {
    try {
      if (!(await existsAssets('test/good-utf8.txt'))) return 'fail';
      if (await existsAssets('test/non-existing.txt')) return 'fail';
      return 'pass';
    } catch {
      return 'fail';
    }
  },
  'mkdir()': async () => {
    const pathA = `${TemporaryDirectoryPath}/test-mkdir-path`;
    const pathB = `${pathA}/inner/path`;
    try {
      await unlink(pathA);
    } catch {}
    try {
      if (await exists(pathA)) return 'fail';
      await mkdir(pathB);
      if (!(await exists(pathB))) return 'fail';
      return 'pass';
    } catch {
      return 'fail';
    }
  },
  'readDirAssets()': async () => {
    try {
      const assets = await readDirAssets('test');

      const assets2 = assets.map((asset) => ({
        ctime: asset.ctime,
        mtime: asset.mtime,
        name: asset.name,
        path: asset.path,
        size: asset.size,
      }));

      if (
        !isEqual(assets2, [
          {
            ctime: null,
            mtime: null,
            name: 'good-latin1.txt',
            path: 'test/good-latin1.txt',
            size: 0,
          },
          {
            ctime: null,
            mtime: null,
            name: 'good-utf8.txt',
            path: 'test/good-utf8.txt',
            size: 0,
          },
        ])
      ) {
        return 'fail';
      }
    } catch {
      return 'fail';
    }
    return 'pass';

    /*  TODO: This would be the ideal test, but because isDirectory and isFile
        are functions, making this isEqual check falsy. We'll hovewer probably
        drop these functions in future, and thus use this test then. Also,
        note that currently it does not return ctime, mtime, size values
        for assets. Should we fix something here?
    if (
      !isEqual(await readDirAssets('test'), [
        {
          ctime: null,
          isDirectory: '[Function isDirectory]',
          isFile: '[Function isFile]',
          mtime: null,
          name: 'good-latin1.txt',
          path: 'test/good-latin1.txt',
          size: 0,
        },
        {
          ctime: null,
          isDirectory: '[Function isDirectory]',
          isFile: '[Function isFile]',
          mtime: null,
          name: 'good-utf8.txt',
          path: 'test/good-utf8.txt',
          size: 0,
        },
      ])
    ) {
      return 'fail';
    }
    */
  },
  'readFile() and writeFile()': async () => {
    const good = 'GÖÖÐ\n';
    const utf8 = '\x47\xC3\x96\xC3\x96\xC3\x90\x0A';
    const path = `${TemporaryDirectoryPath}/test-file`;
    try {
      await writeFile(path, utf8, 'ascii');
      let res = await readFile(path);
      if (res !== good) return 'fail';
      res = await readFile(path, 'ascii');
      if (res !== utf8) return 'fail';
      await writeFile(path, good);
      res = await readFile(path);
      if (res !== good) return 'fail';
      return 'pass';
    } catch (e) {
      console.error(e);
      return 'fail';
    }
  },
  'readFileAssets()': async () => {
    try {
      let res = await readFileAssets('test/good-latin1.txt', 'ascii');
      if (res !== 'GÖÖÐ\n') return 'fail';

      res = await readFileAssets('test/good-utf8.txt', 'ascii');
      if (res !== '\x47\xC3\x96\xC3\x96\xC3\x90\x0A') return 'fail';

      res = await readFileAssets('test/good-utf8.txt', 'utf8');
      if (res !== 'GÖÖÐ\n') return 'fail';

      res = await readFileAssets('test/good-utf8.txt');
      if (res !== 'GÖÖÐ\n') return 'fail';

      res = await readFileAssets('test/good-latin1.txt', 'base64');
      if (res !== 'R9bW0Ao=') return 'fail';

      res = await readFileAssets('test/good-utf8.txt', 'base64');
      if (res !== 'R8OWw5bDkAo=') return 'fail';

      return 'pass';
    } catch {
      return 'fail';
    }
  },
  'unlink()': async () => {
    try {
      const dirPath = `${TemporaryDirectoryPath}/test-unlink-dir`;
      const filePath = `${dirPath}/test-unlink-file`;
      await mkdir(dirPath);
      await writeFile(filePath, 'xxx');
      if (!(await exists(filePath))) return 'fail';
      await unlink(filePath);
      if (await exists(filePath)) return 'fail';
      await writeFile(filePath, 'xxx');
      if (!(await exists(filePath))) return 'fail';
      await unlink(dirPath);
      if (await exists(filePath)) return 'fail';
      return 'pass';
    } catch {
      return 'fail';
    }
  },
};

export default function TestBaseMethods() {
  return (
    <View>
      <Text style={styles.title}>Base Methods</Text>
      {Object.entries(tests).map(([name, test]) => (
        <TestCase key={name} name={name} status={test} />
      ))}
    </View>
  );
}
