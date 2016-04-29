Pod::Spec.new do |s|

  s.name         = "RNFS"
  s.version      = "1.3.0"
  s.homepage     = "https://github.com/johanneslumpe/react-native-fs"
  s.platform     = :ios, "7.0"
  s.source       = { :git => "https://github.com/johanneslumpe/react-native-fs.git" }
  s.source_files = '*.{h,m}'
  s.preserve_paths = "**/*.js"
  s.dependency   'React/Core'
end
