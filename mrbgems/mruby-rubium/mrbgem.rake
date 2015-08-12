require 'fileutils'

module RubiumGem
  def self.dir
    File.expand_path(File.dirname(__FILE__))
  end

  def self.include_dir
    "#{self.dir}/include"
  end
end

MRuby::Gem::Specification.new('mruby-rubium') do |spec|
  spec.license = 'MIT'
  spec.author  = 'Jared Breeden'
  spec.summary = 'A Chromium shell for mruby apps'

  spec.cc.include_paths << ENV['CEF_HOME']
  spec.cxx.include_paths << ENV['CEF_HOME']
  spec.build.cc.include_paths << "#{RubiumGem.dir}/include"
  spec.build.cxx.include_paths << "#{RubiumGem.dir}/include"
  spec.cc.include_paths << "#{RubiumGem.dir}/../mruby-cef/include"
  spec.cxx.include_paths << "#{RubiumGem.dir}/../mruby-cef/include"
  spec.cc.flags << [ '-std=c11' ]
  spec.cxx.flags << [ '-std=c++11' ]
end
