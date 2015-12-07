$mruby_cef_gem_dir = File.expand_path(File.dirname(__FILE__))

MRuby::Gem::Specification.new('mruby-cef') do |spec|
  spec.license = 'MIT'
  spec.author  = 'Jared Breeden'
  spec.summary = 'Bindings to the CEF libraries'

  spec.cxx.flags << [ '-std=c++11' ]
  if ENV['CEF_HOME']
    spec.cc.include_paths << ENV['CEF_HOME']
    spec.cxx.include_paths << ENV['CEF_HOME']
  end
  spec.cc.include_paths << "#{$mruby_cef_gem_dir}/include"
  spec.cxx.include_paths << "#{$mruby_cef_gem_dir}/include"
end
