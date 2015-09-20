module OS
  def OS.windows?
    (/cygwin|mswin|mingw|bccwin|wince|emx/ =~ RUBY_PLATFORM) != nil
  end

  def OS.mac?
   (/darwin/ =~ RUBY_PLATFORM) != nil
  end

  def OS.unix?
    !OS.windows?
  end

  def OS.linux?
    OS.unix? and not OS.mac?
  end
end

namespace :mruby do
  desc "Build the mruby bundled with rubium"
  task :build do
    Dir.chdir "mruby" do
      ENV['CEF_HOME'] = CEF.dir
      sh "ruby minirake"
    end

    # Hack:
    # Something is going wrong in mruby build process...
    # Not getting the .exe extensions for executables.
    # I'll try to fix this later
    if ENV['OS'] =~ /windows/i
      Dir.chdir "mruby/bin" do
        mv 'mirb', 'mirb.exe' if File.exists?('mirb')
        mv 'mruby', 'mruby.exe' if File.exists?('mruby')
        mv 'mrbc', 'mrbc.exe' if File.exists?('mrbc')
        mv 'mruby-strip', 'mruby-strip.exe' if File.exists?('mruby')
        mv 'rubium', 'rubium.exe' if File.exists?('rubium')
        mv 'rubiumw', 'rubiumw.exe' if File.exists?('rubiumw')
      end
    end
  end

  desc "Clean the mruby bundled with rubium"
  task :clean do
    Dir.chdir "mruby" do
      sh "ruby minirake clean"
    end
  end
end

task :binstubs do
  if OS.mac?
  rubium = <<EOS
#! /usr/bin/env ruby
spawn "#{File.expand_path('.')}/rubium.app/Contents/MacOS/rubium", *ARGV
EOS
  elsif OS.linux?
    rubium = <<EOS
#! /usr/bin/env ruby
spawn "#{File.expand_path('.')}/rubium/rubium", *ARGV
EOS
  end

  File.open("/usr/local/bin/rubium", 'w') do |stub|
    stub.write(rubium)
    stub.chmod(0755)
  end
end
