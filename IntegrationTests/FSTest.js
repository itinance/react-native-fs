'use strict';

var RCTTestModule = require('NativeModules').TestModule;
var React = require('react-native');
var {Text, View, } = React;
var fs = require('react-native-fs');
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
  fs.writeFile(path, text, function(err) {
    if (err) {
      updateMessage('testWriteAndReadFile failed' + err);
      throw err;
    }
    fs.readFile(path, function(err, contents) {
      if (err) {
        throw err;
      }
      expectEqual(contents, text, 'testWriteAndReadFile');
      fs.unlink(path, function(err) {
        if (err) {
          throw err;
        }
        runTestCase('\ntestCreateAndDeleteFile', testCreateAndDeleteFile);
      });
    });
  })
}

function testCreateAndDeleteFile() {
  var path = 'Documents/test.txt';
  var text = 'Lorem ipsum dolor sit amet';
  var readText;

  fs.writeFile(path, text, function(err) {
    if (err) {
      throw err;
    }
    fs.unlink(path, function(err) {
      if (err) {
        throw err;
      }
      fs.readFile(path, function(err, res) {
        if (err) {
          expectTrue(true, 'testCreateAndDeleteFile');
        }
        runTestCase('\ntestMkdirAndReaddir', testMkdirAndReaddir);
      });
    });
  });
}


function testMkdirAndReaddir() {
  var dirPath = 'Documents/testDir/';
  var fileName = 'hello.txt';
  var text = 'testing Read Dir';
  fs.mkdir(dirPath, function(err) {
    if (err) {
      updateMessage('mkdir failed');
      throw err;
    } else {
      fs.writeFile(dirPath + fileName, text, function(err) {
        if (err) {
          updateMessage('testWriteFile failed' + err);
          throw err;
        }
      });
      fs.readdir(dirPath, function(err, files) {
        if (err) {
          throw err;
        }
        if (files.length === 1) {
          expectEqual(files[0], fileName, 'testReadDir');
        } else {
          expectTrue(false, 'testReadDir');
        }
        runTestCase('\ntestStat', testStat);
      });
    }
  });
}

function testStat() {
  var file = "Documents/stat.txt";
  var text = 'hello world';
  fs.writeFile(file, text, function(err) {
    if (err) {
      throw err;
    }
    fs.stat(file, function(err, stats) {
      expectEqual(stats.isFile(), true, 'testStat');
      expectEqual(stats.size, 11, 'testStat');
      expectEqual(stats.mode, 644, 'testStat');
    });
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

module.exports = FSTest;