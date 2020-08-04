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
  Picker
} from 'react-native';

import RNFS from 'react-native-fs';
//import {Picker} from '@react-native-community/picker';

import {
  Colors,
  DebugInstructions,
  ReloadInstructions,
} from 'react-native/Libraries/NewAppScreen';

const App: () => React$Node = () => {

  const [selectedValue, setSelectedValue] = useState("md5");
  const [mkdirParam, setMkdirParam] = useState('');
  const [moveFileSource, setMoveFileSource] = useState('');
  const [moveFileDest, setMoveFileDest] = useState('');
  const [copyFileSource, setCopyFileSource] = useState('');
  const [copyFileDest, setCopyFileDest] = useState('');

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
            Alert.alert('Total space: ' + result.totalSpace + ' bytes\nFree space:  ' + result.freeSpace + ' bytes')
          })
          .catch((err) => {
            Alert.alert(err.message)
          })
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
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Copy File to Destination"
              color="#9a73ef"
              onPress={() => Alert.alert('Button with adjusted color pressed')}
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
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Get Info About Directory"
              color="#9a73ef"
              onPress={() => Alert.alert('Button with adjusted color pressed')}
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
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Get Info About Directory"
              color="#9a73ef"
              onPress={() => Alert.alert('Button with adjusted color pressed')}
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
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Read File"
              color="#9a73ef"
              onPress={() => Alert.alert('Button with adjusted color pressed')}
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
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = "Length"
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = "Position"
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Read File Excerpt From Position"
              color="#9a73ef"
              onPress={() => Alert.alert('Button with adjusted color pressed')}
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
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <Picker
                selectedValue={selectedValue}
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
              onPress={() => Alert.alert('Button with adjusted color pressed')}
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
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = "Content"
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Write Content to File"
              color="#9a73ef"
              onPress={() => Alert.alert('Button with adjusted color pressed')}
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
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = "Content"
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Append Content to File"
              color="#9a73ef"
              onPress={() => Alert.alert('Button with adjusted color pressed')}
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
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = "Content"
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = "Position"
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Write Content to File at Position"
              color="#9a73ef"
              onPress={() => Alert.alert('Button with adjusted color pressed')}
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
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = "Modified UNIX Time"
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              <TextInput style = {styles.input}
                placeholder = "Created UNIX Time"
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Touch File with New Times"
              color="#9a73ef"
              onPress={() => Alert.alert('Button with adjusted color pressed')}
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
                placeholderTextColor = "#9a73ef"
                autoCapitalize = "none"
              />
              </View>
            <Button
              title="Download File to Destination [Currently Unavailable]"
              color="#999999"
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
