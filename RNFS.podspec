require 'json'
pjson = JSON.parse(File.read('package.json'))

Pod::Spec.new do |s|

  s.name            = "RNFS"
  s.version         = pjson["version"]
  s.homepage        = "https://github.com/johanneslumpe/react-native-fs"
  s.summary         = pjson["description"]
  s.license         = pjson["license"]
  s.author          = { "Johannes Lumpe" => "johannes@lum.pe" }
  s.platform        = :ios, "7.0"
  s.source          = { :git => "https://github.com/johanneslumpe/react-native-fs", :tag => "#{s.version}" }
  s.source_files    = '*.{h,m}'
  s.preserve_paths  = "**/*.js"
  
  s.dependency 'React/Core'

end
