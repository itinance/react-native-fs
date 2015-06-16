'use strict';

var RCTTestModule = require('NativeModules').TestModule;
var React = require('react-native');
var {Text, View, } = React;
var fs = require('react-native-fs').Promisify;
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
  var path = 'Documents/test.txt';

  var text = 'Lorem ipsum dolor sit amet';
  var readText;

  fs.writeFile(path, text)
    .then((success) => {
      return fs.readFile(path);
    })
    .then((contents) => {
      readText = contents;
      expectEqual(text, readText, 'testWriteAndReadFile');
    })
    .finally(() => {
      runTestCase('\ntestCreateAndDeleteFile', testCreateAndDeleteFile);
    })
    .done(); //promise done needed to throw exception so that in case test fails,error is propagated
}



function testCreateAndDeleteFile() {
  var path = 'Documents/test.txt';
  var text = 'Lorem ipsum dolor sit amet';
  var readText;

  fs.writeFile(path, text)
    .then((success) => {
      return fs.unlink(path);
    })
    .then((success) => {
      return fs.readFile(path);
    })
    .then((contents) => {

    })
    .catch((err) => {
      if (err) {
        expectTrue(true, 'testMkdirAndReaddir');
      }
    })
    .finally(() => {
      runTestCase('\ntestMkdirAndReaddir', testMkdirAndReaddir);
    })
    .done(); //promise done needed to throw exception so that in case test fails,error is propagated
}

function testMkdirAndReaddir() {
  var dirPath = 'Documents/testDir/';
  var fileName = 'hello.txt';
  var text = 'testing Read Dir';
  fs.mkdir(dirPath)
    .then((success) => {
      return fs.writeFile(dirPath + fileName, text);
    })
    .then((success) => {
      return fs.readdir(dirPath);
    })
    .then((files) => {
      if (files.length === 1) {
        expectEqual(files[0], fileName, 'testReadDir');
      } else {
        expectTrue(false, 'testReadDir');
      }
    })
    .finally(() => {
      runTestCase('\ntestStat', testStat);
    })
    .done(); //promise done needed to throw exception so that in case test fails,error is propagated
}

function testStat() {
  var file = "Documents/stat.txt";
  var text = 'hello world';
  fs.writeFile(file, text)
    .then(() => {
      return fs.stat(file);
    })
    .then((stats) => {
      expectEqual(stats.isFile(), true, 'testStat');
      expectEqual(stats.size, 11, 'testStat');
      expectEqual(stats.mode, 644, 'testStat');
    })
    .finally(() => {
      done(); //testrunners done
    })
    .done(); //promise done needed to throw exception so that in case test fails,error is propagated
}

var FSPromisifiedAPITest = React.createClass({
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
    runTestCase('\ntestWriteAndReadFile', testWriteAndReadFile);
  },

  render() {
    return (
      <View style={{
        backgroundColor: 'white',
        padding: 40
      }}>
        <Text>
          {this.constructor.displayName + ': '}
          {this.state.done ? 'Done' : 'Testing...'}
          {'\n\n' + this.state.messages}
        </Text>
      </View>
      );
  }
});

module.exports = FSPromisifiedAPITest;