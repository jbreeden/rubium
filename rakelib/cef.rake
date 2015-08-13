module CEF
  if OS.mac?
    @dir = File.expand_path "cef_distros/cef_binary_3.2171.1979_macosx64"
  elsif OS.linux?
    @dir = File.expand_path "cef_distros/cef_binary_3.2171.1979_linux64"
  else
    raise 'Please define CEF dir for Windows'
  end

  class << self
    attr_reader :dir
  end

  def self.build_dir
    "#{self.dir}/build"
  end
end

namespace :cef do
  desc "Build cefclient"
  task :cefclient do
    configuration = ENV['CONFIGURATION'] || 'Release'
    build_cef_target('cefclient', configuration)
  end
end

if OS.mac?
  def build_cef_target(target, configuration)
    mkdir CEF.build_dir unless Dir.exists?(CEF.build_dir)
    cd CEF.build_dir do
      sh("cmake -G \"Xcode\" -DPROJECT_ARCH=\"x86_64\" #{CEF.dir}")
      sh("xcodebuild -project cef.xcodeproj -target #{target} -configuration #{configuration}")
    end
  end
elsif OS.linux?
  def build_cef_target(target)
    mkdir CEF.build_dir unless Dir.exists?(CEF.build_dir)
    cd CEF.build_dir do
      sh("cmake -DPROJECT_ARCH=\"x86_64\" #{CEF.dir} -DCONFIGURATION=#{configuration}")
      sh("make")
    end
  end
else
  def build_cef_target(target)
    raise "Not defined for this platform"
  end
end
