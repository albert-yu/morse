#
# Be sure to run `pod lib lint morsemail.podspec' to ensure this is a
# valid spec before submitting.
#
# Any lines starting with a # are optional, but their use is encouraged
# To learn more about a Podspec see https://guides.cocoapods.org/syntax/podspec.html
#

Pod::Spec.new do |s|
  s.name             = 'morse'
  s.version          = 'v0.1.8-alpha'
  s.summary          = 'A C library for sending and receiving email.'

# This description is used to generate tags and improve search results.
#   * Think: What does it do? Why did you write it? What is the focus?
#   * Try to keep it short, snappy and to the point.
#   * Write the description between the DESC delimiters below.
#   * Finally, don't worry about the indent, CocoaPods strips it!

  s.description      = <<-DESC
This email library is designed to be used as a backend for a UI.
                       DESC

  s.homepage         = 'https://github.com/albert-yu/morse'
  # s.screenshots     = 'www.example.com/screenshots_1', 'www.example.com/screenshots_2'
  s.license          = { :type => 'MIT', :file => 'LICENSE' }
  s.author           = { 'albert-yu' => 'albert.yu@gmail.com' }
  s.source           = { :git => 'https://github.com/albert-yu/morse.git', :tag => s.version.to_s }
  # s.social_media_url = 'https://twitter.com/<TWITTER_USERNAME>'

  s.platform = :osx, '10.14'

# -framework CoreFoundation -framework Security -lldap -lz

  s.requires_arc = false
  s.static_framework = true
  s.frameworks = 'CoreFoundation', 'Security'
  s.source_files = 'include/**/*.h'
  s.public_header_files = 'include/*.h'  
  s.vendored_libraries = 'libmorse.a', 'lib/libcurl.a', 'lib/libsodium.a' 
  s.library = 'morse', 'curl', 'sodium', 'ldap', 'z'
  
  
  # s.dependency 'AFNetworking', '~> 2.3'
  
  s.prepare_command = <<-CMD
                  make clean
                  make 
                 CMD

end
