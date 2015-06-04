
'use strict';

var RCTTestModule = require('NativeModules').TestModule;
var React = require('react-native');
var {
  Text,
  View,
} = React;
var RNFS = require('react-native-fs');
var DEBUG = false;


// setup in componentDidMount
var done;
var updateMessage;

function runTestCase(description, fn) {
  updateMessage(description);
  fn();
}

function expectTrue(condition, message) {
  if (!condition) {
    throw new Error(message);
  }
}

function expectEqual(lhs, rhs, testname) {
  expectTrue(
    lhs === rhs,
    'Error in test ' + testname + ': expected ' + rhs + ', got ' + lhs
  );
}

function expectFSNoError(err) {
  expectTrue(err === null, 'Unexpected FS error: ' + JSON.stringify(err));
}

function testWriteAndReadFile() {
  var path = RNFS.DocumentDirectoryPath + '/test.txt';

  var text = 'Lorem ipsum dolor sit amet';
  var readText;

  RNFS.writeFile(path, text)
    .then((success) => {
      updateMessage('FILE WRITTEN!');
      return RNFS.readFile(path);
    })
    .then((contents) => {
      updateMessage('FILE READ! Contents:');
      readText = contents;
      updateMessage(readText);
      expectEqual(text, readText, 'testWriteAndReadFile');
      runTestCase('testCreateAndDeleteFile', testCreateAndDeleteFile);
    })
    .catch((err) => {
      updateMessage(err.message, err.code);
    });
}



function testCreateAndDeleteFile() {
  var path = RNFS.DocumentDirectoryPath + '/test.txt';
  var text = 'Lorem ipsum dolor sit amet';
  var readText;

  RNFS.writeFile(path, text)
    .then((success) => {
      updateMessage('FILE CREATED!');
      return RNFS.unlink(path);
    })
    .spread((success, path) => {
      updateMessage('FILE DELETED!' + success + ',' + path);
      return RNFS.stat(path);
    })
    .then((statResult) => {
      updateMessage('*****' + statResult);
      if (statResult.isFile()) {
        updateMessage('FILE STILL EXISTS');
      }
    })
    .catch((err) => {
      updateMessage('catch' + err);
      expectTrue(true,'File is deleted');
      done();      
    });
}




var FSTest = React.createClass({
  getInitialState() {
      return {
        messages: 'Initializing...',
        done: false,
      };
    },

    componentDidMount() {
      done = () => this.setState({
        done: true
      }, RCTTestModule.markTestCompleted);
      updateMessage = (msg) => {
        this.setState({
          messages: this.state.messages.concat('\n' + msg)
        });
        DEBUG && console.log(msg);
      };
      testWriteAndReadFile();
    },

    render() {
      return (
      <View style={{backgroundColor: 'white', padding: 40}}>
        <Text>
          {this.constructor.displayName + ': '}
          {this.state.done ? 'Done' : 'Testing...'}
          {'\n\n' + this.state.messages}
        </Text>
      </View>
      );
    }
});

module.exports = FSTest;
