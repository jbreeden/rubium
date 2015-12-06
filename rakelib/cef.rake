module CEF
  @dir = File.expand_path Dir['cef_distros/cef_binary*'].select { |f| File.directory?(f) }.last

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
      if CEF.dir.end_with?('64')
        sh("cmake -G \"Xcode\" -DPROJECT_ARCH=\"x86_64\" #{CEF.dir}")
      else
        sh("cmake -G \"Xcode\" -DPROJECT_ARCH=\"x86\" #{CEF.dir}")
      end
      sh("xcodebuild -project cef.xcodeproj -target #{target} -configuration #{configuration}")
    end
  end
elsif OS.linux?
  def build_cef_target(target, configuration)
    mkdir CEF.build_dir unless Dir.exists?(CEF.build_dir)
    cd CEF.build_dir do
      if CEF.dir.end_with?('64')
        sh("cmake -DPROJECT_ARCH=\"x86_64\" #{CEF.dir} -DCONFIGURATION=#{configuration}")
      else
        sh("cmake -DPROJECT_ARCH=\"x86\" #{CEF.dir} -DCONFIGURATION=#{configuration}")
      end
      sh("make")
    end
  end
else
  def build_cef_target(target, configuration)
    cd CEF.dir do
      if CEF.dir.end_with?('64')
        sh("msbuild cefclient2010.sln /property:Configuration=Release /property:Platform=X64")
      else
        sh("msbuild cefclient2010.sln /property:Configuration=Release /property:Platform=X86")
      end
    end
  end
end
