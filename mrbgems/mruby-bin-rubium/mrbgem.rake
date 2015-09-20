require 'fileutils'

module RubiumBinGem
  def self.dir
    File.expand_path(File.dirname(__FILE__))
  end

  def self.include_dir
    "#{self.dir}/include"
  end
end

MRuby::Gem::Specification.new('mruby-bin-rubium') do |spec|
  spec.license = 'MIT'
  spec.author  = 'Jared Breeden'
  spec.summary = 'A Chromium shell for mruby apps'

  configuration = ENV['CONFIGURATION'] || 'Release'

  spec.cxx.include_paths << "#{RubiumGem.include_dir}"
  spec.cxx.include_paths << ENV['CEF_HOME']
  spec.bins = ['rubium']

  # Clear the tools folder so we can copy over the right file for the current platform
  FileUtils.rm_rf("#{RubiumBinGem.dir}/tools/rubium") if Dir.exists?("#{RubiumBinGem.dir}/tools/rubium")
  FileUtils.mkdir_p("#{RubiumBinGem.dir}/tools/rubium")

  if OS.mac?
    spec.bins << "rubium Helper"

    FileUtils.rm_rf("#{RubiumBinGem.dir}/tools/rubium Helper") if Dir.exists?("#{RubiumBinGem.dir}/tools/rubium Helper")
    FileUtils.mkdir_p("#{RubiumBinGem.dir}/tools/rubium Helper")

    # MRuby is going to default the language standard to gnu99, we
    # need to remove this since we're actually compiling objective c
    spec.cxx.flags = spec.cxx.flags.reject { |f| f =~ /-std=/ }

    # MRuby doesn't do anything with .mm files, so I'm using .cpp and specifying the language with the -x flag
    FileUtils.cp "#{RubiumBinGem.dir}/platform-bins/mac/rubium.cpp", "#{RubiumBinGem.dir}/tools/rubium"
    FileUtils.cp "#{RubiumBinGem.dir}/platform-bins/mac/rubium Helper.cpp", "#{RubiumBinGem.dir}/tools/rubium Helper"

    spec.cxx.flags << '-x objective-c++'
    spec.linker.flags << "-F#{ENV['CEF_HOME']}/#{configuration}"
    spec.linker.flags << "-framework \"Chromium Embedded Framework\""
    spec.linker.flags << '-framework Cocoa'
    # Make sure install_name_tool will have enough space to alter the paths of the dynamic libraries
    spec.linker.flags << '-headerpad_max_install_names'
    (spec.linker.flags_after_libraries = []) << "#{ENV['CEF_HOME']}/build/libcef_dll/#{configuration}/libcef_dll_wrapper.a"
  elsif OS.linux?
    FileUtils.cp "#{RubiumBinGem.dir}/platform-bins/lin/rubium.cpp", "#{RubiumBinGem.dir}/tools/rubium"
    spec.linker.flags << '-Wl,-rpath,\'$ORIGIN\''
    spec.linker.library_paths << "#{ENV['CEF_HOME']}/#{configuration}"
    spec.linker.libraries << 'X11'
    spec.linker.libraries << 'cef'
    (spec.linker.flags_after_libraries = []) << "#{ENV['CEF_HOME']}/build/libcef_dll/libcef_dll_wrapper.a"
  elsif OS.windows?
    FileUtils.cp "#{RubiumBinGem.dir}/platform-bins/win/rubium.cpp", "#{RubiumBinGem.dir}/tools/rubium"
    (spec.linker.flags_before_libraries = []) << "#{ENV['CEF_HOME']}/out/#{configuration}/lib/libcef_dll_wrapper.lib"
    spec.linker.library_paths << "#{ENV['CEF_HOME']}/#{configuration}"
    spec.linker.libraries << "User32"
    spec.linker.libraries << "libcef"
  end
end
