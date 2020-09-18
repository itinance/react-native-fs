import RNFS from "./index";

// Check overload type:

// 1) Should return Promise<...>
RNFS.downloadFile({
  fromUrl: "",
  toFile: "",
  signal: new AbortSignal()
}).then(result => {
  const num: number = result.statusCode;
});

// 2) Should return Object with promise prop
RNFS.downloadFile({
  fromUrl: "",
  toFile: ""
}).promise.then(result => {
  const num: number = result.statusCode;
});

// 3) Should return Promise<...>
RNFS.uploadFiles({
  toUrl: "",
  files: [],
  signal: new AbortSignal()
}).then(result => {
  const num: number = result.statusCode;
});

// 4) Should return Object with promise prop
RNFS.uploadFiles({
  toUrl: "",
  files: []
}).promise.then(result => {
  const num: number = result.statusCode;
});
