/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 * @flow strict-local
 */

import React, { useState } from 'react';
import {
  SafeAreaView,
  StyleSheet,
  ScrollView,
  View,
  Text,
  StatusBar,
  TextInput,
  Button,
  Alert,
  Picker,
} from 'react-native';


import RNFS from 'react-native-fs';
//import {Picker} from '@react-native-community/picker';

import {
  Colors,
  DebugInstructions,
  ReloadInstructions,
} from 'react-native/Libraries/NewAppScreen';


const App: () => React$Node = () => {

  const [mkdirParam, setMkdirParam] = useState('');

  const [moveFileSource, setMoveFileSource] = useState('');
  const [moveFileDest, setMoveFileDest] = useState('');

  const [copyFileSource, setCopyFileSource] = useState('');
  const [copyFileDest, setCopyFileDest] = useState('');

  const [unlinkFileParam, setUnlinkFileParam] = useState('');

  const [readDirParam, setReadDirParam] = useState('');

  const [statParam, setStatParam] = useState('');

  const [readFileParam, setReadFileParam] = useState('');

  const [readParam, setReadParam] = useState('');
  const [readLengthParam, setReadLengthParam] = useState('');
  const [readPositionParam, setReadPositionParam] = useState('');

  const [hashFileParam, setHashFileParam] = useState('');
  const [selectedValue, setSelectedValue] = useState('md5');

  const [writeFileParam, setWriteFileParam] = useState('');
  const [writeFileContentValue, setWriteFileContentValue] = useState('');

  const [appendFileParam, setAppendFileParam] = useState('');
  const [appendContentValue, setAppendContentValue] = useState('');

  const [writeParam, setWriteParam] = useState('');
  const [writeContentValue, setWriteContentValue] = useState('');
  const [writePositionValue, setWritePositionValue] = useState('');

  const [touchFilePathParam, setTouchFilePathParam] = useState('');
  const [touchMTime, setTouchMTime] = useState('');
  const [touchCTime, setTouchCTime] = useState('');

  const [downloadFilePathParam, setDownloadFilePathParam] = useState('');

  const mkdirExample = () => {
    if(mkdirParam.length > 0) {
      RNFS.mkdir(RNFS.DocumentDirectoryPath + '/' + mkdirParam)
            .then((result) => {
              Alert.alert('Successfully created directory.')
            })
            .catch((err) => {
              Alert.alert(err.message)
            })
    }
  }

  const moveFileExample = () => {
    if(moveFileSource.length > 0) {
      RNFS.moveFile(RNFS.DocumentDirectoryPath + '/' + moveFileSource, 
                    RNFS.DocumentDirectoryPath + '/' + moveFileDest)
            .then((result) => {
              Alert.alert('Successfully moved file to specified destination.')
            })
            .catch((err) => {
              Alert.alert(err.message)
            })
    }
  }

  const copyFileExample = () => {
    if(copyFileSource.length > 0) {
      RNFS.copyFile(RNFS.DocumentDirectoryPath + '/' + copyFileSource, 
                    RNFS.DocumentDirectoryPath + '/' + copyFileDest)
            .then((result) => {
              Alert.alert('Successfully put copy of file to specified destination.')
            })
            .catch((err) => {
              Alert.alert(err.message)
            })
    }
  }

  const getFSInfoExample = () => {
    RNFS.getFSInfo()
          .then((result) => {
            Alert.alert('Total space: ' + result.totalSpace + ' bytes\nFree space: ' + result.freeSpace + ' bytes')
          })
          .catch((err) => {
            Alert.alert(err.message)
          })
  }

  const unlinkExample = () => {
    if(unlinkFileParam.length > 0) {
      RNFS.unlink(RNFS.DocumentDirectoryPath + '/' + unlinkFileParam)
            .then((result) => {
              Alert.alert('Successfully unlinked specified file or folder')
            })
            .catch((err) => {
              Alert.alert(err.message)
            })
    }
  }

  const readDirExample = () => {
    RNFS.readDir(RNFS.DocumentDirectoryPath + '/' + readDirParam)
          .then((result) => {
            if(result.length == 0) {
              Alert.alert('Directory is empty')
            }
            else {
              let title = 'Number of contents: ' + result.length
              let output = '\nresult[0].name: ' + result[0].name +
                            '\nresult[0].size: ' + result[0].size + ' bytes' +
                            '\nresult[0].mtime: ' + result[0].mtime +
                            '\nresult[0].ctime: ' + result[0].ctime +
                            '\nresult[0].name: ' + result[0].name +
                            '\nresult[0].path: ' + result[0].path +
                            '\nresult[0].isFile(): ' + result[0].isFile() +
                            '\nresult[0].isDirectory(): ' + result[0].isDirectory() 
              Alert.alert(title, output)
            }
          })
          .catch((err) => {
            Alert.alert(err.message)
          })
  }

  const statExample = () => {
    RNFS.stat(RNFS.DocumentDirectoryPath + '/' + statParam)
          .then((result) => {
            let title = 'Stat Results:'
            let output = 'result.path: ' + result.path +
                          '\nresult.ctime: ' + result.ctime +
                          '\nresult.mtime: ' + result.mtime +
                          '\nresult.size: ' + result.size + ' bytes' +
                          '\nresult.mode: ' + result.mode +
                          '\nresult.originalFilePath: ' + result.originalFilePath +
                          '\nresult.isFile(): ' + result.isFile() +
                          '\nresult.isDirectory(): ' + result.isDirectory()
            Alert.alert(title, output)
          })
          .catch((err) => {
            Alert.alert(err.message)
          })
  }

  const readFileExample = () => {
    if(readFileParam.length > 0) {
      RNFS.readFile(RNFS.DocumentDirectoryPath + '/' + readFileParam)
            .then((result) => {
              Alert.alert('File Contents:', result)
            })
            .catch((err) => {
              Alert.alert(err.message)
            })
    }
  }

  const readExample = () => {
    if(readParam.length > 0) {

      var length = parseInt(readLengthParam, 10)
      var position = parseInt(readPositionParam, 10)

      if(length == NaN || position == NaN) {
        Alert.alert('Length and Position must be integers')
        return
      }

      RNFS.read(RNFS.DocumentDirectoryPath + '/' + readParam, length, position)
            .then((result) => {
              Alert.alert('File Contents:', result)
            })
            .catch((err) => {
              Alert.alert(err.message)
            })
    }
  }

  const hashFileExample = () => {
    if(hashFileParam.length > 0) {
      RNFS.hash(RNFS.DocumentDirectoryPath + '/' + hashFileParam, selectedValue)
            .then((result) => {
              Alert.alert('Hashed File Contents:', result)
            })
            .catch((err) => {
              Alert.alert(err.message)
            })
    }
  }
  
  const writeFileExample = () => {

    if(writeFileParam.length > 0 && writeFileContentValue.length > 0) {
      RNFS.writeFile(RNFS.DocumentDirectoryPath + '/' + writeFileParam, writeFileContentValue)
            .then((result) => {
              Alert.alert('Successfully Wrote to File')
            })
            .catch((err) => {
              Alert.alert(err.message)
            })
    }
  }

  const appendFileExample = () => {

    if(appendFileParam.length > 0 && appendContentValue.length > 0) {
      RNFS.appendFile(RNFS.DocumentDirectoryPath + '/' + appendFileParam, appendContentValue)
            .then((result) => {
              Alert.alert('Successfully Appended to File')
            })
            .catch((err) => {
              Alert.alert(err.message)
            })
    }
  }

  const writeExample = () => {

    if(writeParam.length > 0 && writeContentValue.length > 0) {

      var position = parseInt(writePositionValue, 10)

      if(position == NaN) {
        Alert.alert('Length and Position must be integers')
        return
      }

      RNFS.write(RNFS.DocumentDirectoryPath + '/' + writeParam, writeContentValue, position)
            .then((result) => {
              Alert.alert('Successfully Wrote to File ')
            })
            .catch((err) => {
              Alert.alert(err.message)
            })
    }
  }

  const touchFileExample = () => {
    if(touchFilePathParam.length > 0 && touchMTime.length > 0 && touchCTime.length > 0) {
      
      RNFS.touch(RNFS.DocumentDirectoryPath + '/' + touchFilePathParam, new Date(touchMTime), new Date(touchCTime))
            .then((result) => {
              Alert.alert('Successfully Appended to File', result)
            })
            .catch((err) => {
              Alert.alert(err.message)
            })
    }
  }

  const downloadFileExample = () => {
    RNFS.downloadFile({
      fromUrl: 'https://upload.wikimedia.org/wikipedia/commons/a/a1/Ronnie_O%E2%80%99Sullivan_at_Snooker_German_Masters_%28DerHexer%29_2015-02-06_07.jpg',
      toFile: RNFS.DocumentDirectoryPath + '/' + downloadFilePathParam +'/working_example2.jpg',
      begin: () => {console.log('It has begun!');},
    }).promise.then((r) => {
      Alert.alert('Successfully Downloaded File', r.jobId + ' ' + r.statusCode + ' ' + r.bytesWritten)
    })
    .catch((err) => {
      Alert.alert(err.message)
    });
  }

  return (
    <>
      <StatusBar barStyle="dark-content" />
      <SafeAreaView>
        <ScrollView
          contentInsetAdjustmentBehavior="automatic"
          style={styles.scrollView}>
          {global.HermesInternal == null ? null : (
            <View style={styles.engine}>
              <Text style={styles.footer}>Engine: Hermes</Text>
            </View>
          )}
          <Text style={styles.sectionTitle}>
            {"React Native File System Windows Demo App"}
          </Text>

          <View style={styles.body}>
            <View style={styles.sectionContainer}>
              <View style={styles.sectionDescription}>
                <Text>
                  {"RNFS.MainBundlePath: " + RNFS.MainBundlePath + "\n"}
                  {"RNFS.CachesDirectoryPath: " + RNFS.CachesDirectoryPath + "\n"}
                  {"RNFS.RoamingDirectoryPath: " + RNFS.RoamingDirectoryPath  + "\n"}
                  {"RNFS.DocumentDirectoryPath: " + RNFS.DocumentDirectoryPath  + "\n"}
                  {"RNFS.TemporaryDirectoryPath: " + RNFS.TemporaryDirectoryPath  + "\n"}
                  {"RNFS.ExternalDirectoryPath: " + RNFS.ExternalDirectoryPath}
                </Text>
              </View>
            </View>
          </View>

          <View style={styles.body}>
            <View style={styles.sectionContainer}>
            <Text style={styles.sectionTitle}>
              {"mkdir"}
            </Text>
              <View style={styles.sectionDescription}>
              <TextInput style = {styles.input}
                placeholder = "Folder Path"
                onChangeText={mkdirParam => setMkdirParam(mkdirParam)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Create Directory"
              color="#9a73ef"
              onPress={mkdirExample}
            />
            </View>
          </View>

          <View style={styles.body}>
            <View style={styles.sectionContainer}>
            <Text style={styles.sectionTitle}>
              {"moveFile"}
            </Text>
              <View style={styles.sectionDescription}>
              <TextInput style = {styles.input}
                placeholder = "Source File Path"
                onChangeText={moveFileSource => setMoveFileSource(moveFileSource)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = "Destination Path"
                onChangeText={moveFileDest => setMoveFileDest(moveFileDest)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Move File to Destination"
              color="#9a73ef"
              onPress={moveFileExample}
            />
            </View>
          </View>

          <View style={styles.body}>
            <View style={styles.sectionContainer}>
            <Text style={styles.sectionTitle}>
              {"copyFile"}
            </Text>
              <View style={styles.sectionDescription}>
              <TextInput style = {styles.input}
                placeholder = "Source File Path"
                onChangeText={copyFileSource => setCopyFileSource(copyFileSource)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = "Destination Path"
                onChangeText={copyFileDest => setCopyFileDest(copyFileDest)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Copy File to Destination"
              color="#9a73ef"
              onPress={copyFileExample}
            />
            </View>
          </View>

          <View style={styles.body}>
          <View style={styles.sectionContainer}>
            <Text style={styles.sectionTitle}>
              {"getFSInfo"}
            </Text>
            <Button
              title="Get Filesystem Information"
              color = "#9a73ef"
              onPress={getFSInfoExample}
            />
            </View>
          </View>
          
          <View style={styles.body}>
            <View style={styles.sectionContainer}>
            <Text style={styles.sectionTitle}>
              {"unlink"}
            </Text>
              <View style={styles.sectionDescription}>
              <TextInput style = {styles.input}
                placeholder = "Path"
                onChangeText={UnlinkFileParam => setUnlinkFileParam(UnlinkFileParam)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Unlink File at Path"
              color="#9a73ef"
              onPress={unlinkExample}
            />
            </View>
          </View>

          <View style={styles.body}>
            <View style={styles.sectionContainer}>
            <Text style={styles.sectionTitle}>
              {"readDir"}
            </Text>
              <View style={styles.sectionDescription}>
              <TextInput style = {styles.input}
                placeholder = "Directory Path"
                onChangeText={readDirParam => setReadDirParam(readDirParam)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Get Info About Directory"
              color="#9a73ef"
              onPress={readDirExample}
            />
            </View>
          </View>

          <View style={styles.body}>
            <View style={styles.sectionContainer}>
            <Text style={styles.sectionTitle}>
              {"stat"}
            </Text>
              <View style={styles.sectionDescription}>
              <TextInput style = {styles.input}
                placeholder = "File Path"
                onChangeText={statParam => setStatParam(statParam)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Get Info About Directory"
              color="#9a73ef"
              onPress={statExample}
            />
            </View>
          </View>

          <View style={styles.body}>
            <View style={styles.sectionContainer}>
            <Text style={styles.sectionTitle}>
              {"readFile"}
            </Text>
              <View style={styles.sectionDescription}>
              <TextInput style = {styles.input}
                placeholder = "File Path"
                onChangeText={readFileParam => setReadFileParam(readFileParam)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Read File"
              color="#9a73ef"
              onPress={readFileExample}
            />
            </View>
          </View>

          <View style={styles.body}>
            <View style={styles.sectionContainer}>
            <Text style={styles.sectionTitle}>
              {"read"}
            </Text>
              <View style={styles.sectionDescription}>
              <TextInput style = {styles.input}
                placeholder = "File Path"
                onChangeText={readParam => setReadParam(readParam)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = "Length (Please Enter Integer)"
                onChangeText={readLengthParam => setReadLengthParam(readLengthParam)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = 'Position (Please Enter Integer)'
                onChangeText={readPositionParam => setReadPositionParam(readPositionParam)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Read File Excerpt From Position"
              color="#9a73ef"
              onPress={readExample}
            />
            </View>
          </View>

          <View style={styles.body}>
            <View style={styles.sectionContainer}>
            <Text style={styles.sectionTitle}>
              {"hash"}
            </Text>
              <View style={styles.sectionDescription}>
              <TextInput style = {styles.input}
                placeholder = "File Path"
                onChangeText={hashFileParam => setHashFileParam(hashFileParam)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <Picker
                selectedValue={selectedValue}
                onChangeText={readPositionParam => setReadPositionParam(readPositionParam)}
                style={{ height: 50, width: 150 }}
                onValueChange={(itemValue, itemIndex) => setSelectedValue(itemValue)}
              >
                <Picker.Item label="MD5" value="md5" />
                <Picker.Item label="SHA1" value="sha1" />
                <Picker.Item label="SHA224" value="sha224" />
                <Picker.Item label="SHA256" value="sha256" />
                <Picker.Item label="SHA384" value="sha384" />
                <Picker.Item label="SHA512" value="sha512" />
              </Picker>
              <View>

              </View>
              </View>
            <Button
              title="Hash File Contents"
              color="#9a73ef"
              onPress={hashFileExample}
            />
            </View>
          </View>

          <View style={styles.body}>
            <View style={styles.sectionContainer}>
            <Text style={styles.sectionTitle}>
              {"writeFile"}
            </Text>
              <View style={styles.sectionDescription}>
              <TextInput style = {styles.input}
                placeholder = "File Path"
                onChangeText={writeFileParam => setWriteFileParam(writeFileParam)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = "Content"
                onChangeText={writeFileContentValue => setWriteFileContentValue(writeFileContentValue)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Write Content to File"
              color="#9a73ef"
              onPress={writeFileExample}
            />
            </View>
          </View>

          <View style={styles.body}>
            <View style={styles.sectionContainer}>
            <Text style={styles.sectionTitle}>
              {"appendFile"}
            </Text>
              <View style={styles.sectionDescription}>
              <TextInput style = {styles.input}
                placeholder = "File Path"
                onChangeText={appendFileParam => setAppendFileParam(appendFileParam)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = "Content"
                onChangeText={appendContentValue => setAppendContentValue(appendContentValue)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Append Content to File"
              color="#9a73ef"
              onPress={appendFileExample}
            />
            </View>
          </View>

          <View style={styles.body}>
            <View style={styles.sectionContainer}>
            <Text style={styles.sectionTitle}>
              {"write"}
            </Text>
              <View style={styles.sectionDescription}>
              <TextInput style = {styles.input}
                placeholder = "File Path"
                onChangeText={writeParam => setWriteParam(writeParam)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = "Content"
                onChangeText={writeContentValue => setWriteContentValue(writeContentValue)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = "Position"
                onChangeText={writePositionValue => setWritePositionValue(writePositionValue)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Write Content to File at Position"
              color="#9a73ef"
              onPress={writeExample}
            />
            </View>
          </View>

          <View style={styles.body}>
            <View style={styles.sectionContainer}>
            <Text style={styles.sectionTitle}>
              {"touch"}
            </Text>
              <View style={styles.sectionDescription}>
              <TextInput style = {styles.input}
                placeholder = "File Path"
                onChangeText={touchFilePathParam => setTouchFilePathParam(touchFilePathParam)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = "Modified UNIX Time"
                onChangeText={touchMTime => setTouchMTime(touchMTime)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = "Created UNIX Time"
                onChangeText={touchCTime => setTouchCTime(touchCTime)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Touch File with New Times"
              color="#9a73ef"
              onPress={touchFileExample}
            />
            </View>
          </View>

          <View style={styles.body}>
            <View style={styles.sectionContainer}>
            <Text style={styles.sectionTitle}>
              {"downloadFile"}
            </Text>
              <View style={styles.sectionDescription}>
              <TextInput style = {styles.input}
                placeholder = "Destination Path"
                onChangeText={touchCTime => setTouchCTime(touchCTime)}
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Download File to Destination"
              onPress={downloadFileExample}
              color="#9a73ef"
            />
            </View>
          </View>

        </ScrollView>
      </SafeAreaView>
    </>
  );
};

const styles = StyleSheet.create({
  scrollView: {
    backgroundColor: Colors.black,
  },
  engine: {
    position: 'absolute',
    right: 0,
  },
  body: {
    backgroundColor: Colors.dark,
  },
  sectionContainer: {
    marginTop: 32,
    paddingHorizontal: 24,
  },
  sectionTitle: {
    fontSize: 24,
    fontWeight: '600',
    color: Colors.white,
  },
  sectionDescription: {
    marginTop: 8,
    fontSize: 18,
    fontWeight: '400',
    color: Colors.dark,
  },
  highlight: {
    fontWeight: '700',
  },
  footer: {
    color: Colors.dark,
    fontSize: 12,
    fontWeight: '600',
    padding: 4,
    paddingRight: 12,
    textAlign: 'right',
  },
});

export default App;
