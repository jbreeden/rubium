module Rubium
  if Dir.exists?('/opt/rubium/js_extensions')
    @js_extensions = Dir.entries('/opt/rubium/js_extensions').reject { |f|
      f =~ /^\.\.?$/ || !File.file?("/opt/rubium/js_extensions/#{f}")
    }.map { |f|
      "/opt/rubium/js_extensions/#{f}"
    }
  else
    @js_extensions = []
  end
end

if File.exists?('./rubium.rb')
  load './rubium.rb'
end
