module CEF
  @dir = File.expand_path Dir['**/cef_binary*'].select { |f| File.directory?(f) }.last

  class << self
    attr_reader :dir
  end

  def self.build_dir
    "#{self.dir}/build"
  end

  def self.x86?
    dir.end_with?('32')
  end

  def self.x86_64?
    dir.end_with?('64')
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
    arch = CEF.x86? ? 'x86' : 'x86_64'
    cd CEF.build_dir do
      sh("cmake -G \"Xcode\" -DPROJECT_ARCH=\"#{arch}\" #{CEF.dir}")
      sh("xcodebuild -project cef.xcodeproj -target #{target} -configuration #{configuration}")
    end
  end

elsif OS.linux?

  def build_cef_target(target, configuration)
    mkdir CEF.build_dir unless Dir.exists?(CEF.build_dir)
    cd CEF.build_dir do
      arch = CEF.x86? ? 'x86' : 'x86_64'
      sh("cmake -DPROJECT_ARCH=\"#{arch}\" #{CEF.dir} -DCONFIGURATION=#{configuration}")
      sh("make")
    end
  end

else

  def build_cef_target(target, configuration)
    cd CEF.dir do
      platform = CEF.x86? ? 'X86' : 'X64'
      sh("msbuild cefclient2010.sln /property:Configuration=Release /property:Platform=#{platform}")
    end
  end

end
